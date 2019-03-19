#ifndef SRC_RGB_OBJECT_H_
#define SRC_RGB_OBJECT_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercentSync : public Napi::ObjectWrap<RgbAllPercentSync> {
public:
    explicit RgbAllPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
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
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
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
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
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
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
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
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
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
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t buf1Size_;
    bool draw_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    bool draw_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
    uint_fast32_t buf1Size_;
    bool draw_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
    bool draw_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
    uint_fast32_t buf1Size_;
    bool draw_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif