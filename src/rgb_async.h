#ifndef SRC_RGB_ASYNC_H_
#define SRC_RGB_ASYNC_H_

#include "diff.h"
#include <napi.h>

Napi::Value RgbDiffAllAsync(const Napi::CallbackInfo &info);

Napi::Value RgbDiffMaskAsync(const Napi::CallbackInfo &info);

Napi::Value RgbDiffRegionsAsync(const Napi::CallbackInfo &info);

#endif