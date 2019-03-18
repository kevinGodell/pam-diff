#ifndef SRC_GRAY_OBJECT_H_
#define SRC_GRAY_OBJECT_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercentSync : public Napi::ObjectWrap<GrayAllPercentSync> {
public:
    explicit GrayAllPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t pixCount_;
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
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t pixCount_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskPercentSync : public Napi::ObjectWrap<GrayMaskPercentSync> {
public:
    explicit GrayMaskPercentSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    uint_fast32_t width_;
    Bounds bounds_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskPercentAsync : public Napi::ObjectWrap<GrayMaskPercentAsync> {
public:
    explicit GrayMaskPercentAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    uint_fast32_t width_;
    Bounds bounds_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
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
    //uint_fast32_t pixCount_;
    uint_fast32_t width_;
    Bounds bounds_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
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
    //uint_fast32_t pixCount_;
    uint_fast32_t width_;
    Bounds bounds_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBoundsSync : public Napi::ObjectWrap<GrayMaskBoundsSync> {
public:
    explicit GrayMaskBoundsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;
    uint_fast32_t width_;
    uint_fast32_t pixCount_;
    Bounds bounds_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBoundsAsync : public Napi::ObjectWrap<GrayMaskBoundsAsync> {
public:
    explicit GrayMaskBoundsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;

    uint_fast32_t width_;
    Bounds bounds_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t minDiff_;
    std::vector<Region> regionVec_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBlobsSync : public Napi::ObjectWrap<GrayMaskBlobsSync> {
public:
    explicit GrayMaskBlobsSync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;

    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    Bounds bounds_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBlobsAsync : public Napi::ObjectWrap<GrayMaskBlobsAsync> {
public:
    explicit GrayMaskBlobsAsync(const Napi::CallbackInfo &info);

    static void Init(const Napi::Env &env);

    static Napi::Object NewInstance(const Napi::Env &env, const Napi::Object &config);

private:
    Napi::Value Compare(const Napi::CallbackInfo &info);

    static Napi::FunctionReference constructor;

    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    Bounds bounds_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
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
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t minDiff_;
    uint_fast32_t minX_;
    uint_fast32_t maxX_;
    uint_fast32_t minY_;
    uint_fast32_t maxY_;
    std::vector<bool> maskVec_;
    std::vector<Region> regionVec_;
    bool draw_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif