#include "gray_sync.h"
#include "gray_async.h"
#include <napi.h>

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "grayDiffAllSync"), Napi::Function::New(env, GrayDiffAllSync));
    exports.Set(Napi::String::New(env, "grayDiffAllAsync"), Napi::Function::New(env, GrayDiffAllAsync));
    exports.Set(Napi::String::New(env, "grayDiffMaskSync"), Napi::Function::New(env, GrayDiffMaskSync));
    //exports.Set(Napi::String::New(env, "grayDiffMaskAsync"), Napi::Function::New(env, GrayDiffMaskAsync));
    exports.Set(Napi::String::New(env, "grayDiffRegionsSync"), Napi::Function::New(env, GrayDiffRegionsSync));
    //exports.Set(Napi::String::New(env, "grayDiffRegionsAsync"), Napi::Function::New(env, GrayDiffRegionsAsync));
    /*exports.Set(Napi::String::New(env, "grayRegionsSync"), Napi::Function::New(env, GrayRegionsSync));
    exports.Set(Napi::String::New(env, "grayRegionsAsync"), Napi::Function::New(env, GrayRegionsAsync));
    exports.Set(Napi::String::New(env, "grayMaskSync"), Napi::Function::New(env, GrayRegionsSync));
    exports.Set(Napi::String::New(env, "grayMaskAsync"), Napi::Function::New(env, GrayRegionsAsync));
    exports.Set(Napi::String::New(env, "rgbPixelsSync"), Napi::Function::New(env, GrayRegionsSync));
    exports.Set(Napi::String::New(env, "rgbaPixelsSync"), Napi::Function::New(env, GrayRegionsAsync));*/
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)