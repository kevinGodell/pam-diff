#ifndef DIFF_ASYNC_H_
#define DIFF_ASYNC_H_

#include <napi.h>

Napi::Value DiffAllAsync(const Napi::CallbackInfo &info);

Napi::Value DiffMaskAsync(const Napi::CallbackInfo &info);

#endif