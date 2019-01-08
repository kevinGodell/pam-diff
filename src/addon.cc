#define _SECURE_SCL 0

#include "gray_sync.h"
#include "gray_async.h"
#include "rgb_sync.h"
#include "rgb_async.h"
#include "diff_object.h"

#include <napi.h>

//https://github.com/nodejs/node-addon-api/blob/master/doc/object_wrap.md

//https://nodejs.github.io/node-addon-api/class_napi_1_1_object_wrap.html

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "grayDiffAllSync"), Napi::Function::New(env, GrayDiffAllSync));
    exports.Set(Napi::String::New(env, "grayDiffAllAsync"), Napi::Function::New(env, GrayDiffAllAsync));
    exports.Set(Napi::String::New(env, "grayDiffMaskSync"), Napi::Function::New(env, GrayDiffMaskSync));
    exports.Set(Napi::String::New(env, "grayDiffMaskAsync"), Napi::Function::New(env, GrayDiffMaskAsync));
    exports.Set(Napi::String::New(env, "grayDiffRegionsSync"), Napi::Function::New(env, GrayDiffRegionsSync));
    exports.Set(Napi::String::New(env, "grayDiffRegionsAsync"), Napi::Function::New(env, GrayDiffRegionsAsync));
    exports.Set(Napi::String::New(env, "rgbDiffAllSync"), Napi::Function::New(env, RgbDiffAllSync));
    exports.Set(Napi::String::New(env, "rgbDiffAllAsync"), Napi::Function::New(env, RgbDiffAllAsync));
    exports.Set(Napi::String::New(env, "rgbDiffMaskSync"), Napi::Function::New(env, RgbDiffMaskSync));
    exports.Set(Napi::String::New(env, "rgbDiffMaskAsync"), Napi::Function::New(env, RgbDiffMaskAsync));
    exports.Set(Napi::String::New(env, "rgbDiffRegionsSync"), Napi::Function::New(env, RgbDiffRegionsSync));
    exports.Set(Napi::String::New(env, "rgbDiffRegionsAsync"), Napi::Function::New(env, RgbDiffRegionsAsync));
    Example::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)