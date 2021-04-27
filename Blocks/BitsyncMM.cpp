/* -*- c++ -*- */
/*
 * Copyright 2004,2010-2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#include "BitsyncMM.h"
#include "../Sources/math.h"
#include "interpolator_taps.h"
#include <stdexcept>


BitsyncMM::BitsyncMM(
        std::string name, float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit)
    : BasicBlock(name, 1, sizeof(float), 1, sizeof(float)),
      d_mu(mu),
      d_gain_mu(gain_mu),
      d_gain_omega(gain_omega),
      d_omega_relative_limit(omega_relative_limit),
      d_last_sample(0)
{
    if (omega < 1)
        throw std::out_of_range("clock rate must be > 0");
    if (gain_mu < 0 || gain_omega < 0)
        throw std::out_of_range("Gains must be non-negative");

    set_omega(omega); // also sets min and max omega
    SetDecimation(omega);
//    set_inverse_relative_rate(omega);
//    enable_update_rate(true); // fixes tag propagation through variable rate block

    filters.reserve(NSTEPS + 1);
    for (int i = 0; i < NSTEPS + 1; i++) {
        std::vector<float> t(&taps[i][0], &taps[i][NTAPS]);
        filters.emplace_back(t);
    }

}

BitsyncMM::~BitsyncMM() {}

void BitsyncMM::forecast(int noutput_items, int& ninput_items_required)
{
    ninput_items_required = (int)ceil((noutput_items * d_omega) + NTAPS);
}

void BitsyncMM::set_omega(float omega)
{
    d_omega = omega;
    d_omega_mid = omega;
    d_omega_lim = d_omega_mid * d_omega_relative_limit;
}

int BitsyncMM::work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output)
{
    const float* in = (const float*)input[0];
    float* out = (float*)output[0];

    int ninput_items = noutput*d_omega;
    int ii = 0;                                  // input index
    int oo = 0;                                  // output index
    int ni = ninput_items; // don't use more input than this
    float mm_val;

    while (oo < noutput && ii < ni) {
        // produce output sample
//        out[oo] = d_interp.interpolate(&in[ii], d_mu);
        int imu = (int)rint(d_mu * NSTEPS);

        if ((imu < 0) || (imu > NSTEPS)) {
            throw std::runtime_error("mmse_fir_interpolator_ff: imu out of bounds.");
        }

        float r = 0.0;//filters[imu].filter(input);
        for (int j=ii; j<ii+NTAPS; ++j)
        {
            r += filters[imu][7-(j-ii)]*in[j];
        }
        out[oo] = r;
        mm_val = slice(d_last_sample) * out[oo] - slice(out[oo]) * d_last_sample;
        d_last_sample = out[oo];

        d_omega = d_omega + d_gain_omega * mm_val;
        d_omega = d_omega_mid + branchless_clip(d_omega - d_omega_mid, d_omega_lim);
        d_mu = d_mu + d_omega + d_gain_mu * mm_val;

        ii += (int)floor(d_mu);
        d_mu = d_mu - floor(d_mu);
        oo++;
    }

    ninput = ii;
    return oo;
}

