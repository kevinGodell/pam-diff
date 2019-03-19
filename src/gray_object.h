#ifndef SRC_GRAY_OBJECT_H_
#define SRC_GRAY_OBJECT_H_

#include "engine.h"
#include "napi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercentSync : public Napi::ObjectWrap<GrayAllPercentSync> {
public:
    explicit GrayAllPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class GrayAllPercentAsync : public Napi::ObjectWrap<GrayAllPercentAsync> {
public:
    explicit GrayAllPercentAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    All all_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionPercentSync : public Napi::ObjectWrap<GrayRegionPercentSync> {
public:
    explicit GrayRegionPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionPercentAsync : public Napi::ObjectWrap<GrayRegionPercentAsync> {
public:
    explicit GrayRegionPercentAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Region region_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsPercentSync : public Napi::ObjectWrap<GrayRegionsPercentSync> {
public:
    explicit GrayRegionsPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsPercentAsync : public Napi::ObjectWrap<GrayRegionsPercentAsync> {
public:
    explicit GrayRegionsPercentAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Regions regions_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBoundsSync : public Napi::ObjectWrap<GrayAllBoundsSync> {
public:
    explicit GrayAllBoundsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    All all_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBoundsAsync : public Napi::ObjectWrap<GrayAllBoundsAsync> {
public:
    explicit GrayAllBoundsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    All all_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionBoundsSync : public Napi::ObjectWrap<GrayRegionBoundsSync> {
public:
    explicit GrayRegionBoundsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Region region_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionBoundsAsync : public Napi::ObjectWrap<GrayRegionBoundsAsync> {
public:
    explicit GrayRegionBoundsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;

    Dimensions dimensions_;
    Region region_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBoundsSync : public Napi::ObjectWrap<GrayRegionsBoundsSync> {
public:
    explicit GrayRegionsBoundsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Regions regions_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBoundsAsync : public Napi::ObjectWrap<GrayRegionsBoundsAsync> {
public:
    explicit GrayRegionsBoundsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Regions regions_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class GrayAllBlobsSync : public Napi::ObjectWrap<GrayAllBlobsSync> {
public:
    explicit GrayAllBlobsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    All all_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBlobsAsync : public Napi::ObjectWrap<GrayAllBlobsAsync> {
public:
    explicit GrayAllBlobsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    All all_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionBlobsSync : public Napi::ObjectWrap<GrayRegionBlobsSync> {
public:
    explicit GrayRegionBlobsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Region region_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionBlobsAsync : public Napi::ObjectWrap<GrayRegionBlobsAsync> {
public:
    explicit GrayRegionBlobsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Region region_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBlobsSync : public Napi::ObjectWrap<GrayRegionsBlobsSync> {
public:
    explicit GrayRegionsBlobsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    Dimensions dimensions_;
    Regions regions_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif