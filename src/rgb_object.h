#ifndef SRC_RGB_OBJECT_H_
#define SRC_RGB_OBJECT_H_

#include "engine.h"
#include "napi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercentSync : public Napi::ObjectWrap<RgbAllPercentSync> {
public:
    explicit RgbAllPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class RgbAllPercentAsync : public Napi::ObjectWrap<RgbAllPercentAsync> {
public:
    explicit RgbAllPercentAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionPercentSync : public Napi::ObjectWrap<RgbRegionPercentSync> {
public:
    explicit RgbRegionPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionPercentAsync : public Napi::ObjectWrap<RgbRegionPercentAsync> {
public:
    explicit RgbRegionPercentAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsPercentSync : public Napi::ObjectWrap<RgbRegionsPercentSync> {
public:
    explicit RgbRegionsPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsPercentAsync : public Napi::ObjectWrap<RgbRegionsPercentAsync> {
public:
    explicit RgbRegionsPercentAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBoundsSync : public Napi::ObjectWrap<RgbAllBoundsSync> {
public:
    explicit RgbAllBoundsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBoundsAsync : public Napi::ObjectWrap<RgbAllBoundsAsync> {
public:
    explicit RgbAllBoundsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionBoundsSync : public Napi::ObjectWrap<RgbRegionBoundsSync> {
public:
    explicit RgbRegionBoundsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionBoundsAsync : public Napi::ObjectWrap<RgbRegionBoundsAsync> {
public:
    explicit RgbRegionBoundsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;

    Config config_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBoundsSync : public Napi::ObjectWrap<RgbRegionsBoundsSync> {
public:
    explicit RgbRegionsBoundsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBoundsAsync : public Napi::ObjectWrap<RgbRegionsBoundsAsync> {
public:
    explicit RgbRegionsBoundsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class RgbAllBlobsSync : public Napi::ObjectWrap<RgbAllBlobsSync> {
public:
    explicit RgbAllBlobsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBlobsAsync : public Napi::ObjectWrap<RgbAllBlobsAsync> {
public:
    explicit RgbAllBlobsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionBlobsSync : public Napi::ObjectWrap<RgbRegionBlobsSync> {
public:
    explicit RgbRegionBlobsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionBlobsAsync : public Napi::ObjectWrap<RgbRegionBlobsAsync> {
public:
    explicit RgbRegionBlobsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBlobsSync : public Napi::ObjectWrap<RgbRegionsBlobsSync> {
public:
    explicit RgbRegionsBlobsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class RgbRegionsBlobsAsync : public Napi::ObjectWrap<RgbRegionsBlobsAsync> {
public:
    explicit RgbRegionsBlobsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Config config_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif