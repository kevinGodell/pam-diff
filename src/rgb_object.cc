#include "rgb_object.h"
#include "rgb_async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentSync::RgbAllPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
}

Napi::FunctionReference RgbAllPercentSync::constructor;

void RgbAllPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbAllPercentSync", {
            InstanceMethod("compare", &RgbAllPercentSync::Compare)
    });
    RgbAllPercentSync::constructor = Napi::Persistent(func);
    RgbAllPercentSync::constructor.SuppressDestruct();
}

Napi::Object RgbAllPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbAllPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    PercentResult percentResult = PercentResult{"all", 0, false};
    RgbAllPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, percentResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult.flagged) {
        ToJs(env, percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentAsync::RgbAllPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
}

Napi::FunctionReference RgbAllPercentAsync::constructor;

void RgbAllPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbAllPercentAsync", {
            InstanceMethod("compare", &RgbAllPercentAsync::Compare)
    });
    RgbAllPercentAsync::constructor = Napi::Persistent(func);
    RgbAllPercentAsync::constructor.SuppressDestruct();
}

Napi::Object RgbAllPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbAllPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbAllPercentWorker = new RgbAllPercentWorker(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, napiBuf0, napiBuf1, cb);
    rgbAllPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionPercentSync::RgbRegionPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
}

Napi::FunctionReference RgbRegionPercentSync::constructor;

void RgbRegionPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionPercentSync", {
            InstanceMethod("compare", &RgbRegionPercentSync::Compare)
    });
    RgbRegionPercentSync::constructor = Napi::Persistent(func);
    RgbRegionPercentSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    PercentResult percentResult = PercentResult{"mask", 0, false};
    RgbRegionPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, percentResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult.flagged) {
        ToJs(env, percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionPercentAsync::RgbRegionPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
}

Napi::FunctionReference RgbRegionPercentAsync::constructor;

void RgbRegionPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionPercentAsync", {
            InstanceMethod("compare", &RgbRegionPercentAsync::Compare)
    });
    RgbRegionPercentAsync::constructor = Napi::Persistent(func);
    RgbRegionPercentAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rbgRegionPercentWorker = new RgbRegionPercentWorker(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, napiBuf0, napiBuf1, cb);
    rbgRegionPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentSync::RgbRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
}

Napi::FunctionReference RgbRegionsPercentSync::constructor;

void RgbRegionsPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionsPercentSync", {
            InstanceMethod("compare", &RgbRegionsPercentSync::Compare)
    });
    RgbRegionsPercentSync::constructor = Napi::Persistent(func);
    RgbRegionsPercentSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionsPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<PercentResult> percentResultVec = std::vector<PercentResult>(this->regionVec_.size());
    uint_fast32_t flaggedCount = RgbRegionsPercent(this->pixDepth_, this->pixCount_, this->minDiff_, this->regionVec_, buf0, buf1, percentResultVec);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (flaggedCount > 0) {
        ToJs(env, percentResultVec, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentAsync::RgbRegionsPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
}

Napi::FunctionReference RgbRegionsPercentAsync::constructor;

void RgbRegionsPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionsPercentAsync", {
            InstanceMethod("compare", &RgbRegionsPercentAsync::Compare)
    });
    RgbRegionsPercentAsync::constructor = Napi::Persistent(func);
    RgbRegionsPercentAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionsPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbRegionsPercentWorker = new RgbRegionsPercentWorker(this->pixDepth_, this->pixCount_, this->minDiff_, this->regionVec_, napiBuf0, napiBuf1, cb);
    rgbRegionsPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsSync::RgbAllBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->buf1Size_ = this->pixCount_ * this->pixDepth_;
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference RgbAllBoundsSync::constructor;

void RgbAllBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbAllBoundsSync", {
            InstanceMethod("compare", &RgbAllBoundsSync::Compare)
    });
    RgbAllBoundsSync::constructor = Napi::Persistent(func);
    RgbAllBoundsSync::constructor.SuppressDestruct();
}

Napi::Object RgbAllBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbAllBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = BoundsResult{"all", this->width_ - 1, 0, this->height_ - 1, 0, 0, false};
    RgbAllBounds(this->pixDepth_, this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, boundsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->buf1Size_);
            DrawRgb(boundsResult, this->width_, this->pixDepth_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsAsync::RgbAllBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference RgbAllBoundsAsync::constructor;

void RgbAllBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbAllBoundsAsync", {
            InstanceMethod("compare", &RgbAllBoundsAsync::Compare)
    });
    RgbAllBoundsAsync::constructor = Napi::Persistent(func);
    RgbAllBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbAllBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbAllBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbAllBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbAllBoundsWorker = new RgbAllBoundsWorker(this->pixDepth_, this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->draw_, napiBuf0, napiBuf1, cb);
    rgbAllBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBoundsSync::RgbRegionBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->buf1Size_ = this->pixCount_ * this->pixDepth_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference RgbRegionBoundsSync::constructor;

void RgbRegionBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionBoundsSync", {
            InstanceMethod("compare", &RgbRegionBoundsSync::Compare)
    });
    RgbRegionBoundsSync::constructor = Napi::Persistent(func);
    RgbRegionBoundsSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = BoundsResult{"mask", this->width_ - 1, 0, this->height_ - 1, 0, 0, false};
    RgbRegionBounds(this->pixDepth_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, boundsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->buf1Size_);
            DrawRgb(boundsResult, this->width_, this->pixDepth_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBoundsAsync::RgbRegionBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Uint32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference RgbRegionBoundsAsync::constructor;

void RgbRegionBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionBoundsAsync", {
            InstanceMethod("compare", &RgbRegionBoundsAsync::Compare)
    });
    RgbRegionBoundsAsync::constructor = Napi::Persistent(func);
    RgbRegionBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbRegionBoundsWorker = new RgbRegionBoundsWorker(this->pixDepth_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->draw_, napiBuf0, napiBuf1, cb);
    rgbRegionBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsSync::RgbRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->buf1Size_ = this->pixCount_ * this->pixDepth_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference RgbRegionsBoundsSync::constructor;

void RgbRegionsBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionsBoundsSync", {
            InstanceMethod("compare", &RgbRegionsBoundsSync::Compare)
    });
    RgbRegionsBoundsSync::constructor = Napi::Persistent(func);
    RgbRegionsBoundsSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionsBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<BoundsResult> boundsResultVec = std::vector<BoundsResult>(this->regionVec_.size(), BoundsResult{std::string(), this->width_ - 1, 0, this->height_ - 1, 0, 0, false});
    uint_fast32_t flaggedCount = RgbRegionsBounds(this->pixDepth_, this->width_, this->height_, this->minDiff_, this->regionVec_, buf0, buf1, boundsResultVec);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (flaggedCount > 0) {
        ToJs(env, boundsResultVec, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->buf1Size_);
            DrawRgb(boundsResultVec, this->width_, this->pixDepth_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsAsync::RgbRegionsBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference RgbRegionsBoundsAsync::constructor;

void RgbRegionsBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionsBoundsAsync", {
            InstanceMethod("compare", &RgbRegionsBoundsAsync::Compare)
    });
    RgbRegionsBoundsAsync::constructor = Napi::Persistent(func);
    RgbRegionsBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionsBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbRegionsBoundsWorker = new RgbRegionsBoundsWorker(this->pixDepth_, this->width_, this->height_, this->minDiff_, this->regionVec_, this->draw_, napiBuf0, napiBuf1, cb);
    rgbRegionsBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////