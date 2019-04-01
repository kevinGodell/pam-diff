#include "gray_object.h"
#include "gray_async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentSync::GrayAllPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint_fast32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();

    const Config config = {width, height, depth, width * height, width * height * depth, false};
    const All all = {"all", difference, percent};

    this->execute_ = std::bind(&GrayAllPercentExecute, config, all, std::placeholders::_1, std::placeholders::_2);
    this->callback_ = &GrayAllPercentCallback;
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
    this->callback_(env, cb, this->execute_(buf0, buf1));
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentAsync::GrayAllPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayAllPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint_fast32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();

    const Config config = {width, height, depth, width * height, width * height * depth, false};
    const All all = {"all", difference, percent};

    this->execute_ = std::bind(&GrayAllPercentExecute, config, all, std::placeholders::_1, std::placeholders::_2);
    this->callback_ = &GrayAllPercentCallback;
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

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionPercentSync::GrayRegionPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint_fast32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    const Config config = {width, height, depth, width * height, width * height * depth, false};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = std::bind(&GrayRegionPercentExecute, config, region, std::placeholders::_1, std::placeholders::_2);
    this->callback_ = &GrayRegionPercentCallback;
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
    this->callback_(env, cb, this->execute_(buf0, buf1));
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionPercentAsync::GrayRegionPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint_fast32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    const Config config = {width, height, depth, width * height, width * height * depth, false};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = std::bind(&GrayRegionPercentExecute, config, region, std::placeholders::_1, std::placeholders::_2);
    this->callback_ = &GrayRegionPercentCallback;
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

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentSync::GrayRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint_fast32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();
    const Napi::Buffer<bool> bitset = configObj.Get("bitset").As<Napi::Buffer<bool>>();
    const uint_fast32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = configObj.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = configObj.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = configObj.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = configObj.Get("maxY").As<Napi::Number>().Uint32Value();

    Config config = {width, height, depth, width * height, width * height * depth, false};
    Regions regions = {RegionsJsToCpp(regionsArr), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};

    this->execute_ = std::bind(&GrayRegionsPercentExecute, config, regions, std::placeholders::_1, std::placeholders::_2);
    this->callback_ = &GrayRegionsPercentCallback;
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
    this->callback_(env, cb, this->execute_(buf0, buf1));
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentAsync::GrayRegionsPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint_fast32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint_fast32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();
    const Napi::Buffer<bool> bitset = configObj.Get("bitset").As<Napi::Buffer<bool>>();
    const uint_fast32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = configObj.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = configObj.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = configObj.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = configObj.Get("maxY").As<Napi::Number>().Uint32Value();

    Config config = {width, height, depth, width * height, width * height * depth, false};
    Regions regions = {RegionsJsToCpp(regionsArr), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};

    this->execute_ = std::bind(&GrayRegionsPercentExecute, config, regions, std::placeholders::_1, std::placeholders::_2);
    this->callback_ = &GrayRegionsPercentCallback;
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

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
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
    const BoundsResult boundsResult = GrayAllBounds(this->config_, this->all_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawGray(boundsResult, this->config_, pixels.Data());
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
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
    auto *grayAllBoundsWorker = new GrayAllBoundsWorker(this->config_, this->all_, napiBuf0, napiBuf1, cb);
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
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
    const BoundsResult boundsResult = GrayRegionBounds(this->config_, this->region_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        ToJs(env, boundsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawGray(boundsResult, this->config_, pixels.Data());
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
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
    auto *grayRegionBoundsWorker = new GrayRegionBoundsWorker(this->config_, this->region_, napiBuf0, napiBuf1, cb);
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    const std::vector<BoundsResult> boundsResultVec = GrayRegionsBounds(this->config_, this->regions_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!boundsResultVec.empty()) {
        ToJs(env, boundsResultVec, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawGray(boundsResultVec, this->config_, pixels.Data());
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    auto *grayRegionsBoundsWorker = new GrayRegionsBoundsWorker(this->config_, this->regions_, napiBuf0, napiBuf1, cb);
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
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
    const BlobsResult blobsResult = GrayAllBlobs(this->config_, this->all_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawGray(blobsResult, this->config_, pixels.Data());
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->all_ = {"all", difference, percent};
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
    auto *grayAllBlobsWorker = new GrayAllBlobsWorker(this->config_, this->all_, napiBuf0, napiBuf1, cb);
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
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
    const BlobsResult blobsResult = GrayRegionBlobs(this->config_, this->region_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (blobsResult.flagged) {
        ToJs(env, blobsResult, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawGray(blobsResult, this->config_, pixels.Data());
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->region_ = RegionJsToCpp(region);
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
    auto *grayRegionBlobsWorker = new GrayRegionBlobsWorker(this->config_, this->region_, napiBuf0, napiBuf1, cb);
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

    this->config_ = {width, height, depth, width * height, width * height * depth, draw};
    this->regions_ = {RegionsJsToCpp(regions), BitsetJsToCpp(bitset), difference, {minX, maxX, minY, maxY}};
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
    const std::vector<BlobsResult> blobsResultVec = GrayRegionsBlobs(this->config_, this->regions_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!blobsResultVec.empty()) {
        ToJs(env, blobsResultVec, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, buf1, this->config_.byteLength);
            DrawGray(blobsResultVec, this->config_, pixels.Data());
            cb.Call({env.Null(), resultsJs, pixels});
            return env.Undefined();
        }
    }
    cb.Call({env.Null(), resultsJs, env.Null()});
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBlobsAsync::GrayRegionsBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<GrayRegionsBlobsAsync>(info) {
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

Napi::FunctionReference GrayRegionsBlobsAsync::constructor;

void GrayRegionsBlobsAsync::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "GrayRegionsBlobsAsync", {
            InstanceMethod("compare", &GrayRegionsBlobsAsync::Compare)
    });
    GrayRegionsBlobsAsync::constructor = Napi::Persistent(func);
    GrayRegionsBlobsAsync::constructor.SuppressDestruct();
}

Napi::Object GrayRegionsBlobsAsync::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = GrayRegionsBlobsAsync::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value GrayRegionsBlobsAsync::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Buffer<uint_fast8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Function cb = info[2].As<Napi::Function>();
    auto *grayRegionsBlobsWorker = new GrayRegionsBlobsWorker(this->config_, this->regions_, napiBuf0, napiBuf1, cb);
    grayRegionsBlobsWorker->Queue();
    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////