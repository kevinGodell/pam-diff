#ifndef DIFF_SYNC_H_
#define DIFF_SYNC_H_

#include <napi.h>

Napi::Array DiffAllSync(const Napi::CallbackInfo &info);

Napi::Array DiffMaskSync(const Napi::CallbackInfo &info);

#endif
