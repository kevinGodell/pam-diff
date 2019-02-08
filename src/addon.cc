#include "object.h"
#include "engine.h"
#include "napi.h"

#ifdef NAPI_DEBUG
#include <iostream>
#endif

Napi::Object CreateObject(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Object config = info[0].As<Napi::Object>();
#ifdef NAPI_DEBUG
    if (config.Has("depth")) std::cout << "depth: " << config.Get("depth").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("width")) std::cout << "width: " << config.Get("width").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("height")) std::cout << "height: " << config.Get("height").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("response")) std::cout << "response: " << config.Get("response").As<Napi::String>().Utf8Value() << std::endl;
    if (config.Has("draw")) std::cout << "draw: " << config.Get("draw").As<Napi::Boolean>().Value() << std::endl;
    if (config.Has("async")) std::cout << "async: " << config.Get("async").As<Napi::Boolean>().Value() << std::endl;
    if (config.Has("target")) std::cout << "target: " << config.Get("target").As<Napi::String>().Utf8Value() << std::endl;
    if (config.Has("difference")) std::cout << "difference: " << config.Get("difference").As<Napi::Number>().Int32Value() << std::endl;
    if (config.Has("percent")) std::cout << "percent: " << config.Get("percent").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("bitsetCount")) std::cout << "bitsetCount: " << config.Get("bitsetCount").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("bitset")) std::cout << "bitset length: " << config.Get("bitset").As<Napi::Buffer<bool>>().Length() << std::endl;
    if (config.Has("minDiff")) std::cout << "minDiff: " << config.Get("minDiff").As<Napi::Number>().Int32Value() << std::endl;
    if (config.Has("regions")) {
        const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
        std::cout << "regions length: " << regionsJs.Length() << std::endl;
        for (uint_fast32_t r = 0; r < regionsJs.Length(); r++) {
            const std::string name = regionsJs.Get(r).As<Napi::Object>().Get("name").As<Napi::String>();
            const uint_fast32_t diff = regionsJs.Get(r).As<Napi::Object>().Get("diff").As<Napi::Number>().Int32Value();
            const uint_fast32_t percent = regionsJs.Get(r).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
            const uint_fast32_t count = regionsJs.Get(r).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
            std::cout << name << " - " << diff << " - " << percent << " - " << count << " - " << regionsJs.Get(r).As<Napi::Object>().Get("bitset").As<Napi::Buffer<bool>>().Length() << std::endl;
        }
    }
#endif
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