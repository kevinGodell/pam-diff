#include "object.h"
#include <napi.h>

Napi::Object CreateObject(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t pixDepth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const std::string target = config.Get("target").As<Napi::String>().Utf8Value();
    const std::string response = config.Get("response").As<Napi::String>().Utf8Value();
    const bool async = config.Get("async").As<Napi::Boolean>().Value();
    const uint_fast32_t engineType = Engine::EngineType(pixDepth, target, response, async);
    switch (engineType) {
        case Engine::GRAY_ALL_PERCENT_SYNC :
            return GrayAllPercentSync::NewInstance(env, config);
        case Engine::GRAY_ALL_PERCENT_ASYNC :
            return GrayAllPercentAsync::NewInstance(env, config);
        case Engine::GRAY_MASK_PERCENT_SYNC :
            return GrayMaskPercentSync::NewInstance(env, config);
        case Engine::GRAY_MASK_PERCENT_ASYNC :
            return GrayMaskPercentAsync::NewInstance(env, config);
        case Engine::GRAY_REGIONS_PERCENT_SYNC :
            return GrayRegionsPercentSync::NewInstance(env, config);
        case Engine::GRAY_REGIONS_PERCENT_ASYNC :
            return GrayRegionsPercentAsync::NewInstance(env, config);
        case Engine::GRAY_ALL_BOUNDS_SYNC :
            return GrayAllBoundsSync::NewInstance(env, config);
        case Engine::GRAY_ALL_BOUNDS_ASYNC :
            return GrayAllBoundsAsync::NewInstance(env, config);
        case Engine::GRAY_MASK_BOUNDS_SYNC :
            return GrayMaskBoundsSync::NewInstance(env, config);
        case Engine::GRAY_MASK_BOUNDS_ASYNC :
            return GrayMaskBoundsAsync::NewInstance(env, config);
        case Engine::GRAY_REGIONS_BOUNDS_SYNC :
            return GrayRegionsBoundsSync::NewInstance(env, config);
        case Engine::GRAY_REGIONS_BOUNDS_ASYNC :
            return GrayRegionsBoundsAsync::NewInstance(env, config);
        case Engine::RGB_ALL_PERCENT_SYNC :
            return RgbAllPercentSync::NewInstance(env, config);
        case Engine::RGB_ALL_PERCENT_ASYNC :
            return RgbAllPercentAsync::NewInstance(env, config);
        case Engine::RGB_MASK_PERCENT_SYNC :
            return RgbMaskPercentSync::NewInstance(env, config);
        case Engine::RGB_MASK_PERCENT_ASYNC :
            return RgbMaskPercentAsync::NewInstance(env, config);
        case Engine::RGB_REGIONS_PERCENT_SYNC :
            return RgbRegionsPercentSync::NewInstance(env, config);
        case Engine::RGB_REGIONS_PERCENT_ASYNC :
            return RgbRegionsPercentAsync::NewInstance(env, config);
        case Engine::RGB_ALL_BOUNDS_SYNC :
            return RgbAllBoundsSync::NewInstance(env, config);
        case Engine::RGB_ALL_BOUNDS_ASYNC :
            return RgbAllBoundsAsync::NewInstance(env, config);
        case Engine::RGB_MASK_BOUNDS_SYNC :
            return RgbMaskBoundsSync::NewInstance(env, config);
        case Engine::RGB_MASK_BOUNDS_ASYNC :
            return RgbMaskBoundsAsync::NewInstance(env, config);
        case Engine::RGB_REGIONS_BOUNDS_SYNC :
            return RgbRegionsBoundsSync::NewInstance(env, config);
        case Engine::RGB_REGIONS_BOUNDS_ASYNC :
            return RgbRegionsBoundsAsync::NewInstance(env, config);
        default:
            throw Napi::Error::New(env, "Engine not found for type " + std::to_string(engineType));
    }

}

Napi::Object Init(Napi::Env env, Napi::Object /*exports*/) {
    Napi::Object new_exports = Napi::Function::New(env, CreateObject, "CreateObject");
    GrayAllPercentSync::Init(env, new_exports);
    GrayAllPercentAsync::Init(env, new_exports);
    GrayMaskPercentSync::Init(env, new_exports);
    GrayMaskPercentAsync::Init(env, new_exports);
    GrayRegionsPercentSync::Init(env, new_exports);
    GrayRegionsPercentAsync::Init(env, new_exports);
    GrayAllBoundsSync::Init(env, new_exports);
    GrayAllBoundsAsync::Init(env, new_exports);
    GrayMaskBoundsSync::Init(env, new_exports);
    GrayMaskBoundsAsync::Init(env, new_exports);
    GrayRegionsBoundsSync::Init(env, new_exports);
    GrayRegionsBoundsAsync::Init(env, new_exports);
    RgbAllPercentSync::Init(env, new_exports);
    RgbAllPercentAsync::Init(env, new_exports);
    RgbMaskPercentSync::Init(env, new_exports);
    RgbMaskPercentAsync::Init(env, new_exports);
    RgbRegionsPercentSync::Init(env, new_exports);
    RgbRegionsPercentAsync::Init(env, new_exports);
    RgbAllBoundsSync::Init(env, new_exports);
    RgbAllBoundsAsync::Init(env, new_exports);
    RgbMaskBoundsSync::Init(env, new_exports);
    RgbMaskBoundsAsync::Init(env, new_exports);
    RgbRegionsBoundsSync::Init(env, new_exports);
    RgbRegionsBoundsAsync::Init(env, new_exports);
    return new_exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)