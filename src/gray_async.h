#ifndef SRC_GRAY_ASYNC_H_
#define SRC_GRAY_ASYNC_H_

#include "diff.h"
#include <napi.h>

Napi::Value GrayDiffAllAsync(const Napi::CallbackInfo &info);

Napi::Value GrayDiffMaskAsync(const Napi::CallbackInfo &info);

Napi::Value GrayDiffRegionsAsync(const Napi::CallbackInfo &info);

#endif