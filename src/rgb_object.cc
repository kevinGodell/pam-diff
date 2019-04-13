#include "rgb_object.h"
#include "async.h"
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

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();

    const Config config = Config{width, height, depth, false};
    const All all = All{"all", difference, percent};

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbAllPercentExecute(config, all, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbAllPercentExecute, config, all, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &AllPercentCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentAsync::RgbAllPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();

    const Config config = Config{width, height, depth, false};
    const All all = All{"all", difference, percent};

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbAllPercentExecute(config, all, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbAllPercentExecute, config, all, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &AllPercentCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionPercentSync::RgbRegionPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    const Config config = Config{width, height, depth, false};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbRegionPercentExecute(config, region, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbRegionPercentExecute, config, region, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionPercentCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionPercentAsync::RgbRegionPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();

    const Config config = Config{width, height, depth, false};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbRegionPercentExecute(config, region, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbRegionPercentExecute, config, region, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionPercentCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentSync::RgbRegionsPercentSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();

    const Config config = Config{width, height, depth, false};
    const std::vector<Region> regions = RegionsJsToCpp(regionsArr);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) { RgbRegionsPercentExecute(config, regions, buf0, buf1, results); };
    //this->execute_ = std::bind(&RgbRegionsPercentExecute, config, regions, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionsPercentCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentAsync::RgbRegionsPercentAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsPercentAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();

    const Config config = Config{width, height, depth, false};
    const std::vector<Region> regions = RegionsJsToCpp(regionsArr);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) { RgbRegionsPercentExecute(config, regions, buf0, buf1, results); };
    //this->execute_ = std::bind(&RgbRegionsPercentExecute, config, regions, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionsPercentCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsSync::RgbAllBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const All all = All{"all", difference, percent};

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbAllBoundsExecute(config, all, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbAllBoundsExecute, config, all, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &AllBoundsCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsAsync::RgbAllBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const All all = All{"all", difference, percent};

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbAllBoundsExecute(config, all, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbAllBoundsExecute, config, all, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &AllBoundsCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBoundsSync::RgbRegionBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbRegionBoundsExecute(config, region, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbRegionBoundsExecute, config, region, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionBoundsCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBoundsAsync::RgbRegionBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbRegionBoundsExecute(config, region, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbRegionBoundsExecute, config, region, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionBoundsCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsSync::RgbRegionsBoundsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const std::vector<Region> regions = RegionsJsToCpp(regionsArr);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) { RgbRegionsBoundsExecute(config, regions, buf0, buf1, results); };
    //this->execute_ = std::bind(&RgbRegionsBoundsExecute, config, regions, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionsBoundsCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsAsync::RgbRegionsBoundsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBoundsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const std::vector<Region> regions = RegionsJsToCpp(regionsArr);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) { RgbRegionsBoundsExecute(config, regions, buf0, buf1, results); };
    //this->execute_ = std::bind(&RgbRegionsBoundsExecute, config, regions, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionsBoundsCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBlobsSync::RgbAllBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const All all = All{"all", difference, percent};

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbAllBlobsExecute(config, all, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbAllBlobsExecute, config, all, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &AllBlobsCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBlobsAsync::RgbAllBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbAllBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const uint32_t difference = configObj.Get("difference").As<Napi::Number>().Uint32Value();
    const uint32_t percent = configObj.Get("percent").As<Napi::Number>().Uint32Value();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const All all = All{"all", difference, percent};

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbAllBlobsExecute(config, all, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbAllBlobsExecute, config, all, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &AllBlobsCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBlobsSync::RgbRegionBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbRegionBlobsExecute(config, region, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbRegionBlobsExecute, config, region, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionBlobsCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBlobsAsync::RgbRegionBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Object regionObj = configObj.Get("regions").As<Napi::Array>().Get("0").As<Napi::Object>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const Region region = RegionJsToCpp(regionObj);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {
        RgbRegionBlobsExecute(config, region, buf0, buf1, results);
    };
    //this->execute_ = std::bind(&RgbRegionBlobsExecute, config, region, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionBlobsCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBlobsSync::RgbRegionsBlobsSync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBlobsSync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const std::vector<Region> regions = RegionsJsToCpp(regionsArr);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) { RgbRegionsBlobsExecute(config, regions, buf0, buf1, results); };
    //this->execute_ = std::bind(&RgbRegionsBlobsExecute, config, regions, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionsBlobsCallback;
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
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    Results results;
    this->execute_(buf0, buf1, results);
    this->callback_(env, cb, results);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBlobsAsync::RgbRegionsBlobsAsync(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<RgbRegionsBlobsAsync>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);

    const Napi::Object configObj = info[0].As<Napi::Object>();
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = configObj.Get("regions").As<Napi::Array>();
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    const Config config = Config{width, height, depth, draw};
    const std::vector<Region> regions = RegionsJsToCpp(regionsArr);

    this->execute_ = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) { RgbRegionsBlobsExecute(config, regions, buf0, buf1, results); };
    //this->execute_ = std::bind(&RgbRegionsBlobsExecute, config, regions, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    this->callback_ = &RegionsBlobsCallback;
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
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *asyncWorker = new AsyncWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    asyncWorker->Queue();

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////