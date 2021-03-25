/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SIG_SOURCE_H
#define SIG_SOURCE_H

#include "sig_source_waveform.h"
#include "fxpt_nco.h"
#include "math.h"
#include "../BasicBlock.h"
#include <algorithm>
#include <stdexcept>


template <class T>
class sig_source : public BasicBlock
{
private:
    double d_sampling_freq;
    waveform_t d_waveform;
    double d_frequency;
    double d_ampl;
    T d_offset;
    fxpt_nco d_nco;

public:
    sig_source(double sampling_freq,
                    waveform_t waveform,
                    double wave_freq,
                    double ampl,
                    T offset = 0,
                    float phase = 0);
    ~sig_source() override;

    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput_items,
             std::vector<const void*>& input_items,
             std::vector<void*>& output_items) override;

    double sampling_freq() const  { return d_sampling_freq; }
    waveform_t waveform() const  { return d_waveform; }
    double frequency() const  { return d_frequency; }
    double amplitude() const  { return d_ampl; }
    T offset() const  { return d_offset; }
    float phase() const  { return d_nco.get_phase(); }

    void set_sampling_freq(double sampling_freq) ;
    void set_waveform(waveform_t waveform) ;


    void set_frequency(double frequency) ;
    void set_amplitude(double ampl) ;
    void set_offset(T offset) ;
    void set_phase(float phase) ;
};



// -------------------------------------------------------
// ------------------- implementation --------------------
// -------------------------------------------------------
template <class T>
sig_source<T>::sig_source(double sampling_freq,
                                    waveform_t waveform,
                                    double frequency,
                                    double ampl,
                                    T offset,
                                    float phase)
        : BasicBlock(0,0,1,sizeof(T)),
          d_sampling_freq(sampling_freq),
          d_waveform(waveform),
          d_frequency(frequency),
          d_ampl(ampl),
          d_offset(offset)
{
    this->set_frequency(frequency);
    this->set_phase(phase);
}

template <class T>
sig_source<T>::~sig_source()
{
}

template <class T>
void sig_source<T>::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}


template <class T>
int sig_source<T>::work(int noutput_items,
                             std::vector<const void*>& input_items,
                             std::vector<void*>& output_items)
{
    T* optr = (T*)output_items[0];
    T t;

    switch (d_waveform) {
        case CONST_WAVE:
            t = (T)d_ampl + d_offset;
            std::fill_n(optr, noutput_items, t);
            break;

        case SIN_WAVE:
            d_nco.sin(optr, noutput_items, d_ampl);
            if (d_offset == 0)
                break;

            for (int i = 0; i < noutput_items; i++) {
                optr[i] += d_offset;
            }
            break;

        case COS_WAVE:
            d_nco.cos(optr, noutput_items, d_ampl);
            if (d_offset == 0)
                break;

            for (int i = 0; i < noutput_items; i++) {
                optr[i] += d_offset;
            }
            break;

            /* The square wave is high from -PI to 0. */
        case SQR_WAVE:
            t = (T)d_ampl + d_offset;
            for (int i = 0; i < noutput_items; i++) {
                if (d_nco.get_phase() < 0)
                    optr[i] = t;
                else
                    optr[i] = d_offset;
                d_nco.step();
            }
            break;

            /* The triangle wave rises from -PI to 0 and falls from 0 to PI. */
        case TRI_WAVE:
            for (int i = 0; i < noutput_items; i++) {
                double t = d_ampl * d_nco.get_phase() / M_PI;
                if (d_nco.get_phase() < 0)
                    optr[i] = static_cast<T>(t + d_ampl + d_offset);
                else
                    optr[i] = static_cast<T>(-1 * t + d_ampl + d_offset);
                d_nco.step();
            }
            break;

            /* The saw tooth wave rises from -PI to PI. */
        case SAW_WAVE:
            for (int i = 0; i < noutput_items; i++) {
                t = static_cast<T>(d_ampl * d_nco.get_phase() / (2 * M_PI) + d_ampl / 2 +
                                   d_offset);
                optr[i] = t;
                d_nco.step();
            }
            break;
        default:
            throw std::runtime_error("analog::sig_source: invalid waveform");
    }

    return noutput_items;
}


template <>
int sig_source<std::complex<float>>::work(int noutput_items,
                                               std::vector<const void*>& input_items,
                                               std::vector<void*>& output_items)
{
    std::complex<float>* optr = (std::complex<float>*)output_items[0];
    std::complex<float> t;

    switch (d_waveform) {
        case CONST_WAVE:
            t = (std::complex<float>)d_ampl + d_offset;
            std::fill_n(optr, noutput_items, t);
            break;

        case SIN_WAVE:
        case COS_WAVE:
            d_nco.sincos(optr, noutput_items, d_ampl);
            if (d_offset == std::complex<float>(0, 0))
                break;

            for (int i = 0; i < noutput_items; i++) {
                optr[i] += d_offset;
            }
            break;

            /* Implements a real square wave high from -PI to 0.
             * The imaginary square wave leads by 90 deg.
             */
        case SQR_WAVE:
            for (int i = 0; i < noutput_items; i++) {
                if (d_nco.get_phase() < -1 * M_PI / 2)
                    optr[i] = std::complex<float>(d_ampl, 0) + d_offset;
                else if (d_nco.get_phase() < 0)
                    optr[i] = std::complex<float>(d_ampl, d_ampl) + d_offset;
                else if (d_nco.get_phase() < M_PI / 2)
                    optr[i] = std::complex<float>(0, d_ampl) + d_offset;
                else
                    optr[i] = d_offset;
                d_nco.step();
            }
            break;

            /* Implements a real triangle wave rising from -PI to 0 and
             * falling from 0 to PI. The imaginary triangle wave leads by
             * 90 deg.
             */
        case TRI_WAVE:
            for (int i = 0; i < noutput_items; i++) {
                if (d_nco.get_phase() < -1 * M_PI / 2) {
                    optr[i] =
                            std::complex<float>(d_ampl * d_nco.get_phase() / M_PI + d_ampl,
                                                -1 * d_ampl * d_nco.get_phase() / M_PI - d_ampl / 2) +
                            d_offset;
                } else if (d_nco.get_phase() < 0) {
                    optr[i] = std::complex<float>(d_ampl * d_nco.get_phase() / M_PI + d_ampl,
                                                  d_ampl * d_nco.get_phase() / M_PI + d_ampl / 2) +
                              d_offset;
                } else if (d_nco.get_phase() < M_PI / 2) {
                    optr[i] = std::complex<float>(-1 * d_ampl * d_nco.get_phase() / M_PI + d_ampl,
                                                  d_ampl * d_nco.get_phase() / M_PI + d_ampl / 2) +
                              d_offset;
                } else {
                    optr[i] = std::complex<float>(-1 * d_ampl * d_nco.get_phase() / M_PI + d_ampl,
                                                  -1 * d_ampl * d_nco.get_phase() / M_PI +
                                                  3 * d_ampl / 2) +
                              d_offset;
                }
                d_nco.step();
            }
            break;

            /* Implements a real saw tooth wave rising from -PI to PI.
             * The imaginary saw tooth wave leads by 90 deg.
             */
        case SAW_WAVE:
            for (int i = 0; i < noutput_items; i++) {
                if (d_nco.get_phase() < -1 * M_PI / 2) {
                    optr[i] =
                            std::complex<float>(d_ampl * d_nco.get_phase() / (2 * M_PI) + d_ampl / 2,
                                                d_ampl * d_nco.get_phase() / (2 * M_PI) +
                                                5 * d_ampl / 4) +
                            d_offset;
                } else {
                    optr[i] =
                            std::complex<float>(d_ampl * d_nco.get_phase() / (2 * M_PI) + d_ampl / 2,
                                                d_ampl * d_nco.get_phase() / (2 * M_PI) + d_ampl / 4) +
                            d_offset;
                }
                d_nco.step();
            }
            break;
        default:
            throw std::runtime_error("analog::sig_source: invalid waveform");
    }

    return noutput_items;
}


template <class T>
void sig_source<T>::set_sampling_freq(double sampling_freq)
{
    d_sampling_freq = sampling_freq;
    d_nco.set_freq(2 * M_PI * this->d_frequency / this->d_sampling_freq);
}

template <class T>
void sig_source<T>::set_waveform(waveform_t waveform)
{
    d_waveform = waveform;
}

template <class T>
void sig_source<T>::set_frequency(double frequency)
{
    d_frequency = frequency;
    d_nco.set_freq(2 * M_PI * this->d_frequency / this->d_sampling_freq);
}

template <class T>
void sig_source<T>::set_amplitude(double ampl)
{
    d_ampl = ampl;
}

template <class T>
void sig_source<T>::set_offset(T offset)
{
    d_offset = offset;
}

template <class T>
void sig_source<T>::set_phase(float phase)
{
    d_nco.set_phase(phase);
}


#endif /* SIG_SOURCE_H */
