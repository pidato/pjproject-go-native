/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


/*
 * This header file contains some internal resampling functions.
 *
 */

#ifndef COMMON_AUDIO_SIGNAL_PROCESSING_RESAMPLE_BY_2_INTERNAL_H_
#define COMMON_AUDIO_SIGNAL_PROCESSING_RESAMPLE_BY_2_INTERNAL_H_

#include "../common.h"

// C + the 32 most significant bits of A * B
#define WEBRTC_SPL_SCALEDIFF32(A, B, C) \
  (C + (B >> 16) * A + (((uint32_t)(B & 0x0000FFFF) * A) >> 16))

/*******************************************************************
 * resample_by_2_fast.c
 * Functions for internal use in the other resample functions
 ******************************************************************/
void WebRtcSpl_DownBy2IntToShort(int32_t *in, int32_t len, int16_t *out,
                                 int32_t *state);

void WebRtcSpl_DownBy2ShortToInt(const int16_t *in, int32_t len,
                                 int32_t *out, int32_t *state);

void WebRtcSpl_LPBy2IntToInt(const int32_t* in, int32_t len, int32_t* out,
                             int32_t* state);

void WebRtcSpl_DownsampleBy2(const int16_t* in, size_t len,
                             int16_t* out, int32_t* filtState);

void WebRtcSpl_UpsampleBy2(const int16_t* in, size_t len,
                           int16_t* out, int32_t* filtState);

void WebRtcSpl_Resample32khzTo24khz(const int32_t *In, int32_t *Out, size_t K);

void WebRtcSpl_Resample48khzTo32khz(const int32_t *In, int32_t *Out, size_t K);

void WebRtcSpl_Resample44khzTo32khz(const int32_t *In, int32_t *Out, size_t K);

#endif  // COMMON_AUDIO_SIGNAL_PROCESSING_RESAMPLE_BY_2_INTERNAL_H_
