#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_

#include "engine.h"
#include "napi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PixelChange : public Napi::ObjectWrap<PixelChange> {
public:
    explicit PixelChange(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    Napi::Value CompareSync(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    ExecuteFunc execute_;
    CallbackFunc callback_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif