#include "gray_object.h"
//#include "rgb_object.h"
#include "engine.h"
#include "napi.h"

#ifdef NAPI_DEBUG
#include <iostream>
#endif

Napi::Object CreateObject(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Object config = info[0].As<Napi::Object>();
#ifdef NAPI_DEBUG

    // show system size values for types being used
    std::cout << "size of bool : " << sizeof(bool) << std::endl;
    std::cout << "size of std::string : " << sizeof(std::string) << std::endl;
    std::cout << "size of int_fast32_t : " << sizeof(int_fast32_t) << std::endl;
    std::cout << "size of uint_fast32_t : " << sizeof(uint_fast32_t) << std::endl;
    std::cout << "size of std::vector<bool> : " << sizeof(std::vector<bool>) << std::endl;
    std::cout << "size of std::vector<uint_fast8_t> : " << sizeof(std::vector<uint_fast8_t>) << std::endl;
    std::cout << "size of Region struct : " << sizeof(Region) << std::endl;
    std::cout << "size of PercentResult struct : " << sizeof(PercentResult) << std::endl;
    std::cout << "size of BoundsResult struct : " << sizeof(BoundsResult) << std::endl;

    if (config.Has("depth")) std::cout << "depth : " << config.Get("depth").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("width")) std::cout << "width : " << config.Get("width").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("height")) std::cout << "height : " << config.Get("height").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("response")) std::cout << "response : " << config.Get("response").As<Napi::String>().Utf8Value() << std::endl;
    if (config.Has("draw")) std::cout << "draw : " << config.Get("draw").As<Napi::Boolean>().Value() << std::endl;
    if (config.Has("async")) std::cout << "async : " << config.Get("async").As<Napi::Boolean>().Value() << std::endl;
    if (config.Has("target")) std::cout << "target : " << config.Get("target").As<Napi::String>().Utf8Value() << std::endl;
    if (config.Has("difference")) std::cout << "difference : " << config.Get("difference").As<Napi::Number>().Int32Value() << std::endl;
    if (config.Has("percent")) std::cout << "percent : " << config.Get("percent").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("bitsetCount")) std::cout << "bitsetCount : " << config.Get("bitsetCount").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("bitset")) std::cout << "bitset length : " << config.Get("bitset").As<Napi::Buffer<bool>>().Length() << std::endl;
    if (config.Has("minDiff")) std::cout << "minDiff : " << config.Get("minDiff").As<Napi::Number>().Int32Value() << std::endl;
    if (config.Has("minX")) std::cout << "minX : " << config.Get("minX").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("maxX")) std::cout << "maxX : " << config.Get("maxX").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("minY")) std::cout << "minY : " << config.Get("minY").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("maxY")) std::cout << "maxY : " << config.Get("maxY").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.Has("regions")) {
        const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
        std::cout << "regions length : " << regionsJs.Length() << std::endl;
        for (uint_fast32_t r = 0; r < regionsJs.Length(); r++) {
            const std::string name = regionsJs.Get(r).As<Napi::Object>().Get("name").As<Napi::String>();
            const uint_fast32_t diff = regionsJs.Get(r).As<Napi::Object>().Get("difference").As<Napi::Number>().Int32Value();
            const uint_fast32_t percent = regionsJs.Get(r).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
            const uint_fast32_t count = regionsJs.Get(r).As<Napi::Object>().Get("bitsetCount").As<Napi::Number>().Uint32Value();
            const uint_fast32_t minX = regionsJs.Get(r).As<Napi::Object>().Get("minX").As<Napi::Number>().Uint32Value();
            const uint_fast32_t maxX = regionsJs.Get(r).As<Napi::Object>().Get("maxX").As<Napi::Number>().Uint32Value();
            const uint_fast32_t minY = regionsJs.Get(r).As<Napi::Object>().Get("minY").As<Napi::Number>().Uint32Value();
            const uint_fast32_t maxY = regionsJs.Get(r).As<Napi::Object>().Get("maxY").As<Napi::Number>().Uint32Value();
            std::cout << name << " - " << diff << " - " << minX << " - " << maxX << " - " << minY << " - " << maxY << " - " << percent << " - " << count << " - " << regionsJs.Get(r).As<Napi::Object>().Get("bitset").As<Napi::Buffer<bool>>().Length() << std::endl;
        }
    }
#endif
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();

    const uint_fast32_t regionsLength = config.HasOwnProperty("regions") && config.Get("regions").IsArray() && config.Get("regions").As<Napi::Array>().Length() > 0 ? config.Get("regions").As<Napi::Array>().Length() : 0;

    const std::string response = config.Get("response").As<Napi::String>().Utf8Value();

    const bool async = config.Get("async").As<Napi::Boolean>().Value();

    const uint_fast32_t engineType = EngineType(depth, response, async, regionsLength);

    switch (engineType) {
        case GRAY_ALL_PERCENT_SYNC :
            return GrayAllPercentSync::NewInstance(env, config);
        case GRAY_ALL_PERCENT_ASYNC :
            return GrayAllPercentAsync::NewInstance(env, config);
        case GRAY_REGION_PERCENT_SYNC :
            return GrayRegionPercentSync::NewInstance(env, config);
        case GRAY_REGION_PERCENT_ASYNC :
            return GrayRegionPercentAsync::NewInstance(env, config);
        case GRAY_REGIONS_PERCENT_SYNC :
            return GrayRegionsPercentSync::NewInstance(env, config);
        case GRAY_REGIONS_PERCENT_ASYNC :
            return GrayRegionsPercentAsync::NewInstance(env, config);
        case GRAY_ALL_BOUNDS_SYNC :
            return GrayAllBoundsSync::NewInstance(env, config);
        case GRAY_ALL_BOUNDS_ASYNC :
            return GrayAllBoundsAsync::NewInstance(env, config);
        case GRAY_REGION_BOUNDS_SYNC :
            return GrayRegionBoundsSync::NewInstance(env, config);
        case GRAY_REGION_BOUNDS_ASYNC :
            return GrayRegionBoundsAsync::NewInstance(env, config);
        case GRAY_REGIONS_BOUNDS_SYNC :
            return GrayRegionsBoundsSync::NewInstance(env, config);
        case GRAY_REGIONS_BOUNDS_ASYNC :
            return GrayRegionsBoundsAsync::NewInstance(env, config);
        case GRAY_ALL_BLOBS_SYNC :
            return GrayAllBlobsSync::NewInstance(env, config);
        case GRAY_ALL_BLOBS_ASYNC :
            return GrayAllBlobsAsync::NewInstance(env, config);
        case GRAY_REGION_BLOBS_SYNC :
            return GrayRegionBlobsSync::NewInstance(env, config);
        case GRAY_REGION_BLOBS_ASYNC :
            return GrayRegionBlobsAsync::NewInstance(env, config);
        case GRAY_REGIONS_BLOBS_SYNC :
            return GrayRegionsBlobsSync::NewInstance(env, config);
        case GRAY_REGIONS_BLOBS_ASYNC :
            return GrayRegionsBlobsAsync::NewInstance(env, config);
            /*case RGB_ALL_PERCENT_SYNC :
                return RgbAllPercentSync::NewInstance(env, config);
            case RGB_ALL_PERCENT_ASYNC :
                return RgbAllPercentAsync::NewInstance(env, config);
            case RGB_REGION_PERCENT_SYNC :
                return RgbRegionPercentSync::NewInstance(env, config);
            case RGB_REGION_PERCENT_ASYNC :
                return RgbRegionPercentAsync::NewInstance(env, config);
            case RGB_REGIONS_PERCENT_SYNC :
                return RgbRegionsPercentSync::NewInstance(env, config);
            case RGB_REGIONS_PERCENT_ASYNC :
                return RgbRegionsPercentAsync::NewInstance(env, config);
            case RGB_ALL_BOUNDS_SYNC :
                return RgbAllBoundsSync::NewInstance(env, config);
            case RGB_ALL_BOUNDS_ASYNC :
                return RgbAllBoundsAsync::NewInstance(env, config);
            case RGB_REGION_BOUNDS_SYNC :
                return RgbRegionBoundsSync::NewInstance(env, config);
            case RGB_REGION_BOUNDS_ASYNC :
                return RgbRegionBoundsAsync::NewInstance(env, config);
            case RGB_REGIONS_BOUNDS_SYNC :
                return RgbRegionsBoundsSync::NewInstance(env, config);
            case RGB_REGIONS_BOUNDS_ASYNC :
                return RgbRegionsBoundsAsync::NewInstance(env, config);*/
        default:
            throw Napi::Error::New(env, "Engine not found for type " + std::to_string(engineType));
    }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports = Napi::Function::New(env, CreateObject, "CreateObject");

    GrayAllPercentSync::Init(env);
    GrayAllPercentAsync::Init(env);

    GrayRegionPercentSync::Init(env);
    GrayRegionPercentAsync::Init(env);

    GrayRegionsPercentSync::Init(env);
    GrayRegionsPercentAsync::Init(env);

    GrayAllBoundsSync::Init(env);
    GrayAllBoundsAsync::Init(env);

    GrayRegionBoundsSync::Init(env);
    GrayRegionBoundsAsync::Init(env);

    GrayRegionsBoundsSync::Init(env);
    GrayRegionsBoundsAsync::Init(env);

    GrayAllBlobsSync::Init(env);
    GrayAllBlobsAsync::Init(env);

    GrayRegionBlobsSync::Init(env);
    GrayRegionBlobsAsync::Init(env);

    GrayRegionsBlobsSync::Init(env);
    GrayRegionsBlobsAsync::Init(env);

    /*RgbAllPercentSync::Init(env);
    RgbAllPercentAsync::Init(env);

    RgbRegionPercentSync::Init(env);
    RgbRegionPercentAsync::Init(env);

    RgbRegionsPercentSync::Init(env);
    RgbRegionsPercentAsync::Init(env);

    RgbAllBoundsSync::Init(env);
    RgbAllBoundsAsync::Init(env);

    RgbRegionBoundsSync::Init(env);
    RgbRegionBoundsAsync::Init(env);

    RgbRegionsBoundsSync::Init(env);
    RgbRegionsBoundsAsync::Init(env);*/

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)