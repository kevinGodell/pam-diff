#include "object.h"
#include "engine.h"
#include "async.h"
#include "results.h"
#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

void Example::Init(const Napi::Env &env, Napi::Object &exports) {
    Napi::Function func = DefineClass(env, "Example", {
            InstanceMethod("compare", &Example::Compare),
            InstanceAccessor("myValue", &Example::GetMyValue, &Example::SetMyValue)
    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Example", func);
}

Example::Example(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Example>(info), env_(info.Env()) {
    Napi::HandleScope scope(this->env_);

    const Napi::Object obj = info[0].As<Napi::Object>();

    //all config objects must have these values
    this->width_ = obj.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = obj.Get("height").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = obj.Get("depth").As<Napi::Number>().Uint32Value();
    this->target_ = obj.Get("target").As<Napi::String>().Utf8Value();
    this->response_ = obj.Get("response").As<Napi::String>().Utf8Value();
    this->async_ =  obj.Get("async").As<Napi::Boolean>().Value();

    // calculate some values based on required information
    this->pixCount_ = this->width_ * this->height_;
    this->bufLen_ = this->pixCount_ * this->pixDepth_;

    // the following settings are optional depending on chosen engine, grab whatever is available
    if (obj.Has("difference")) this->pixDiff_ = obj.Get("difference").As<Napi::Number>().Uint32Value();
    if (obj.Has("percent")) this->diffsPerc_ = obj.Get("percent").As<Napi::Number>().Uint32Value();
    if (obj.Has("minDiff")) this->minDiff_ = obj.Get("minDiff").As<Napi::Number>().Uint32Value();
    if (obj.Has("bitsetCount")) this->bitsetCount_ = obj.Get("bitsetCount").As<Napi::Number>().Uint32Value();

    if (obj.Has("bitset")) {
        const bool *bitset = obj.Get("bitset").As<Napi::Buffer<bool>>().Data();
        this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    }

    if (obj.Has("regions")) {
        const Napi::Array regionsJs = obj.Get("regions").As<Napi::Array>();
        this->regionsLen_ = regionsJs.Length();
        this->regionVec_ = Example::RegionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs);
    }

    this->engineType_ = Example::EngineType(this->pixDepth_, this->target_, this->response_, this->async_);

    switch (this->engineType_) {
        case GRAY_ALL_PERCENT_SYNC :
            this->comparePtr_ = &Example::GrayAllPercentSync;
            break;
        case GRAY_MASK_PERCENT_SYNC :
            this->comparePtr_ = &Example::GrayMaskPercentSync;
            break;
        case GRAY_REGIONS_PERCENT_SYNC :
            this->comparePtr_ = &Example::GrayRegionsPercentSync;
            break;
        case GRAY_ALL_PERCENT_ASYNC :
            this->comparePtr_ = &Example::GrayAllPercentAsync;
            break;
        case GRAY_MASK_PERCENT_ASYNC :
            this->comparePtr_ = &Example::GrayMaskPercentAsync;
            break;
        case GRAY_REGIONS_PERCENT_ASYNC :
            this->comparePtr_ = &Example::GrayRegionsPercentAsync;
            break;
        case RGB_ALL_PERCENT_SYNC :
            this->comparePtr_ = &Example::RgbAllPercentSync;
            break;
        case RGB_MASK_PERCENT_SYNC :
            this->comparePtr_ = &Example::RgbMaskPercentSync;
            break;
        case RGB_REGIONS_PERCENT_SYNC :
            this->comparePtr_ = &Example::RgbRegionsPercentSync;
            break;
        case RGB_ALL_PERCENT_ASYNC :
            this->comparePtr_ = &Example::RgbAllPercentAsync;
            break;
        case RGB_MASK_PERCENT_ASYNC :
            this->comparePtr_ = &Example::RgbMaskPercentAsync;
            break;
        case RGB_REGIONS_PERCENT_ASYNC :
            this->comparePtr_ = &Example::RgbRegionsPercentAsync;
            break;
        case GRAY_ALL_BOUNDS_SYNC :
            this->comparePtr_ = &Example::GrayAllBoundsSync;
            break;
        case GRAY_MASK_BOUNDS_SYNC :
            this->comparePtr_ = &Example::GrayMaskBoundsSync;
            break;
        case GRAY_REGIONS_BOUNDS_SYNC :
            this->comparePtr_ = &Example::GrayRegionsBoundsSync;
            break;
        case GRAY_ALL_BOUNDS_ASYNC :
            this->comparePtr_ = &Example::GrayAllBoundsAsync;
            break;
        case GRAY_MASK_BOUNDS_ASYNC :
            this->comparePtr_ = &Example::GrayMaskBoundsAsync;
            break;
        case GRAY_REGIONS_BOUNDS_ASYNC :
            this->comparePtr_ = &Example::GrayRegionsBoundsAsync;
            break;
        case RGB_ALL_BOUNDS_SYNC :
            this->comparePtr_ = &Example::RgbAllBoundsSync;
            break;
        case RGB_MASK_BOUNDS_SYNC :
            this->comparePtr_ = &Example::RgbMaskBoundsSync;
            break;
        case RGB_REGIONS_BOUNDS_SYNC :
            this->comparePtr_ = &Example::RgbRegionsBoundsSync;
            break;
        case RGB_ALL_BOUNDS_ASYNC :
            this->comparePtr_ = &Example::RgbAllBoundsAsync;
            break;
        case RGB_MASK_BOUNDS_ASYNC :
            this->comparePtr_ = &Example::RgbMaskBoundsAsync;
            break;
        case RGB_REGIONS_BOUNDS_ASYNC :
            this->comparePtr_ = &Example::RgbRegionsBoundsAsync;
            break;
        default:
            throw Napi::Error::New(this->env_, "Engine not found for type " + std::to_string(this->engineType_));
    }
}

Napi::FunctionReference Example::constructor;

void Example::Compare(const Napi::CallbackInfo &info) {
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    (this->*(this->comparePtr_))(buf0, buf1, cb);
}

void Example::SetMyValue(const Napi::CallbackInfo &/*info*/, const Napi::Value& value) {
    this->myValue_ = value.As<Napi::String>();
}

Napi::Value Example::GetMyValue(const Napi::CallbackInfo &/*info*/) {
    //Napi::Env env = info.Env();
    return Napi::String::New(this->env_, this->myValue_);
}

/////////////////////////////////////////////////////////////////////

// private static
uint_fast32_t Example::EngineType(const uint_fast32_t pixDepth, const std::string &target, const std::string &response, const bool async) {
    uint_fast32_t value = 0;
    if (pixDepth == 3 || pixDepth == 4) {//dont add for pixDepth == 1
        value += 1;
    }
    if (target == "mask") {//dont add for target == "all"
        value += 10;
    } else if (target == "regions") {
        value += 20;
    }
    if (response == "bounds") {//dont add for target == "percent"
        value += 100;
    } else if (response == "blobs") {
        value += 200;
    }
    if (async) {
        value += 1000;
    }
    return value;
}

// private static
std::vector<Engine::Region> Example::RegionsJsToCpp(const uint_fast32_t pixLen, const uint_fast32_t regionsLen, const Napi::Array &regionsJs) {
    std::vector<Engine::Region> regionVec;
    regionVec.reserve(regionsLen);
    for (uint_fast32_t i = 0; i < regionsLen; i++) {
        const std::string name = regionsJs.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast32_t diff = regionsJs.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsJs.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsJs.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const bool *bitset = regionsJs.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<bool>>().Data();
        std::vector<bool> bitsetVec;
        bitsetVec.assign(bitset, bitset + pixLen);
        regionVec.push_back(Engine::Region {name, diff, percent, count, bitsetVec});
    }
    return regionVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::GrayAllPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    uint_fast32_t percentResult = Engine::GrayAllPercent(this->pixCount_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, percentResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::GrayMaskPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    uint_fast32_t percentResult = Engine::GrayMaskPercent(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, percentResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::GrayRegionsPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    std::vector<uint_fast32_t> resultsVec = Engine::GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->regionsLen_, this->regionVec_, resultsVec);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::GrayAllPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *grayAllPercent = new GrayAllPercent(this->target_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    grayAllPercent->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::GrayMaskPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *grayMaskPercent = new GrayMaskPercent(this->target_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    grayMaskPercent->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::GrayRegionsPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *grayRegionsPercent = new GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1, cb);
    grayRegionsPercent->Queue();
    return this->env_.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::RgbAllPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    uint_fast32_t percentResult = Engine::RgbAllPercent(this->pixCount_, this-> pixDepth_, this->bufLen_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, percentResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::RgbMaskPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    uint_fast32_t percentResult = Engine::RgbMaskPercent(this-> pixDepth_, this->bufLen_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, percentResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::RgbRegionsPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    std::vector<uint_fast32_t> resultsVec = Engine::RgbRegionsPercent(this-> pixDepth_, this->bufLen_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->regionsLen_, this->regionVec_, resultsVec);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::RgbAllPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *rgbAllPercent = new RgbAllPercent(this->target_, this->pixCount_, this-> pixDepth_, this->bufLen_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    rgbAllPercent->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::RgbMaskPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *rgbMaskPercent = new RgbMaskPercent(this->target_, this-> pixDepth_, this->bufLen_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    rgbMaskPercent->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::RgbRegionsPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *rgbRegionsPercent = new RgbRegionsPercent(this-> pixDepth_, this->bufLen_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1, cb);
    rgbRegionsPercent->Queue();
    return this->env_.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::GrayAllBoundsSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    Engine::BoundsResult boundsResult = Engine::GrayAllBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::GrayMaskBoundsSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    Engine::BoundsResult boundsResult = Engine::GrayMaskBounds(this->width_, this->height_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::GrayRegionsBoundsSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    std::vector<Engine::BoundsResult> resultsVec = Engine::GrayRegionsBounds(this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->regionsLen_, this->regionVec_, resultsVec);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::GrayAllBoundsAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *grayAllBounds = new GrayAllBounds(this->target_, this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    grayAllBounds->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::GrayMaskBoundsAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *grayMaskBounds = new GrayMaskBounds(this->target_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    grayMaskBounds->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::GrayRegionsBoundsAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *grayRegionsBounds = new GrayRegionsBounds(this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1, cb);
    grayRegionsBounds->Queue();
    return this->env_.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::RgbAllBoundsSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    Engine::BoundsResult boundsResult = Engine::RgbAllBounds(this->width_, this->height_, this->pixCount_, this-> pixDepth_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::RgbMaskBoundsSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    Engine::BoundsResult boundsResult = Engine::RgbMaskBounds(this->width_, this->height_, this-> pixDepth_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

Napi::Value Example::RgbRegionsBoundsSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    std::vector<Engine::BoundsResult> boundsResultVec = Engine::RgbRegionsBounds(this->width_, this->height_, this-> pixDepth_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = Results::ToJs(this->env_, this->regionsLen_, this->regionVec_, boundsResultVec);
    cb.Call({this->env_.Null(), resultsJs});
    return this->env_.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Napi::Value Example::RgbAllBoundsAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *rgbAllBounds = new RgbAllBounds(this->target_, this->width_, this->height_, this->pixCount_, this-> pixDepth_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    rgbAllBounds->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::RgbMaskBoundsAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *rgbMaskBounds = new RgbMaskBounds(this->target_, this->width_, this->height_, this-> pixDepth_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    rgbMaskBounds->Queue();
    return this->env_.Undefined();
}

Napi::Value Example::RgbRegionsBoundsAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    auto *rgbRegionsBounds = new RgbRegionsBounds(this->width_, this->height_, this-> pixDepth_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1, cb);
    rgbRegionsBounds->Queue();
    return this->env_.Undefined();
}