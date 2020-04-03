//
// Created by CMO on 4/2/20.
//

#ifndef PJPROJECT_GO_NATIVE_RESAMPLE_BY_2_H
#define PJPROJECT_GO_NATIVE_RESAMPLE_BY_2_H

#include "signal_processing_library.h"

void WebRtcSpl_DownsampleBy2(const int16_t* in, size_t len,
                             int16_t* out, int32_t* filtState);

void WebRtcSpl_UpsampleBy2(const int16_t* in, size_t len,
                           int16_t* out, int32_t* filtState);

#endif //PJPROJECT_GO_NATIVE_RESAMPLE_BY_2_H
