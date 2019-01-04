#ifndef SRC_RGB_SYNC_H_
#define SRC_RGB_SYNC_H_

#include "diff.h"
#include <napi.h>

Napi::Array RgbDiffAllSync(const Napi::CallbackInfo &info);

Napi::Array RgbDiffMaskSync(const Napi::CallbackInfo &info);

Napi::Array RgbDiffRegionsSync(const Napi::CallbackInfo &info);

#endif
