#include "object.h"
#include "engine.h"
#include "async.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentSync::GrayAllPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
}

Napi::FunctionReference GrayAllPercentSync::constructor;

void GrayAllPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayAllPercentSync", {
            InstanceMethod("compare", &GrayAllPercentSync::Compare)
    });
    GrayAllPercentSync::constructor = Napi::Persistent(func);
    GrayAllPercentSync::constructor.SuppressDestruct();
}

Napi::Object GrayAllPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayAllPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    PercentResult percentResult = PercentResult{"all", 0, false};
    GrayAllPercent(this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, percentResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult.flagged) {
        ToJs(env, percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentAsync::GrayAllPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
}

Napi::FunctionReference GrayAllPercentAsync::constructor;

void GrayAllPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayAllPercentAsync", {
            InstanceMethod("compare", &GrayAllPercentAsync::Compare)
    });
    GrayAllPercentAsync::constructor = Napi::Persistent(func);
    GrayAllPercentAsync::constructor.SuppressDestruct();
}

Napi::Object GrayAllPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayAllPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayAllPercentWorker = new GrayAllPercentWorker(this->pixCount_, this->pixDiff_, this->diffsPerc_, napiBuf0, napiBuf1, cb);
    grayAllPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercentSync::GrayMaskPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->minX_ = config.Get("minX").As<Napi::Number>().Uint32Value();
    this->maxX_ = config.Get("maxX").As<Napi::Number>().Uint32Value();
    this->minY_ = config.Get("minY").As<Napi::Number>().Uint32Value();
    this->maxY_ = config.Get("maxY").As<Napi::Number>().Uint32Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const Napi::Buffer<bool> bitset = config.Get("bitset").As<Napi::Buffer<bool>>();
    const bool *bitsetData = bitset.Data();
    const size_t bitsetLength = bitset.Length();
    this->bitsetVec_.assign(bitsetData, bitsetData + bitsetLength);

}

Napi::FunctionReference GrayMaskPercentSync::constructor;

void GrayMaskPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayMaskPercentSync", {
            InstanceMethod("compare", &GrayMaskPercentSync::Compare)
    });
    GrayMaskPercentSync::constructor = Napi::Persistent(func);
    GrayMaskPercentSync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayMaskPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    PercentResult percentResult = PercentResult{"mask", 0, false};
    GrayMaskPercent(this->width_, this->minX_, this->maxX_, this->minY_, this->maxY_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, percentResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult.flagged) {
        ToJs(env, percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercentAsync::GrayMaskPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->minX_ = config.Get("minX").As<Napi::Number>().Uint32Value();
    this->maxX_ = config.Get("maxX").As<Napi::Number>().Uint32Value();
    this->minY_ = config.Get("minY").As<Napi::Number>().Uint32Value();
    this->maxY_ = config.Get("maxY").As<Napi::Number>().Uint32Value();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const Napi::Buffer<bool> bitset = config.Get("bitset").As<Napi::Buffer<bool>>();
    const bool *bitsetData = bitset.Data();
    const size_t bitsetLength = bitset.Length();
    this->bitsetVec_.assign(bitsetData, bitsetData + bitsetLength);
}

Napi::FunctionReference GrayMaskPercentAsync::constructor;

void GrayMaskPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayMaskPercentAsync", {
            InstanceMethod("compare", &GrayMaskPercentAsync::Compare)
    });
    GrayMaskPercentAsync::constructor = Napi::Persistent(func);
    GrayMaskPercentAsync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayMaskPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayMaskPercentWorker = new GrayMaskPercentWorker(this->width_, this->minX_, this->maxX_, this->minY_, this->maxY_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, napiBuf0, napiBuf1, cb);
    grayMaskPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentSync::GrayRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
}

Napi::FunctionReference GrayRegionsPercentSync::constructor;

void GrayRegionsPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionsPercentSync", {
            InstanceMethod("compare", &GrayRegionsPercentSync::Compare)
    });
    GrayRegionsPercentSync::constructor = Napi::Persistent(func);
    GrayRegionsPercentSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionsPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<PercentResult> percentResultVec = std::vector<PercentResult>(this->regionVec_.size());
    uint_fast32_t flaggedCount = GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionVec_, buf0, buf1, percentResultVec);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (flaggedCount > 0) {
        ToJs(env, percentResultVec, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentAsync::GrayRegionsPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
}

Napi::FunctionReference GrayRegionsPercentAsync::constructor;

void GrayRegionsPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionsPercentAsync", {
            InstanceMethod("compare", &GrayRegionsPercentAsync::Compare)
    });
    GrayRegionsPercentAsync::constructor = Napi::Persistent(func);
    GrayRegionsPercentAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionsPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionsPercentWorker = new GrayRegionsPercentWorker(this->pixCount_, this->minDiff_, this->regionVec_, napiBuf0, napiBuf1, cb);
    grayRegionsPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsSync::GrayAllBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayAllBoundsSync::constructor;

void GrayAllBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayAllBoundsSync", {
            InstanceMethod("compare", &GrayAllBoundsSync::Compare)
    });
    GrayAllBoundsSync::constructor = Napi::Persistent(func);
    GrayAllBoundsSync::constructor.SuppressDestruct();
}

Napi::Object GrayAllBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayAllBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = BoundsResult{"all", this->width_ - 1, 0, this->height_ - 1, 0, 0, false};
    GrayAllBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, boundsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->pixCount_);
            DrawGray(boundsResult, this->width_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsAsync::GrayAllBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayAllBoundsAsync::constructor;

void GrayAllBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayAllBoundsAsync", {
            InstanceMethod("compare", &GrayAllBoundsAsync::Compare)
    });
    GrayAllBoundsAsync::constructor = Napi::Persistent(func);
    GrayAllBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayAllBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayAllBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayAllBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayAllBoundsWorker = new GrayAllBoundsWorker(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->draw_, napiBuf0, napiBuf1, cb);
    grayAllBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBoundsSync::GrayMaskBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayMaskBoundsSync::constructor;

void GrayMaskBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayMaskBoundsSync", {
            InstanceMethod("compare", &GrayMaskBoundsSync::Compare)
    });
    GrayMaskBoundsSync::constructor = Napi::Persistent(func);
    GrayMaskBoundsSync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayMaskBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = BoundsResult{"mask", this->width_ - 1, 0, this->height_ - 1, 0, 0, false};
    GrayMaskBounds(this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, boundsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->pixCount_);
            DrawGray(boundsResult, this->width_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBoundsAsync::GrayMaskBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayMaskBoundsAsync::constructor;

void GrayMaskBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayMaskBoundsAsync", {
            InstanceMethod("compare", &GrayMaskBoundsAsync::Compare)
    });
    GrayMaskBoundsAsync::constructor = Napi::Persistent(func);
    GrayMaskBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayMaskBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayMaskBoundsWorker = new GrayMaskBoundsWorker(this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->draw_, napiBuf0, napiBuf1, cb);
    grayMaskBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsSync::GrayRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayRegionsBoundsSync::constructor;

void GrayRegionsBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionsBoundsSync", {
            InstanceMethod("compare", &GrayRegionsBoundsSync::Compare)
    });
    GrayRegionsBoundsSync::constructor = Napi::Persistent(func);
    GrayRegionsBoundsSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionsBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    std::vector<BoundsResult> boundsResultVec = std::vector<BoundsResult>(this->regionVec_.size(), BoundsResult{std::string(), this->width_ - 1, 0, this->height_ - 1, 0, 0, false});
    uint_fast32_t flaggedCount = GrayRegionsBounds(this->width_, this->height_, this->minDiff_, this->regionVec_, buf0, buf1, boundsResultVec);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (flaggedCount > 0) {
        ToJs(env, boundsResultVec, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->pixCount_);
            DrawGray(boundsResultVec, this->width_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsAsync::GrayRegionsBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayRegionsBoundsAsync::constructor;

void GrayRegionsBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionsBoundsAsync", {
            InstanceMethod("compare", &GrayRegionsBoundsAsync::Compare)
    });
    GrayRegionsBoundsAsync::constructor = Napi::Persistent(func);
    GrayRegionsBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionsBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionsBoundsWorker = new GrayRegionsBoundsWorker(this->width_, this->height_, this->minDiff_, this->regionVec_, this->draw_, napiBuf0, napiBuf1, cb);
    grayRegionsBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentSync::RgbAllPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
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
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
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

RgbMaskPercentSync::RgbMaskPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
}

Napi::FunctionReference RgbMaskPercentSync::constructor;

void RgbMaskPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbMaskPercentSync", {
            InstanceMethod("compare", &RgbMaskPercentSync::Compare)
    });
    RgbMaskPercentSync::constructor = Napi::Persistent(func);
    RgbMaskPercentSync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbMaskPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    PercentResult percentResult = PercentResult{"mask", 0, false};
    RgbMaskPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, percentResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult.flagged) {
        ToJs(env, percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskPercentAsync::RgbMaskPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->pixDepth_ = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = width * height;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
}

Napi::FunctionReference RgbMaskPercentAsync::constructor;

void RgbMaskPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbMaskPercentAsync", {
            InstanceMethod("compare", &RgbMaskPercentAsync::Compare)
    });
    RgbMaskPercentAsync::constructor = Napi::Persistent(func);
    RgbMaskPercentAsync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbMaskPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rbgMaskPercentWorker = new RgbMaskPercentWorker(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, napiBuf0, napiBuf1, cb);
    rbgMaskPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentSync::RgbRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
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
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
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
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
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
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
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

RgbMaskBoundsSync::RgbMaskBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
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

Napi::FunctionReference RgbMaskBoundsSync::constructor;

void RgbMaskBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbMaskBoundsSync", {
            InstanceMethod("compare", &RgbMaskBoundsSync::Compare)
    });
    RgbMaskBoundsSync::constructor = Napi::Persistent(func);
    RgbMaskBoundsSync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbMaskBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = BoundsResult{"mask", this->width_ - 1, 0, this->height_ - 1, 0, 0, false};
    RgbMaskBounds(this->pixDepth_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, boundsResult);
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

RgbMaskBoundsAsync::RgbMaskBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbMaskBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
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

Napi::FunctionReference RgbMaskBoundsAsync::constructor;

void RgbMaskBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbMaskBoundsAsync", {
            InstanceMethod("compare", &RgbMaskBoundsAsync::Compare)
    });
    RgbMaskBoundsAsync::constructor = Napi::Persistent(func);
    RgbMaskBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbMaskBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbMaskBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbMaskBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbMaskBoundsWorker = new RgbMaskBoundsWorker(this->pixDepth_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->draw_, napiBuf0, napiBuf1, cb);
    rgbMaskBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsSync::RgbRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
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
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
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

GrayAllBlobsSync::GrayAllBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayAllBlobsSync::constructor;

void
GrayAllBlobsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayAllBlobsSync", {
            InstanceMethod("compare", &GrayAllBlobsSync::Compare)
    });
    GrayAllBlobsSync::constructor = Napi::Persistent(func);
    GrayAllBlobsSync::constructor.SuppressDestruct();
}

Napi::Object
GrayAllBlobsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayAllBlobsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value
GrayAllBlobsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BlobsResult blobsResult = BlobsResult{"all", this->width_ - 1, 0, this->height_ - 1, 0, 0, false, std::vector<Blob>()};
    GrayAllBlobs(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, buf0, buf1, blobsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->pixCount_);
            DrawGray(blobsResult, this->width_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBlobsAsync::GrayAllBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayAllBlobsAsync::constructor;

void
GrayAllBlobsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayAllBlobsAsync", {
            InstanceMethod("compare", &GrayAllBlobsAsync::Compare)
    });
    GrayAllBlobsAsync::constructor = Napi::Persistent(func);
    GrayAllBlobsAsync::constructor.SuppressDestruct();
}

Napi::Object
GrayAllBlobsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayAllBlobsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value
GrayAllBlobsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayAllBlobsWorker = new GrayAllBlobsWorker(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->draw_, napiBuf0, napiBuf1, cb);
    grayAllBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBlobsSync::GrayMaskBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayMaskBlobsSync::constructor;

void GrayMaskBlobsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayMaskBlobsSync", {
            InstanceMethod("compare", &GrayMaskBlobsSync::Compare)
    });
    GrayMaskBlobsSync::constructor = Napi::Persistent(func);
    GrayMaskBlobsSync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskBlobsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayMaskBlobsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskBlobsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BlobsResult blobsResult = BlobsResult{"mask", this->width_ - 1, 0, this->height_ - 1, 0, 0, false, std::vector<Blob>()};
    GrayMaskBlobs(this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, buf0, buf1, blobsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->pixCount_);
            DrawGray(blobsResult, this->width_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBlobsAsync::GrayMaskBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayMaskBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();
    this->pixDiff_ = config.Get("difference").As<Napi::Number>().Int32Value();
    this->diffsPerc_ = config.Get("percent").As<Napi::Number>().Uint32Value();
    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;
    this->bitsetCount_ = config.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const bool *bitset = config.Get("bitset").As<Napi::Buffer<bool>>().Data();
    this->bitsetVec_.assign(bitset, bitset + this->pixCount_);
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayMaskBlobsAsync::constructor;

void GrayMaskBlobsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayMaskBlobsAsync", {
            InstanceMethod("compare", &GrayMaskBlobsAsync::Compare)
    });
    GrayMaskBlobsAsync::constructor = Napi::Persistent(func);
    GrayMaskBlobsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayMaskBlobsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayMaskBlobsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayMaskBlobsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayMaskBlobsWorker = new GrayMaskBlobsWorker(this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->draw_, napiBuf0, napiBuf1, cb);
    grayMaskBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBlobsSync::GrayRegionsBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object config = info[0].As<Napi::Object>();

    this->width_ = config.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = config.Get("height").As<Napi::Number>().Uint32Value();
    this->pixCount_ = this->width_ * this->height_;

    // minDiff, minX, maxX, minY, maxY, mask used in first pass to minimize looping calculations
    this->minDiff_ = config.Get("minDiff").As<Napi::Number>().Int32Value();
    this->minX_ = config.Get("minX").As<Napi::Number>().Uint32Value();
    this->maxX_ = config.Get("maxX").As<Napi::Number>().Uint32Value();
    this->minY_ = config.Get("minY").As<Napi::Number>().Uint32Value();
    this->maxY_ = config.Get("maxY").As<Napi::Number>().Uint32Value();
    const bool *mask = config.Get("mask").As<Napi::Buffer<bool>>().Data();
    this->maskVec_.reserve(this->pixCount_);
    this->maskVec_.assign(mask, mask + this->pixCount_);

    // get region specific values
    const Napi::Array regionsJs = config.Get("regions").As<Napi::Array>();
    this->regionVec_ = RegionsJsToCpp(this->pixCount_, regionsJs);

    // should results be drawn to pixels
    if (config.HasOwnProperty("draw")) this->draw_ = config.Get("draw").As<Napi::Boolean>().Value();
    else this->draw_ = false;
}

Napi::FunctionReference GrayRegionsBlobsSync::constructor;

void GrayRegionsBlobsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionsBlobsSync", {
            InstanceMethod("compare", &GrayRegionsBlobsSync::Compare)
    });
    GrayRegionsBlobsSync::constructor = Napi::Persistent(func);
    GrayRegionsBlobsSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsBlobsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionsBlobsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsBlobsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();

    std::vector<BlobsResult> blobsResultVec = std::vector<BlobsResult>(this->regionVec_.size(), BlobsResult{std::string(), this->width_ - 1, 0, this->height_ - 1, 0, 0, false, std::vector<Blob>()});

    uint_fast32_t flaggedCount = GrayRegionsBlobs(this->width_, this->height_, this->minDiff_, this->minX_, this->maxX_, this->minY_, this->maxY_, this->regionVec_, buf0, buf1, blobsResultVec);

    Napi::Array resultsJs = Napi::Array::New(env);
    if (flaggedCount > 0) {
        ToJs(env, blobsResultVec, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->pixCount_);
            DrawGray(blobsResultVec, this->width_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////