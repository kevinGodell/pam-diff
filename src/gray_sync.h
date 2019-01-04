#ifndef SRC_GRAY_SYNC_H_
#define SRC_GRAY_SYNC_H_

#include "diff.h"
#include <napi.h>

Napi::Array GrayDiffAllSync(const Napi::CallbackInfo &info);

Napi::Array GrayDiffMaskSync(const Napi::CallbackInfo &info);

Napi::Array GrayDiffRegionsSync(const Napi::CallbackInfo &info);

#endif
