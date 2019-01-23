#include "object.h"
#include "engine.h"
#include "async.h"
#include "results.h"
#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentSync::GrayAllPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllPercentSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
}

Napi::FunctionReference GrayAllPercentSync::constructor;

void GrayAllPercentSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayAllPercentSync", {
            InstanceMethod("compare", &GrayAllPercentSync::Compare)
    });
    GrayAllPercentSync::constructor = Napi::Persistent(func);
    GrayAllPercentSync::constructor.SuppressDestruct();
}

Napi::Object GrayAllPercentSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayAllPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    uint_fast32_t percentResult = GrayAllPercent(this->pixCount_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, percentResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentAsync::GrayAllPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllPercentAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
}

Napi::FunctionReference GrayAllPercentAsync::constructor;

void GrayAllPercentAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayAllPercentAsync", {
            InstanceMethod("compare", &GrayAllPercentAsync::Compare)
    });
    GrayAllPercentAsync::constructor = Napi::Persistent(func);
    GrayAllPercentAsync::constructor.SuppressDestruct();
}

Napi::Object GrayAllPercentAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayAllPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayAllPercentWorker = new GrayAllPercentWorker(this->target_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    grayAllPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercentSync::GrayMaskPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskPercentSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);

    std::cout << "gray mask percent sync bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;
}

Napi::FunctionReference GrayMaskPercentSync::constructor;

void GrayMaskPercentSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayMaskPercentSync", {
            InstanceMethod("compare", &GrayMaskPercentSync::Compare)
    });
    GrayMaskPercentSync::constructor = Napi::Persistent(func);
    GrayMaskPercentSync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskPercentSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayMaskPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    uint_fast32_t percentResult = GrayMaskPercent(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, percentResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercentAsync::GrayMaskPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskPercentAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);

    std::cout << "gray mask percent async bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;

}

Napi::FunctionReference GrayMaskPercentAsync::constructor;

void GrayMaskPercentAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayMaskPercentAsync", {
            InstanceMethod("compare", &GrayMaskPercentAsync::Compare)
    });
    GrayMaskPercentAsync::constructor = Napi::Persistent(func);
    GrayMaskPercentAsync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskPercentAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayMaskPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayMaskPercentWorker = new GrayMaskPercentWorker(this->target_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    grayMaskPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentSync::GrayRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsPercentSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs);
}

Napi::FunctionReference GrayRegionsPercentSync::constructor;

void GrayRegionsPercentSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayRegionsPercentSync", {
            InstanceMethod("compare", &GrayRegionsPercentSync::Compare)
    });
    GrayRegionsPercentSync::constructor = Napi::Persistent(func);
    GrayRegionsPercentSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsPercentSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayRegionsPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<uint_fast32_t> resultsVec = GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionsLen_,
                                                               this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, resultsVec);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentAsync::GrayRegionsPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsPercentAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs);
}

Napi::FunctionReference GrayRegionsPercentAsync::constructor;

void GrayRegionsPercentAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayRegionsPercentAsync", {
            InstanceMethod("compare", &GrayRegionsPercentAsync::Compare)
    });
    GrayRegionsPercentAsync::constructor = Napi::Persistent(func);
    GrayRegionsPercentAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsPercentAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayRegionsPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionsPercentWorker = new GrayRegionsPercentWorker(this->pixCount_, this->minDiff_, this->regionsLen_,
                                                                  this->regionVec_, buf0, buf1, cb);
    grayRegionsPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsSync::GrayAllBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBoundsSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
}

Napi::FunctionReference GrayAllBoundsSync::constructor;

void GrayAllBoundsSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayAllBoundsSync", {
            InstanceMethod("compare", &GrayAllBoundsSync::Compare)
    });
    GrayAllBoundsSync::constructor = Napi::Persistent(func);
    GrayAllBoundsSync::constructor.SuppressDestruct();
}

Napi::Object GrayAllBoundsSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayAllBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = GrayAllBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsAsync::GrayAllBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBoundsAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
}

Napi::FunctionReference GrayAllBoundsAsync::constructor;

void GrayAllBoundsAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayAllBoundsAsync", {
            InstanceMethod("compare", &GrayAllBoundsAsync::Compare)
    });
    GrayAllBoundsAsync::constructor = Napi::Persistent(func);
    GrayAllBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayAllBoundsAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayAllBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayAllBoundsWorker = new GrayAllBoundsWorker(this->target_, this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    grayAllBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBoundsSync::GrayMaskBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskBoundsSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    //this->pixCount_ = this->width_ * this->height_;
    uint_fast32_t pixCount = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + pixCount);

    std::cout << "gray mask bounds sync bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;

}

Napi::FunctionReference GrayMaskBoundsSync::constructor;

void GrayMaskBoundsSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayMaskBoundsSync", {
            InstanceMethod("compare", &GrayMaskBoundsSync::Compare)
    });
    GrayMaskBoundsSync::constructor = Napi::Persistent(func);
    GrayMaskBoundsSync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskBoundsSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayMaskBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = GrayMaskBounds(this->width_, this->height_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBoundsAsync::GrayMaskBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskBoundsAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    //this->pixCount_ = this->width_ * this->height_;
    uint_fast32_t pixCount = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + pixCount);

    std::cout << "gray mask bounds async bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;

}

Napi::FunctionReference GrayMaskBoundsAsync::constructor;

void GrayMaskBoundsAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayMaskBoundsAsync", {
            InstanceMethod("compare", &GrayMaskBoundsAsync::Compare)
    });
    GrayMaskBoundsAsync::constructor = Napi::Persistent(func);
    GrayMaskBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskBoundsAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayMaskBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayMaskBoundsWorker = new GrayMaskBoundsWorker(this->target_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_,this->bitsetVec_, buf0, buf1, cb);
    grayMaskBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsSync::GrayRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBoundsSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    //this->pixCount_ = this->width_ * this->height_;
    uint_fast32_t pixCount = this->width_ * this->height_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(pixCount, this->regionsLen_, regionsJs);
}

Napi::FunctionReference GrayRegionsBoundsSync::constructor;

void GrayRegionsBoundsSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayRegionsBoundsSync", {
            InstanceMethod("compare", &GrayRegionsBoundsSync::Compare)
    });
    GrayRegionsBoundsSync::constructor = Napi::Persistent(func);
    GrayRegionsBoundsSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsBoundsSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayRegionsBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<BoundsResult> resultsVec = GrayRegionsBounds(this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, resultsVec);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsAsync::GrayRegionsBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBoundsAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    //this->pixCount_ = this->width_ * this->height_;
    uint_fast32_t pixCount = this->width_ * this->height_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(pixCount, this->regionsLen_, regionsJs);
}

Napi::FunctionReference GrayRegionsBoundsAsync::constructor;

void GrayRegionsBoundsAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "GrayRegionsBoundsAsync", {
            InstanceMethod("compare", &GrayRegionsBoundsAsync::Compare)
    });
    GrayRegionsBoundsAsync::constructor = Napi::Persistent(func);
    GrayRegionsBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsBoundsAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = GrayRegionsBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionsBoundsWorker = new GrayRegionsBoundsWorker(this->width_, this->height_, this->minDiff_,
                                                                this->regionsLen_, this->regionVec_, buf0, buf1, cb);
    grayRegionsBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentSync::RgbAllPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllPercentSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bufLen_ = this->pixCount_ * this->pixDepth_;
}

Napi::FunctionReference RgbAllPercentSync::constructor;

void RgbAllPercentSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbAllPercentSync", {
            InstanceMethod("compare", &RgbAllPercentSync::Compare)
    });
    RgbAllPercentSync::constructor = Napi::Persistent(func);
    RgbAllPercentSync::constructor.SuppressDestruct();
}

Napi::Object RgbAllPercentSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbAllPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    uint_fast32_t percentResult = RgbAllPercent(this->pixCount_, this->pixDepth_, this->bufLen_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, percentResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentAsync::RgbAllPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllPercentAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bufLen_ = this->pixCount_ * this->pixDepth_;
}

Napi::FunctionReference RgbAllPercentAsync::constructor;

void RgbAllPercentAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbAllPercentAsync", {
            InstanceMethod("compare", &RgbAllPercentAsync::Compare)
    });
    RgbAllPercentAsync::constructor = Napi::Persistent(func);
    RgbAllPercentAsync::constructor.SuppressDestruct();
}

Napi::Object RgbAllPercentAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbAllPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbAllPercentWorker = new RgbAllPercentWorker(this->target_, this->pixCount_, this->pixDepth_, this->bufLen_,
                                                        this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    rgbAllPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskPercentSync::RgbMaskPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskPercentSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bufLen_ = this->pixCount_ * this->pixDepth_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);

    std::cout << "rgb mask percent sync bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;

}

Napi::FunctionReference RgbMaskPercentSync::constructor;

void RgbMaskPercentSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbMaskPercentSync", {
            InstanceMethod("compare", &RgbMaskPercentSync::Compare)
    });
    RgbMaskPercentSync::constructor = Napi::Persistent(func);
    RgbMaskPercentSync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskPercentSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbMaskPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    uint_fast32_t percentResult = RgbMaskPercent(this->pixDepth_, this->bufLen_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, percentResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskPercentAsync::RgbMaskPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskPercentAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bufLen_ = this->pixCount_ * this->pixDepth_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);

    std::cout << "rgb mask percent async bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;

}

Napi::FunctionReference RgbMaskPercentAsync::constructor;

void RgbMaskPercentAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbMaskPercentAsync", {
            InstanceMethod("compare", &RgbMaskPercentAsync::Compare)
    });
    RgbMaskPercentAsync::constructor = Napi::Persistent(func);
    RgbMaskPercentAsync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskPercentAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbMaskPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();//pixDepth,bufLen, pixDiff, bitsetCount, bitsetVec, buf0, buf1
    //const std::string &target, uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb
    auto *rbgMaskPercentWorker = new RgbMaskPercentWorker(this->target_, this->pixDepth_, this->bufLen_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    rbgMaskPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentSync::RgbRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bufLen_ = this->pixCount_ * this->pixDepth_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs);
}

Napi::FunctionReference RgbRegionsPercentSync::constructor;

void RgbRegionsPercentSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbRegionsPercentSync", {
            InstanceMethod("compare", &RgbRegionsPercentSync::Compare)
    });
    RgbRegionsPercentSync::constructor = Napi::Persistent(func);
    RgbRegionsPercentSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsPercentSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbRegionsPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<uint_fast32_t> resultsVec = RgbRegionsPercent(this->pixDepth_, this->bufLen_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, resultsVec);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentAsync::RgbRegionsPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bufLen_ = this->pixCount_ * this->pixDepth_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs);
}

Napi::FunctionReference RgbRegionsPercentAsync::constructor;

void RgbRegionsPercentAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbRegionsPercentAsync", {
            InstanceMethod("compare", &RgbRegionsPercentAsync::Compare)
    });
    RgbRegionsPercentAsync::constructor = Napi::Persistent(func);
    RgbRegionsPercentAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsPercentAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbRegionsPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbRegionsPercentWorker = new RgbRegionsPercentWorker(this->pixDepth_, this->bufLen_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1, cb);
    rgbRegionsPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsSync::RgbAllBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBoundsSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
}

Napi::FunctionReference RgbAllBoundsSync::constructor;

void RgbAllBoundsSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbAllBoundsSync", {
            InstanceMethod("compare", &RgbAllBoundsSync::Compare)
    });
    RgbAllBoundsSync::constructor = Napi::Persistent(func);
    RgbAllBoundsSync::constructor.SuppressDestruct();
}

Napi::Object RgbAllBoundsSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbAllBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = RgbAllBounds(this->width_, this->height_, this->pixCount_, this->pixDepth_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsAsync::RgbAllBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBoundsAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "all";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
}

Napi::FunctionReference RgbAllBoundsAsync::constructor;

void RgbAllBoundsAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbAllBoundsAsync", {
            InstanceMethod("compare", &RgbAllBoundsAsync::Compare)
    });
    RgbAllBoundsAsync::constructor = Napi::Persistent(func);
    RgbAllBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbAllBoundsAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbAllBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbAllBoundsWorker = new RgbAllBoundsWorker(this->target_, this->width_, this->height_, this->pixCount_, this->pixDepth_, this->pixDiff_, this->diffsPerc_, buf0, buf1, cb);
    rgbAllBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskBoundsSync::RgbMaskBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskBoundsSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);

    std::cout << "rgb mask bounds sync bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;

}

Napi::FunctionReference RgbMaskBoundsSync::constructor;

void RgbMaskBoundsSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbMaskBoundsSync", {
            InstanceMethod("compare", &RgbMaskBoundsSync::Compare)
    });
    RgbMaskBoundsSync::constructor = Napi::Persistent(func);
    RgbMaskBoundsSync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskBoundsSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbMaskBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = RgbMaskBounds(this->width_, this->height_, this->pixDepth_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, boundsResult);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskBoundsAsync::RgbMaskBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskBoundsAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->target_ = "mask";//config.Get("target").As<Napi::String>().Utf8Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);

    std::cout << "rgb mask bounds async bitset: " << this->bitsetVec_.size() << " - " << this->bitsetVec_.capacity() << " - " << this->bitsetVec_.max_size() << std::endl;

}

Napi::FunctionReference RgbMaskBoundsAsync::constructor;

void RgbMaskBoundsAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbMaskBoundsAsync", {
            InstanceMethod("compare", &RgbMaskBoundsAsync::Compare)
    });
    RgbMaskBoundsAsync::constructor = Napi::Persistent(func);
    RgbMaskBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskBoundsAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbMaskBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbMaskBoundsWorker = new RgbMaskBoundsWorker(this->target_, this->width_, this->height_, this->pixDepth_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, cb);
    rgbMaskBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsSync::RgbRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsSync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs);
}

Napi::FunctionReference RgbRegionsBoundsSync::constructor;

void RgbRegionsBoundsSync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbRegionsBoundsSync", {
            InstanceMethod("compare", &RgbRegionsBoundsSync::Compare)
    });
    RgbRegionsBoundsSync::constructor = Napi::Persistent(func);
    RgbRegionsBoundsSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsBoundsSync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbRegionsBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<BoundsResult> boundsResultVec = RgbRegionsBounds(this->width_, this->height_, this->pixDepth_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, boundsResultVec);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsAsync::RgbRegionsBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsAsync>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionsLen_ = regionsJs.Length();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, this->regionsLen_, regionsJs);
}

Napi::FunctionReference RgbRegionsBoundsAsync::constructor;

void RgbRegionsBoundsAsync::Init(const Napi::Env &env) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "RgbRegionsBoundsAsync", {
            InstanceMethod("compare", &RgbRegionsBoundsAsync::Compare)
    });
    RgbRegionsBoundsAsync::constructor = Napi::Persistent(func);
    RgbRegionsBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsBoundsAsync::NewInstance(Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object object = RgbRegionsBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbRegionsBoundsWorker = new RgbRegionsBoundsWorker(this->width_, this->height_, this->pixDepth_, this->minDiff_, this->regionsLen_, this->regionVec_, buf0, buf1, cb);
    rgbRegionsBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////