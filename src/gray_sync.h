#ifndef SRC_GRAY_SYNC_H_
#define SRC_GRAY_SYNC_H_

#include "diff.h"
#include <napi.h>

Napi::Value GrayDiffAllSync(const Napi::CallbackInfo &info);

Napi::Value GrayDiffMaskSync(const Napi::CallbackInfo &info);

Napi::Value GrayDiffRegionsSync(const Napi::CallbackInfo &info);

#endif