#include "diff_object.h"
#include <napi.h>

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Example::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)