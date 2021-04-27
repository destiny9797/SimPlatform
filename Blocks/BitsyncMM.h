/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BITSYNCMM_H
#define INCLUDED_BITSYNCMM_H

//#include <gnuradio/digital/clock_recovery_mm_ff.h>
//#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include "../BasicBlock.h"


class BitsyncMM : public BasicBlock
{
public:
    BitsyncMM(std::string name,
                              float omega,
                              float gain_omega,
                              float mu,
                              float gain_mu,
                              float omega_relative_limi);
    ~BitsyncMM() override;

    float mu()  { return d_mu; }
    float omega()  { return d_omega; }
    float gain_mu()  { return d_gain_mu; }
    float gain_omega()  { return d_gain_omega; }

    void set_verbose(bool verbose)  { d_verbose = verbose; }
    void set_gain_mu(float gain_mu)  { d_gain_mu = gain_mu; }
    void set_gain_omega(float gain_omega)  { d_gain_omega = gain_omega; }
    void set_mu(float mu)  { d_mu = mu; }
    void set_omega(float omega);

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    float d_mu;                   // fractional sample position [0.0, 1.0]
    float d_gain_mu;              // gain for adjusting mu
    float d_omega;                // nominal frequency
    float d_gain_omega;           // gain for adjusting omega
    float d_omega_relative_limit; // used to compute min and max omega
    float d_omega_mid;            // average omega
    float d_omega_lim;            // actual omega clipping limit

    float d_last_sample;
//    filter::mmse_fir_interpolator_ff d_interp;

    bool d_verbose;

    std::vector<std::vector<float>> filters;

    float slice(float x) { return x < 0 ? -1.0F : 1.0F; }
};


#endif /* INCLUDED_BITSYNCMM_H */
