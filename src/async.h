#ifndef DIFF_ASYNC_H_
#define DIFF_ASYNC_H_

#include <napi.h>

Napi::Value AllDiffAsync(const Napi::CallbackInfo &info);

Napi::Value MaskDiffAsync(const Napi::CallbackInfo &info);

#endif