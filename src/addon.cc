#include <napi.h>
#include "sync.h"
#include "async.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "pixelsDiffSync"), Napi::Function::New(env, PixelsDiffSync));
    exports.Set(Napi::String::New(env, "pixelsDiffAsync"), Napi::Function::New(env, PixelsDiffAsync));
    /*exports.Set(Napi::String::New(env, "grayRegionsSync"), Napi::Function::New(env, GrayRegionsSync));
    exports.Set(Napi::String::New(env, "grayRegionsAsync"), Napi::Function::New(env, GrayRegionsAsync));
    exports.Set(Napi::String::New(env, "grayMaskSync"), Napi::Function::New(env, GrayRegionsSync));
    exports.Set(Napi::String::New(env, "grayMaskAsync"), Napi::Function::New(env, GrayRegionsAsync));
    exports.Set(Napi::String::New(env, "rgbPixelsSync"), Napi::Function::New(env, GrayRegionsSync));
    exports.Set(Napi::String::New(env, "rgbaPixelsSync"), Napi::Function::New(env, GrayRegionsAsync));*/
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)