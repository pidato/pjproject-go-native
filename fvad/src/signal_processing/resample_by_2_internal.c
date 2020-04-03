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

#include "resample_by_2_internal.h"

// allpass filter coefficients.
static const int16_t kResampleAllpass[2][3] = {
        {821, 6110, 12382},
        {3050, 9368, 15063}
};

//
//   decimator
// input:  int32_t (shifted 15 positions to the left, + offset 16384) OVERWRITTEN!
// output: int16_t (saturated) (of length len/2)
// state:  filter state array; length = 8

void RTC_NO_SANITIZE("signed-integer-overflow")  // bugs.webrtc.org/5486
WebRtcSpl_DownBy2IntToShort(int32_t *in, int32_t len, int16_t *out,
                            int32_t *state)
{
    int32_t tmp0, tmp1, diff;
    int32_t i;

    len >>= 1;

    // lower allpass filter (operates on even input samples)
    for (i = 0; i < len; i++)
    {
        tmp0 = in[i << 1];
        diff = tmp0 - state[1];
        // UBSan: -1771017321 - 999586185 cannot be represented in type 'int'

        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[0] + diff * kResampleAllpass[1][0];
        state[0] = tmp0;
        diff = tmp1 - state[2];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[1] + diff * kResampleAllpass[1][1];
        state[1] = tmp1;
        diff = tmp0 - state[3];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[3] = state[2] + diff * kResampleAllpass[1][2];
        state[2] = tmp0;

        // divide by two and store temporarily
        in[i << 1] = (state[3] >> 1);
    }

    in++;

    // upper allpass filter (operates on odd input samples)
    for (i = 0; i < len; i++)
    {
        tmp0 = in[i << 1];
        diff = tmp0 - state[5];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[4] + diff * kResampleAllpass[0][0];
        state[4] = tmp0;
        diff = tmp1 - state[6];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[5] + diff * kResampleAllpass[0][1];
        state[5] = tmp1;
        diff = tmp0 - state[7];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[7] = state[6] + diff * kResampleAllpass[0][2];
        state[6] = tmp0;

        // divide by two and store temporarily
        in[i << 1] = (state[7] >> 1);
    }

    in--;

    // combine allpass outputs
    for (i = 0; i < len; i += 2)
    {
        // divide by two, add both allpass outputs and round
        tmp0 = (in[i << 1] + in[(i << 1) + 1]) >> 15;
        tmp1 = (in[(i << 1) + 2] + in[(i << 1) + 3]) >> 15;
        if (tmp0 > (int32_t)0x00007FFF)
            tmp0 = 0x00007FFF;
        if (tmp0 < (int32_t)0xFFFF8000)
            tmp0 = 0xFFFF8000;
        out[i] = (int16_t)tmp0;
        if (tmp1 > (int32_t)0x00007FFF)
            tmp1 = 0x00007FFF;
        if (tmp1 < (int32_t)0xFFFF8000)
            tmp1 = 0xFFFF8000;
        out[i + 1] = (int16_t)tmp1;
    }
}

//
//   decimator
// input:  int16_t
// output: int32_t (shifted 15 positions to the left, + offset 16384) (of length len/2)
// state:  filter state array; length = 8

void RTC_NO_SANITIZE("signed-integer-overflow")  // bugs.webrtc.org/5486
WebRtcSpl_DownBy2ShortToInt(const int16_t *in,
                            int32_t len,
                            int32_t *out,
                            int32_t *state)
{
    int32_t tmp0, tmp1, diff;
    int32_t i;

    len >>= 1;

    // lower allpass filter (operates on even input samples)
    for (i = 0; i < len; i++)
    {
        tmp0 = ((int32_t)in[i << 1] << 15) + (1 << 14);
        diff = tmp0 - state[1];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[0] + diff * kResampleAllpass[1][0];
        state[0] = tmp0;
        diff = tmp1 - state[2];
        // UBSan: -1379909682 - 834099714 cannot be represented in type 'int'

        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[1] + diff * kResampleAllpass[1][1];
        state[1] = tmp1;
        diff = tmp0 - state[3];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[3] = state[2] + diff * kResampleAllpass[1][2];
        state[2] = tmp0;

        // divide by two and store temporarily
        out[i] = (state[3] >> 1);
    }

    in++;

    // upper allpass filter (operates on odd input samples)
    for (i = 0; i < len; i++)
    {
        tmp0 = ((int32_t)in[i << 1] << 15) + (1 << 14);
        diff = tmp0 - state[5];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[4] + diff * kResampleAllpass[0][0];
        state[4] = tmp0;
        diff = tmp1 - state[6];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[5] + diff * kResampleAllpass[0][1];
        state[5] = tmp1;
        diff = tmp0 - state[7];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[7] = state[6] + diff * kResampleAllpass[0][2];
        state[6] = tmp0;

        // divide by two and store temporarily
        out[i] += (state[7] >> 1);
    }

    in--;
}


//   lowpass filter
// input:  int32_t (shifted 15 positions to the left, + offset 16384)
// output: int32_t (normalized, not saturated)
// state:  filter state array; length = 8
void RTC_NO_SANITIZE("signed-integer-overflow")  // bugs.webrtc.org/5486
WebRtcSpl_LPBy2IntToInt(const int32_t* in, int32_t len, int32_t* out,
                        int32_t* state)
{
    int32_t tmp0, tmp1, diff;
    int32_t i;

    len >>= 1;

    // lower allpass filter: odd input -> even output samples
    in++;
    // initial state of polyphase delay element
    tmp0 = state[12];
    for (i = 0; i < len; i++)
    {
        diff = tmp0 - state[1];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[0] + diff * kResampleAllpass[1][0];
        state[0] = tmp0;
        diff = tmp1 - state[2];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[1] + diff * kResampleAllpass[1][1];
        state[1] = tmp1;
        diff = tmp0 - state[3];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[3] = state[2] + diff * kResampleAllpass[1][2];
        state[2] = tmp0;

        // scale down, round and store
        out[i << 1] = state[3] >> 1;
        tmp0 = in[i << 1];
    }
    in--;

    // upper allpass filter: even input -> even output samples
    for (i = 0; i < len; i++)
    {
        tmp0 = in[i << 1];
        diff = tmp0 - state[5];
        // UBSan: -794814117 - 1566149201 cannot be represented in type 'int'

        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[4] + diff * kResampleAllpass[0][0];
        state[4] = tmp0;
        diff = tmp1 - state[6];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[5] + diff * kResampleAllpass[0][1];
        state[5] = tmp1;
        diff = tmp0 - state[7];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[7] = state[6] + diff * kResampleAllpass[0][2];
        state[6] = tmp0;

        // average the two allpass outputs, scale down and store
        out[i << 1] = (out[i << 1] + (state[7] >> 1)) >> 15;
    }

    // switch to odd output samples
    out++;

    // lower allpass filter: even input -> odd output samples
    for (i = 0; i < len; i++)
    {
        tmp0 = in[i << 1];
        diff = tmp0 - state[9];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[8] + diff * kResampleAllpass[1][0];
        state[8] = tmp0;
        diff = tmp1 - state[10];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[9] + diff * kResampleAllpass[1][1];
        state[9] = tmp1;
        diff = tmp0 - state[11];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[11] = state[10] + diff * kResampleAllpass[1][2];
        state[10] = tmp0;

        // scale down, round and store
        out[i << 1] = state[11] >> 1;
    }

    // upper allpass filter: odd input -> odd output samples
    in++;
    for (i = 0; i < len; i++)
    {
        tmp0 = in[i << 1];
        diff = tmp0 - state[13];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[12] + diff * kResampleAllpass[0][0];
        state[12] = tmp0;
        diff = tmp1 - state[14];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        tmp0 = state[13] + diff * kResampleAllpass[0][1];
        state[13] = tmp1;
        diff = tmp0 - state[15];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0)
            diff += 1;
        state[15] = state[14] + diff * kResampleAllpass[0][2];
        state[14] = tmp0;

        // average the two allpass outputs, scale down and store
        out[i << 1] = (out[i << 1] + (state[15] >> 1)) >> 15;
    }
}





static __inline int16_t WebRtcSpl_SatW32ToW16(int32_t value32) {
    int16_t out16 = (int16_t) value32;
    if (value32 > 32767)
        out16 = 32767;
    else if (value32 < -32768)
        out16 = -32768;
    return out16;
}

#ifdef WEBRTC_ARCH_ARM_V7
// allpass filter coefficients.
static const uint32_t kResampleAllpass1[3] = {3284, 24441, 49528 << 15};
static const uint32_t kResampleAllpass2[3] =
  {12199, 37471 << 15, 60255 << 15};
// Multiply two 32-bit values and accumulate to another input value.
// Return: state + ((diff * tbl_value) >> 16)
static __inline int32_t MUL_ACCUM_1(int32_t tbl_value,
                                    int32_t diff,
                                    int32_t state) {
  int32_t result;
  __asm __volatile ("smlawb %0, %1, %2, %3": "=r"(result): "r"(diff),
                                   "r"(tbl_value), "r"(state));
  return result;
}
// Multiply two 32-bit values and accumulate to another input value.
// Return: Return: state + (((diff << 1) * tbl_value) >> 32)
//
// The reason to introduce this function is that, in case we can't use smlawb
// instruction (in MUL_ACCUM_1) due to input value range, we can still use
// smmla to save some cycles.
static __inline int32_t MUL_ACCUM_2(int32_t tbl_value,
                                    int32_t diff,
                                    int32_t state) {
  int32_t result;
  __asm __volatile ("smmla %0, %1, %2, %3": "=r"(result): "r"(diff << 1),
                                  "r"(tbl_value), "r"(state));
  return result;
}
#else
// allpass filter coefficients.
static const uint16_t kResampleAllpass1[3] = {3284, 24441, 49528};
static const uint16_t kResampleAllpass2[3] = {12199, 37471, 60255};
// Multiply a 32-bit value with a 16-bit value and accumulate to another input:
#define MUL_ACCUM_1(a, b, c) WEBRTC_SPL_SCALEDIFF32(a, b, c)
#define MUL_ACCUM_2(a, b, c) WEBRTC_SPL_SCALEDIFF32(a, b, c)
#endif  // WEBRTC_ARCH_ARM_V7
// decimator
#if !defined(MIPS32_LE)
void WebRtcSpl_DownsampleBy2(const int16_t* in, size_t len,
                             int16_t* out, int32_t* filtState) {
    int32_t tmp1, tmp2, diff, in32, out32;
    size_t i;
    register int32_t state0 = filtState[0];
    register int32_t state1 = filtState[1];
    register int32_t state2 = filtState[2];
    register int32_t state3 = filtState[3];
    register int32_t state4 = filtState[4];
    register int32_t state5 = filtState[5];
    register int32_t state6 = filtState[6];
    register int32_t state7 = filtState[7];
    for (i = (len >> 1); i > 0; i--) {
        // lower allpass filter
        in32 = (int32_t)(*in++) << 10;
        diff = in32 - state1;
        tmp1 = MUL_ACCUM_1(kResampleAllpass2[0], diff, state0);
        state0 = in32;
        diff = tmp1 - state2;
        tmp2 = MUL_ACCUM_2(kResampleAllpass2[1], diff, state1);
        state1 = tmp1;
        diff = tmp2 - state3;
        state3 = MUL_ACCUM_2(kResampleAllpass2[2], diff, state2);
        state2 = tmp2;
        // upper allpass filter
        in32 = (int32_t)(*in++) << 10;
        diff = in32 - state5;
        tmp1 = MUL_ACCUM_1(kResampleAllpass1[0], diff, state4);
        state4 = in32;
        diff = tmp1 - state6;
        tmp2 = MUL_ACCUM_1(kResampleAllpass1[1], diff, state5);
        state5 = tmp1;
        diff = tmp2 - state7;
        state7 = MUL_ACCUM_2(kResampleAllpass1[2], diff, state6);
        state6 = tmp2;
        // add two allpass outputs, divide by two and round
        out32 = (state3 + state7 + 1024) >> 11;
        // limit amplitude to prevent wrap-around, and write to output array
        *out++ = WebRtcSpl_SatW32ToW16(out32);
    }
    filtState[0] = state0;
    filtState[1] = state1;
    filtState[2] = state2;
    filtState[3] = state3;
    filtState[4] = state4;
    filtState[5] = state5;
    filtState[6] = state6;
    filtState[7] = state7;
}
#endif  // #if defined(MIPS32_LE)
void WebRtcSpl_UpsampleBy2(const int16_t* in, size_t len,
                           int16_t* out, int32_t* filtState) {
    int32_t tmp1, tmp2, diff, in32, out32;
    size_t i;
    register int32_t state0 = filtState[0];
    register int32_t state1 = filtState[1];
    register int32_t state2 = filtState[2];
    register int32_t state3 = filtState[3];
    register int32_t state4 = filtState[4];
    register int32_t state5 = filtState[5];
    register int32_t state6 = filtState[6];
    register int32_t state7 = filtState[7];
    for (i = len; i > 0; i--) {
        // lower allpass filter
        in32 = (int32_t)(*in++) << 10;
        diff = in32 - state1;
        tmp1 = MUL_ACCUM_1(kResampleAllpass1[0], diff, state0);
        state0 = in32;
        diff = tmp1 - state2;
        tmp2 = MUL_ACCUM_1(kResampleAllpass1[1], diff, state1);
        state1 = tmp1;
        diff = tmp2 - state3;
        state3 = MUL_ACCUM_2(kResampleAllpass1[2], diff, state2);
        state2 = tmp2;
        // round; limit amplitude to prevent wrap-around; write to output array
        out32 = (state3 + 512) >> 10;
        *out++ = WebRtcSpl_SatW32ToW16(out32);
        // upper allpass filter
        diff = in32 - state5;
        tmp1 = MUL_ACCUM_1(kResampleAllpass2[0], diff, state4);
        state4 = in32;
        diff = tmp1 - state6;
        tmp2 = MUL_ACCUM_2(kResampleAllpass2[1], diff, state5);
        state5 = tmp1;
        diff = tmp2 - state7;
        state7 = MUL_ACCUM_2(kResampleAllpass2[2], diff, state6);
        state6 = tmp2;
        // round; limit amplitude to prevent wrap-around; write to output array
        out32 = (state7 + 512) >> 10;
        *out++ = WebRtcSpl_SatW32ToW16(out32);
    }
    filtState[0] = state0;
    filtState[1] = state1;
    filtState[2] = state2;
    filtState[3] = state3;
    filtState[4] = state4;
    filtState[5] = state5;
    filtState[6] = state6;
    filtState[7] = state7;
}