#ifndef DIFF_SYNC_H_
#define DIFF_SYNC_H_

#include <napi.h>

Napi::Array AllDiffSync(const Napi::CallbackInfo &info);

Napi::Array MaskDiffSync(const Napi::CallbackInfo &info);

#endif
