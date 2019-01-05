#ifndef SRC_RGB_SYNC_H_
#define SRC_RGB_SYNC_H_

#include "diff.h"
#include <napi.h>

Napi::Value RgbDiffAllSync(const Napi::CallbackInfo &info);

Napi::Value RgbDiffMaskSync(const Napi::CallbackInfo &info);

Napi::Value RgbDiffRegionsSync(const Napi::CallbackInfo &info);

#endif