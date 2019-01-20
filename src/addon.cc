#include "object.h"
#include "engine.h"
#include <napi.h>

Napi::Object CreateObject(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t pixDepth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const std::string target = config.Get("target").As<Napi::String>().Utf8Value();
    const std::string response = config.Get("response").As<Napi::String>().Utf8Value();
    const bool async = config.Get("async").As<Napi::Boolean>().Value();
    const uint_fast32_t engineType = EngineType(pixDepth, target, response, async);
    switch (engineType) {
        case GRAY_ALL_PERCENT_SYNC :
            return GrayAllPercentSync::NewInstance(env, config);
        case GRAY_ALL_PERCENT_ASYNC :
            return GrayAllPercentAsync::NewInstance(env, config);
        case GRAY_MASK_PERCENT_SYNC :
            return GrayMaskPercentSync::NewInstance(env, config);
        case GRAY_MASK_PERCENT_ASYNC :
            return GrayMaskPercentAsync::NewInstance(env, config);
        case GRAY_REGIONS_PERCENT_SYNC :
            return GrayRegionsPercentSync::NewInstance(env, config);
        case GRAY_REGIONS_PERCENT_ASYNC :
            return GrayRegionsPercentAsync::NewInstance(env, config);
        case GRAY_ALL_BOUNDS_SYNC :
            return GrayAllBoundsSync::NewInstance(env, config);
        case GRAY_ALL_BOUNDS_ASYNC :
            return GrayAllBoundsAsync::NewInstance(env, config);
        case GRAY_MASK_BOUNDS_SYNC :
            return GrayMaskBoundsSync::NewInstance(env, config);
        case GRAY_MASK_BOUNDS_ASYNC :
            return GrayMaskBoundsAsync::NewInstance(env, config);
        case GRAY_REGIONS_BOUNDS_SYNC :
            return GrayRegionsBoundsSync::NewInstance(env, config);
        case GRAY_REGIONS_BOUNDS_ASYNC :
            return GrayRegionsBoundsAsync::NewInstance(env, config);
        case RGB_ALL_PERCENT_SYNC :
            return RgbAllPercentSync::NewInstance(env, config);
        case RGB_ALL_PERCENT_ASYNC :
            return RgbAllPercentAsync::NewInstance(env, config);
        case RGB_MASK_PERCENT_SYNC :
            return RgbMaskPercentSync::NewInstance(env, config);
        case RGB_MASK_PERCENT_ASYNC :
            return RgbMaskPercentAsync::NewInstance(env, config);
        case RGB_REGIONS_PERCENT_SYNC :
            return RgbRegionsPercentSync::NewInstance(env, config);
        case RGB_REGIONS_PERCENT_ASYNC :
            return RgbRegionsPercentAsync::NewInstance(env, config);
        case RGB_ALL_BOUNDS_SYNC :
            return RgbAllBoundsSync::NewInstance(env, config);
        case RGB_ALL_BOUNDS_ASYNC :
            return RgbAllBoundsAsync::NewInstance(env, config);
        case RGB_MASK_BOUNDS_SYNC :
            return RgbMaskBoundsSync::NewInstance(env, config);
        case RGB_MASK_BOUNDS_ASYNC :
            return RgbMaskBoundsAsync::NewInstance(env, config);
        case RGB_REGIONS_BOUNDS_SYNC :
            return RgbRegionsBoundsSync::NewInstance(env, config);
        case RGB_REGIONS_BOUNDS_ASYNC :
            return RgbRegionsBoundsAsync::NewInstance(env, config);
        default:
            throw Napi::Error::New(env, "Engine not found for type " + std::to_string(engineType));
    }

}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports = Napi::Function::New(env, CreateObject, "CreateObject");
    GrayAllPercentSync::Init(env);
    GrayAllPercentAsync::Init(env);
    GrayMaskPercentSync::Init(env);
    GrayMaskPercentAsync::Init(env);
    GrayRegionsPercentSync::Init(env);
    GrayRegionsPercentAsync::Init(env);
    GrayAllBoundsSync::Init(env);
    GrayAllBoundsAsync::Init(env);
    GrayMaskBoundsSync::Init(env);
    GrayMaskBoundsAsync::Init(env);
    GrayRegionsBoundsSync::Init(env);
    GrayRegionsBoundsAsync::Init(env);
    RgbAllPercentSync::Init(env);
    RgbAllPercentAsync::Init(env);
    RgbMaskPercentSync::Init(env);
    RgbMaskPercentAsync::Init(env);
    RgbRegionsPercentSync::Init(env);
    RgbRegionsPercentAsync::Init(env);
    RgbAllBoundsSync::Init(env);
    RgbAllBoundsAsync::Init(env);
    RgbMaskBoundsSync::Init(env);
    RgbMaskBoundsAsync::Init(env);
    RgbRegionsBoundsSync::Init(env);
    RgbRegionsBoundsAsync::Init(env);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)