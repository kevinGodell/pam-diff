#include "gray_object.h"
#include "gray_async.h"
#include "engine.h"
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->all_ = {difference, percent};
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
    GrayAllPercent(this->dimensions_, this->all_, buf0, buf1, percentResult);
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->all_ = {difference, percent};
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
    auto *grayAllPercentWorker = new GrayAllPercentWorker(this->dimensions_, this->all_, napiBuf0, napiBuf1, cb);
    grayAllPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



GrayRegionPercentSync::GrayRegionPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->region_ = RegionJsToCpp(region);
}

Napi::FunctionReference GrayRegionPercentSync::constructor;

void GrayRegionPercentSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionPercentSync", {
            InstanceMethod("compare", &GrayRegionPercentSync::Compare)
    });
    GrayRegionPercentSync::constructor = Napi::Persistent(func);
    GrayRegionPercentSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionPercentSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionPercentSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionPercentSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();

    PercentResult percentResult = PercentResult{this->region_.name, 0, false};

    GrayRegionPercent(this->dimensions_, this->region_, buf0, buf1, percentResult);

    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult.flagged) {
        ToJs(env, percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionPercentAsync::GrayRegionPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->region_ = RegionJsToCpp(region);
}

Napi::FunctionReference GrayRegionPercentAsync::constructor;

void GrayRegionPercentAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionPercentAsync", {
            InstanceMethod("compare", &GrayRegionPercentAsync::Compare)
    });
    GrayRegionPercentAsync::constructor = Napi::Persistent(func);
    GrayRegionPercentAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionPercentAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionPercentAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionPercentAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionPercentWorker = new GrayRegionPercentWorker(this->dimensions_, this->region_, napiBuf0, napiBuf1, cb);
    grayRegionPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentSync::GrayRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regions = config.Get("regions").As<Napi::Array>();
    const Napi::Buffer<bool> bitset = config.Get("bitset").As<Napi::Buffer<bool>>();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = config.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = config.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = config.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = config.Get("maxY").As<Napi::Number>().Uint32Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    std::vector<PercentResult> percentResultVec = std::vector<PercentResult>(this->regions_.regions.size());
    uint_fast32_t flaggedCount = GrayRegionsPercent(this->dimensions_, this->regions_, buf0, buf1, percentResultVec);
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regions = config.Get("regions").As<Napi::Array>();
    const Napi::Buffer<bool> bitset = config.Get("bitset").As<Napi::Buffer<bool>>();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = config.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = config.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = config.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = config.Get("maxY").As<Napi::Number>().Uint32Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    auto *grayRegionsPercentWorker = new GrayRegionsPercentWorker(this->dimensions_, this->regions_, napiBuf0, napiBuf1, cb);
    grayRegionsPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsSync::GrayAllBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->all_ = {difference, percent};
    this->draw_ = draw;
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
    BoundsResult boundsResult = BoundsResult{"all", this->dimensions_.width - 1, 0, this->dimensions_.height - 1, 0, 0, false};
    GrayAllBounds(this->dimensions_, this->all_, buf0, buf1, boundsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->dimensions_.byteLength);
            DrawGray(boundsResult, this->dimensions_.width, pixels.Data());
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->all_ = {difference, percent};
    this->draw_ = draw;
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
    auto *grayAllBoundsWorker = new GrayAllBoundsWorker(this->dimensions_, this->all_, this->draw_, napiBuf0, napiBuf1, cb);
    grayAllBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionBoundsSync::GrayRegionBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->region_ = RegionJsToCpp(region);
    this->draw_ = draw;
}

Napi::FunctionReference GrayRegionBoundsSync::constructor;

void GrayRegionBoundsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionBoundsSync", {
            InstanceMethod("compare", &GrayRegionBoundsSync::Compare)
    });
    GrayRegionBoundsSync::constructor = Napi::Persistent(func);
    GrayRegionBoundsSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionBoundsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionBoundsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionBoundsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    BoundsResult boundsResult = BoundsResult{this->region_.name, this->region_.bounds.maxX, this->region_.bounds.minX, this->region_.bounds.maxY, this->region_.bounds.minY, 0, false};
    GrayRegionBounds(this->dimensions_, this->region_, buf0, buf1, boundsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->dimensions_.byteLength);
            DrawGray(boundsResult, this->dimensions_.width, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionBoundsAsync::GrayRegionBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->region_ = RegionJsToCpp(region);
    this->draw_ = draw;
}

Napi::FunctionReference GrayRegionBoundsAsync::constructor;

void GrayRegionBoundsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionBoundsAsync", {
            InstanceMethod("compare", &GrayRegionBoundsAsync::Compare)
    });
    GrayRegionBoundsAsync::constructor = Napi::Persistent(func);
    GrayRegionBoundsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionBoundsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionBoundsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionBoundsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionBoundsWorker = new GrayRegionBoundsWorker(this->dimensions_, this->region_, this->draw_, napiBuf0, napiBuf1, cb);
    grayRegionBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsSync::GrayRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regions = config.Get("regions").As<Napi::Array>();
    const Napi::Buffer<bool> bitset = config.Get("bitset").As<Napi::Buffer<bool>>();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = config.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = config.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = config.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = config.Get("maxY").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
    this->draw_ = draw;
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
    std::vector<BoundsResult> boundsResultVec = std::vector<BoundsResult>(this->regions_.regions.size(), BoundsResult{std::string(), this->dimensions_.width - 1, 0, this->dimensions_.height - 1, 0, 0, false});
    uint_fast32_t flaggedCount = GrayRegionsBounds(this->dimensions_, this->regions_, buf0, buf1, boundsResultVec);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (flaggedCount > 0) {
        ToJs(env, boundsResultVec, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->dimensions_.byteLength);
            DrawGray(boundsResultVec, this->dimensions_.width, pixels.Data());
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regions = config.Get("regions").As<Napi::Array>();
    const Napi::Buffer<bool> bitset = config.Get("bitset").As<Napi::Buffer<bool>>();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = config.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = config.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = config.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = config.Get("maxY").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
    this->draw_ = draw;
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
    auto *grayRegionsBoundsWorker = new GrayRegionsBoundsWorker(this->dimensions_, this->regions_, this->draw_, napiBuf0, napiBuf1, cb);
    grayRegionsBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBlobsSync::GrayAllBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->all_ = {difference, percent};
    this->draw_ = draw;
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
    BlobsResult blobsResult = BlobsResult{"all", this->dimensions_.width - 1, 0, this->dimensions_.height - 1, 0, 0, false, std::vector<Blob>()};
    GrayAllBlobs(this->dimensions_, this->all_, buf0, buf1, blobsResult);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->dimensions_.byteLength);
            DrawGray(blobsResult, this->dimensions_.width, pixels.Data());
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->all_ = {difference, percent};
    this->draw_ = draw;
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
    auto *grayAllBlobsWorker = new GrayAllBlobsWorker(this->dimensions_, this->all_, this->draw_, napiBuf0, napiBuf1, cb);
    grayAllBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionBlobsSync::GrayRegionBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->region_ = RegionJsToCpp(region);
    this->draw_ = draw;
}

Napi::FunctionReference GrayRegionBlobsSync::constructor;

void GrayRegionBlobsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionBlobsSync", {
            InstanceMethod("compare", &GrayRegionBlobsSync::Compare)
    });
    GrayRegionBlobsSync::constructor = Napi::Persistent(func);
    GrayRegionBlobsSync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionBlobsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionBlobsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionBlobsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();

    BlobsResult blobsResult = BlobsResult{this->region_.name, this->region_.bounds.maxX, this->region_.bounds.minX, this->region_.bounds.maxY, this->region_.bounds.minY, 0, false, std::vector<Blob>()};

    GrayRegionBlobs(this->dimensions_, this->region_, buf0, buf1, blobsResult);

    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->dimensions_.byteLength);
            DrawGray(blobsResult, this->dimensions_.width, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionBlobsAsync::GrayRegionBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->region_ = RegionJsToCpp(region);
    this->draw_ = draw;
}

Napi::FunctionReference GrayRegionBlobsAsync::constructor;

void GrayRegionBlobsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionBlobsAsync", {
            InstanceMethod("compare", &GrayRegionBlobsAsync::Compare)
    });
    GrayRegionBlobsAsync::constructor = Napi::Persistent(func);
    GrayRegionBlobsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionBlobsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionBlobsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionBlobsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionBlobsWorker = new GrayRegionBlobsWorker(this->dimensions_, this->region_, this->draw_, napiBuf0, napiBuf1, cb);
    grayRegionBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBlobsSync::GrayRegionsBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regions = config.Get("regions").As<Napi::Array>();
    const Napi::Buffer<bool> bitset = config.Get("bitset").As<Napi::Buffer<bool>>();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = config.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = config.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = config.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = config.Get("maxY").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->dimensions_ = {width, height, depth, width * height, width * height * depth};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
    this->draw_ = draw;
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

    std::vector<BlobsResult> blobsResultVec = std::vector<BlobsResult>(this->regions_.regions.size(), BlobsResult{std::string(), this->regions_.bounds.maxX, this->regions_.bounds.minX, this->regions_.bounds.maxY, this->regions_.bounds.minY, 0, false, std::vector<Blob>()});

    uint_fast32_t flaggedCount = GrayRegionsBlobs(this->dimensions_, this->regions_, buf0, buf1, blobsResultVec);

    Napi::Array resultsJs = Napi::Array::New(env);
    if (flaggedCount > 0) {
        ToJs(env, blobsResultVec, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->dimensions_.byteLength);
            DrawGray(blobsResultVec, this->dimensions_.width, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////