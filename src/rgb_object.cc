#include "rgb_object.h"
#include "rgb_async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentSync::RgbAllPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, false};
    this->all_ = {"all", difference, percent};
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
    const PercentResult percentResult = RgbAllPercent(this->config_, this->all_, buf0, buf1);
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, false};
    this->all_ = {"all", difference, percent};
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
    auto *rgbAllPercentWorker = new RgbAllPercentWorker(this->config_, this->all_, napiBuf0, napiBuf1, cb);
    rgbAllPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionPercentSync::RgbRegionPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    this->config_ = {width, height, depth, width * height, width * height * depth, false};
    this->region_ = RegionJsToCpp(region);
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
    const PercentResult percentResult = RgbRegionPercent(this->config_, this->region_, buf0, buf1);
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    this->config_ = {width, height, depth, width * height, width * height * depth, false};
    this->region_ = RegionJsToCpp(region);
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
    auto *rgbRegionPercentWorker = new RgbRegionPercentWorker(this->config_, this->region_, napiBuf0, napiBuf1, cb);
    rgbRegionPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentSync::RgbRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentSync>(info) {
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

    this->config_ = {width, height, depth, width * height, width * height * depth, false};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    const std::vector<PercentResult> percentResultVec = RgbRegionsPercent(this->config_, this->regions_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!percentResultVec.empty()) {
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

    this->config_ = {width, height, depth, width * height, width * height * depth, false};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    auto *rgbRegionsPercentWorker = new RgbRegionsPercentWorker(this->config_, this->regions_, napiBuf0, napiBuf1, cb);
    rgbRegionsPercentWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsSync::RgbAllBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
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
    const BoundsResult boundsResult = RgbAllBounds(this->config_, this->all_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawRgb(boundsResult, this->config_, pixels.Data());
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
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
    auto *rgbAllBoundsWorker = new RgbAllBoundsWorker(this->config_, this->all_, napiBuf0, napiBuf1, cb);
    rgbAllBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBoundsSync::RgbRegionBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
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
    const BoundsResult boundsResult = RgbRegionBounds(this->config_, this->region_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawRgb(boundsResult, this->config_, pixels.Data());
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
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
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
    auto *rgbRegionBoundsWorker = new RgbRegionBoundsWorker(this->config_, this->region_, napiBuf0, napiBuf1, cb);
    rgbRegionBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsSync::RgbRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsSync>(info) {
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    const std::vector<BoundsResult> boundsResultVec = RgbRegionsBounds(this->config_, this->regions_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!boundsResultVec.empty()) {
        ToJs(env, boundsResultVec, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawRgb(boundsResultVec, this->config_, pixels.Data());
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    auto *rgbRegionsBoundsWorker = new RgbRegionsBoundsWorker(this->config_, this->regions_, napiBuf0, napiBuf1, cb);
    rgbRegionsBoundsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBlobsSync::RgbAllBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
}

Napi::FunctionReference RgbAllBlobsSync::constructor;

void
RgbAllBlobsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbAllBlobsSync", {
            InstanceMethod("compare", &RgbAllBlobsSync::Compare)
    });
    RgbAllBlobsSync::constructor = Napi::Persistent(func);
    RgbAllBlobsSync::constructor.SuppressDestruct();
}

Napi::Object
RgbAllBlobsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbAllBlobsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value
RgbAllBlobsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    const BlobsResult blobsResult = RgbAllBlobs(this->config_, this->all_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawRgb(blobsResult, this->config_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBlobsAsync::RgbAllBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = config.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = config.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
}

Napi::FunctionReference RgbAllBlobsAsync::constructor;

void
RgbAllBlobsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbAllBlobsAsync", {
            InstanceMethod("compare", &RgbAllBlobsAsync::Compare)
    });
    RgbAllBlobsAsync::constructor = Napi::Persistent(func);
    RgbAllBlobsAsync::constructor.SuppressDestruct();
}

Napi::Object
RgbAllBlobsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbAllBlobsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value
RgbAllBlobsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbAllBlobsWorker = new RgbAllBlobsWorker(this->config_, this->all_, napiBuf0, napiBuf1, cb);
    rgbAllBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBlobsSync::RgbRegionBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
}

Napi::FunctionReference RgbRegionBlobsSync::constructor;

void RgbRegionBlobsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionBlobsSync", {
            InstanceMethod("compare", &RgbRegionBlobsSync::Compare)
    });
    RgbRegionBlobsSync::constructor = Napi::Persistent(func);
    RgbRegionBlobsSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionBlobsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionBlobsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionBlobsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    const BlobsResult blobsResult = RgbRegionBlobs(this->config_, this->region_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawRgb(blobsResult, this->config_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBlobsAsync::RgbRegionBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object config = info[0].As<Napi::Object>();
    const uint_fast32_t width = config.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = config.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = config.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object region = config.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = config.HasOwnProperty("draw") && config.Get("draw").As<Napi::Boolean>().Value();

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
}

Napi::FunctionReference RgbRegionBlobsAsync::constructor;

void RgbRegionBlobsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionBlobsAsync", {
            InstanceMethod("compare", &RgbRegionBlobsAsync::Compare)
    });
    RgbRegionBlobsAsync::constructor = Napi::Persistent(func);
    RgbRegionBlobsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionBlobsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionBlobsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionBlobsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbRegionBlobsWorker = new RgbRegionBlobsWorker(this->config_, this->region_, napiBuf0, napiBuf1, cb);
    rgbRegionBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBlobsSync::RgbRegionsBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBlobsSync>(info) {
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
}

Napi::FunctionReference RgbRegionsBlobsSync::constructor;

void RgbRegionsBlobsSync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionsBlobsSync", {
            InstanceMethod("compare", &RgbRegionsBlobsSync::Compare)
    });
    RgbRegionsBlobsSync::constructor = Napi::Persistent(func);
    RgbRegionsBlobsSync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsBlobsSync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionsBlobsSync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsBlobsSync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();
    const std::vector<BlobsResult> blobsResultVec = RgbRegionsBlobs(this->config_, this->regions_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!blobsResultVec.empty()) {
        ToJs(env, blobsResultVec, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawRgb(blobsResultVec, this->config_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBlobsAsync::RgbRegionsBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBlobsAsync>(info) {
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
}

Napi::FunctionReference RgbRegionsBlobsAsync::constructor;

void RgbRegionsBlobsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "RgbRegionsBlobsAsync", {
            InstanceMethod("compare", &RgbRegionsBlobsAsync::Compare)
    });
    RgbRegionsBlobsAsync::constructor = Napi::Persistent(func);
    RgbRegionsBlobsAsync::constructor.SuppressDestruct();
}

Napi::Object RgbRegionsBlobsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = RgbRegionsBlobsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value RgbRegionsBlobsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *rgbRegionsBlobsWorker = new RgbRegionsBlobsWorker(this->config_, this->regions_, napiBuf0, napiBuf1, cb);
    rgbRegionsBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////