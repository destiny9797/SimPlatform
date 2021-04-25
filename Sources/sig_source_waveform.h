/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SIG_SOURCE_WAVEFORM_H
#define INCLUDED_SIG_SOURCE_WAVEFORM_H

/*!
 * \brief Types of signal generator waveforms.
 * \ingroup waveform_generators_blk
 */
typedef enum {
    CONST_WAVE = 100,
    SIN_WAVE,
    COS_WAVE,
    SQR_WAVE,
    TRI_WAVE,
    SAW_WAVE
} waveform_t;


#endif /* INCLUDED_SIG_SOURCE_WAVEFORM_H */
