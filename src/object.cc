#include "object.h"
#include "worker.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PixelChange::PixelChange(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<PixelChange>(info) {
    const Napi::Env env = info.Env();
    const Napi::HandleScope scope(env);
    const Napi::Object configObj = info[0].As<Napi::Object>();
    SetFunctions(configObj, this->execute_, this->callback_);
}

Napi::FunctionReference PixelChange::constructor;

void PixelChange::Init(const Napi::Env &env) {
    const Napi::HandleScope scope(env);
    const Napi::Function func = DefineClass(env, "PixelChange", {
            InstanceMethod("compare", &PixelChange::Compare),
            InstanceMethod("compareSync", &PixelChange::CompareSync)
    });
    PixelChange::constructor = Napi::Persistent(func);
    PixelChange::constructor.SuppressDestruct();
}

Napi::Object PixelChange::NewInstance(const Napi::Env &env, const Napi::Object &config) {
    Napi::EscapableHandleScope scope(env);
    const Napi::Object object = PixelChange::constructor.New({config});
    return scope.Escape(napi_value(object)).ToObject();
}

Napi::Value PixelChange::Compare(const Napi::CallbackInfo &info) {
    const Napi::Env &env = info.Env();
    const Napi::Buffer<uint8_t> &napiBuf0 = info[0].As<Napi::Buffer<uint8_t>>();
    const Napi::Buffer<uint8_t> &napiBuf1 = info[1].As<Napi::Buffer<uint8_t>>();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    auto *pixelChangeWorker = new PixelChangeWorker(this->execute_, this->callback_, napiBuf0, napiBuf1, cb);
    pixelChangeWorker->Queue();

    return env.Undefined();
}

Napi::Value PixelChange::CompareSync(const Napi::CallbackInfo &info) {
    const Napi::Env &env = info.Env();
    const uint8_t *buf0 = info[0].As<Napi::Buffer<uint8_t>>().Data();
    const uint8_t *buf1 = info[1].As<Napi::Buffer<uint8_t>>().Data();
    const Napi::Function &cb = info[2].As<Napi::Function>();

    CallbackData callbackData;
    this->execute_(buf0, buf1, callbackData);
    this->callback_(env, cb, callbackData);

    return env.Undefined();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////