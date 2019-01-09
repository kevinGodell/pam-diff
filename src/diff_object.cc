#include "diff_object.h"
#include "diff.h"
#include "helper.h"
#include "gray_async.h"
#include "rgb_async.h"
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

Example::Example(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Example>(info) {
    Napi::Env env = info.Env();//todo not sure if line needed
    Napi::HandleScope scope(env);//todo not sure if line needed

    const Napi::Object obj = info[0].As<Napi::Object>();

    //all config objects must have these values
    this->width_ = obj.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = obj.Get("height").As<Napi::Number>().Uint32Value();
    this->depth_ = obj.Get("depth").As<Napi::Number>().Uint32Value();
    this->target_ = obj.Get("target").As<Napi::String>().Utf8Value();
    this->filter_ = obj.Get("filter").As<Napi::String>().Utf8Value();
    this->async_ =  obj.Get("async").As<Napi::Boolean>().Value();

    // calculate some values based on required information
    this->pixCount_ = this->width_ * this->height_;
    this->bufLen_ = this->pixCount_ * this->depth_;
    this->engineType_ = engineType(this->depth_, this->target_, this->filter_, this->async_);

    // the following settings are optional depending on chosen engine, grab whatever is available
    if (obj.Has("difference")) this->pixDiff_ = obj.Get("difference").As<Napi::Number>().Uint32Value();
    if (obj.Has("percent")) this->diffsPerc_ = obj.Get("percent").As<Napi::Number>().Uint32Value();
    if (obj.Has("minDiff")) this->minDiff_ = obj.Get("minDiff").As<Napi::Number>().Uint32Value();
    if (obj.Has("bitsetCount")) this->bitsetCount_ = obj.Get("bitsetCount").As<Napi::Number>().Uint32Value();

    if (obj.Has("bitset")) {
        //this->bitset_ = obj.Get("bitset").As<Napi::Buffer<uint_fast8_t>>().Data();
        const bool *bitset = obj.Get("bitset").As<Napi::Buffer<bool>>().Data();
        this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    }

    if (obj.Has("regions")) {
        const Napi::Array regionsJs = obj.Get("regions").As<Napi::Array>();
        this->regionsLen_ = regionsJs.Length();
        regionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs, this->regionsVec_);
    }

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
    }
}

Napi::FunctionReference Example::constructor;

void Example::Compare(const Napi::CallbackInfo &info) {
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();

    (this->*(this->comparePtr_))(buf0, buf1, cb);
}

void Example::SetMyValue(const Napi::CallbackInfo &info, const Napi::Value& value) {
    this->myValue_ = value.As<Napi::String>();
}

Napi::Value Example::GetMyValue(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->myValue_);
}

Napi::Value Example::GrayAllPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    uint_fast8_t percentResult = MeasureDiffs(this->pixCount_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    allResultsToJs(env, this->diffsPerc_, percentResult, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::GrayMaskPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    uint_fast8_t percentResult = MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    maskResultsToJs(env, this->diffsPerc_, percentResult, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::GrayRegionsPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    std::vector<uint_fast32_t> resultsVec(this->regionsLen_, 0);
    MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, buf0, buf1, resultsVec);
    Napi::Array resultsJs = Napi::Array::New(env);
    regionsResultsToJs(env, this->regionsLen_, this->regionsVec_, resultsVec, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::GrayAllPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    GrayDiffAllWorker *grayDiffAllWorker = new GrayDiffAllWorker(this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    grayDiffAllWorker->Queue();
    return env.Undefined();
}

Napi::Value Example::GrayMaskPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    GrayDiffMaskWorker *grayDiffMaskWorker = new GrayDiffMaskWorker(this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    grayDiffMaskWorker->Queue();
    return env.Undefined();
}

Napi::Value Example::GrayRegionsPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    GrayDiffRegionsWorker *grayDiffRegionsWorker = new GrayDiffRegionsWorker(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, buf0, buf1, cb);
    grayDiffRegionsWorker->Queue();
    return env.Undefined();
}

/////////////////////////////////////////////////////////////////////

Napi::Value Example::RgbAllPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    uint_fast8_t percentResult = MeasureDiffs(this->pixCount_, this->depth_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    allResultsToJs(env, this->diffsPerc_, percentResult, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::RgbMaskPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    uint_fast8_t percentResult = MeasureDiffs(this->pixCount_, this->depth_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    maskResultsToJs(env, this->diffsPerc_, percentResult, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::RgbRegionsPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    std::vector<uint_fast32_t> resultsVec(this->regionsLen_, 0);
    MeasureDiffs(this->pixCount_, this->depth_, this->minDiff_, this->regionsLen_, this->regionsVec_, buf0, buf1, resultsVec);
    Napi::Array resultsJs = Napi::Array::New(env);
    regionsResultsToJs(env, this->regionsLen_, this->regionsVec_, resultsVec, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::RgbAllPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    RgbDiffAllWorker *rgbDiffAllWorker = new RgbDiffAllWorker(this->pixCount_, this->depth_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    rgbDiffAllWorker->Queue();
    return env.Undefined();
}

Napi::Value Example::RgbMaskPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    RgbDiffMaskWorker *rgbDiffMaskWorker = new RgbDiffMaskWorker(this->pixCount_, this->depth_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    rgbDiffMaskWorker->Queue();
    return env.Undefined();
}

Napi::Value Example::RgbRegionsPercentAsync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    RgbDiffRegionsWorker *rgbDiffRegionsWorker = new RgbDiffRegionsWorker(this->pixCount_, this->depth_, this->minDiff_, this->regionsLen_, this->regionsVec_, buf0, buf1, cb);
    rgbDiffRegionsWorker->Queue();
    return env.Undefined();
}

// Napi::Object obj = constructor.New({ Napi::Number::New(info.Env(), this->value_ * multiple.DoubleValue()) });
// https://nodejs.github.io/node-addon-api/class_napi_1_1_object_wrap.html