#include "gray_object.h"
//#include "rgb_object.h"
#include "engine.h"
#include "napi.h"

#include <cstdint>


//#ifdef NAPI_DEBUG
#include <iostream>
using std::cout;
using std::endl;
//#endif

Napi::Object CreateObject(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Object config = info[0].As<Napi::Object>();
//#ifdef NAPI_DEBUG

    // show system size values for types being used
    cout << "size of bool : " << sizeof(bool) << endl;
    cout << "size of std::string : " << sizeof(std::string) << endl;
    cout << "size of int_fast32_t : " << sizeof(int_fast32_t) << endl;
    cout << "size of uint_fast32_t : " << sizeof(uint_fast32_t) << endl;
    cout << "size of std::vector<bool> : " << sizeof(std::vector<bool>) << endl;
    cout << "size of std::vector<uint_fast8_t> : " << sizeof(std::vector<uint_fast8_t>) << endl;
    cout << "size of Region struct : " << sizeof(Region) << endl;
    cout << "size of PercentResult struct : " << sizeof(PercentResult) << endl;
    cout << "size of BoundsResult struct : " << sizeof(BoundsResult) << endl;

    if (config.HasOwnProperty("depth")) cout << "depth : " << config.Get("depth").As<Napi::Number>().Uint32Value() << endl;
    if (config.HasOwnProperty("width")) cout << "width : " << config.Get("width").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("height")) std::cout << "height : " << config.Get("height").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("response")) std::cout << "response : " << config.Get("response").As<Napi::String>().Utf8Value() << std::endl;
    if (config.HasOwnProperty("draw")) std::cout << "draw : " << config.Get("draw").As<Napi::Boolean>().Value() << std::endl;
    if (config.HasOwnProperty("async")) std::cout << "async : " << config.Get("async").As<Napi::Boolean>().Value() << std::endl;
    if (config.HasOwnProperty("target")) std::cout << "target : " << config.Get("target").As<Napi::String>().Utf8Value() << std::endl;
    if (config.HasOwnProperty("difference")) std::cout << "difference : " << config.Get("difference").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("percent")) std::cout << "percent : " << config.Get("percent").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("bitsetCount")) std::cout << "bitsetCount : " << config.Get("bitsetCount").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("bitset")) std::cout << "bitset length : " << config.Get("bitset").As<Napi::Buffer<bool>>().Length() << std::endl;
    if (config.HasOwnProperty("minX")) std::cout << "minX : " << config.Get("minX").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("maxX")) std::cout << "maxX : " << config.Get("maxX").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("minY")) std::cout << "minY : " << config.Get("minY").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("maxY")) std::cout << "maxY : " << config.Get("maxY").As<Napi::Number>().Uint32Value() << std::endl;
    if (config.HasOwnProperty("regions")) {
        const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
        //uint_fast32_t regionsLength = regionsJs.Length();
        cout << "regions length : " << regionsJs.Length() << endl;
        for (uint_fast32_t r = 0; r < regionsJs.Length(); ++r) {
            Napi::Object obj = regionsJs.Get(r).As<Napi::Object>();
            const std::string name = obj.HasOwnProperty("name") ? obj.Get("name").As<Napi::String>() : std::string();
            const uint_fast32_t difference = obj.HasOwnProperty("difference") ? obj.Get("difference").As<Napi::Number>().Uint32Value() : 0;
            const uint_fast32_t percent = obj.HasOwnProperty("percent") ? obj.Get("percent").As<Napi::Number>().Uint32Value() : 0;
            const uint_fast32_t bitsetCount = obj.HasOwnProperty("bitsetCount") ? obj.Get("bitsetCount").As<Napi::Number>().Uint32Value() : 0;
            const uint_fast32_t bitsetLength = obj.HasOwnProperty("bitset") ? static_cast<uint_fast32_t>(obj.Get("bitset").As<Napi::Buffer<bool>>().Length()) : 0;
            const uint_fast32_t minX = obj.HasOwnProperty("minX") ? obj.Get("minX").As<Napi::Number>().Uint32Value() : 0;
            const uint_fast32_t maxX = obj.HasOwnProperty("maxX") ? obj.Get("maxX").As<Napi::Number>().Uint32Value() : 0;
            const uint_fast32_t minY = obj.HasOwnProperty("minY") ? obj.Get("minY").As<Napi::Number>().Uint32Value() : 0;
            const uint_fast32_t maxY = obj.HasOwnProperty("maxY") ? obj.Get("maxY").As<Napi::Number>().Uint32Value() : 0;
            cout << name << " - " << difference << " - " << minX << " - " << maxX << " - " << minY << " - " << maxY << " - " << percent << " - " << bitsetCount << " - " << bitsetLength << endl;
        }
    }
//#endif
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    cout << "depth " << depth << endl;
    const uint_fast32_t regionsLength = config.HasOwnProperty("regions") && config.Get("regions").IsArray() && config.Get("regions").As<Napi::Array>().Length() > 0 ? config.Get("regions").As<Napi::Array>().Length() : 0;
    cout << "regionsLength " << regionsLength << endl;
    const std::string response = config.Get("response").As<Napi::String>().Utf8Value();
    cout << "response " << response << endl;
    const bool async = config.Get("async").As<Napi::Boolean>().Value();
    cout << "async " << async << endl;
    const uint_fast32_t engineType = EngineType(depth, response, async, regionsLength);
    cout << "engineType " << engineType << endl;
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