#ifndef SRC_DIFF_OBJECT_H_
#define SRC_DIFF_OBJECT_H_

#include "engine.h"
#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercentSync : public Napi::ObjectWrap<GrayAllPercentSync> {
public:
    explicit GrayAllPercentSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercentAsync : public Napi::ObjectWrap<GrayAllPercentAsync> {
public:
    explicit GrayAllPercentAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskPercentSync : public Napi::ObjectWrap<GrayMaskPercentSync> {
public:
    explicit GrayMaskPercentSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
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
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
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
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t pixCount_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsPercentAsync : public Napi::ObjectWrap<GrayRegionsPercentAsync> {
public:
    explicit GrayRegionsPercentAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t pixCount_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBoundsSync : public Napi::ObjectWrap<GrayAllBoundsSync> {
public:
    explicit GrayAllBoundsSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBoundsAsync : public Napi::ObjectWrap<GrayAllBoundsAsync> {
public:
    explicit GrayAllBoundsAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBoundsSync : public Napi::ObjectWrap<GrayMaskBoundsSync> {
public:
    explicit GrayMaskBoundsSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBoundsAsync : public Napi::ObjectWrap<GrayMaskBoundsAsync> {
public:
    explicit GrayMaskBoundsAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBoundsSync : public Napi::ObjectWrap<GrayRegionsBoundsSync> {
public:
    explicit GrayRegionsBoundsSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBoundsAsync : public Napi::ObjectWrap<GrayRegionsBoundsAsync> {
public:
    explicit GrayRegionsBoundsAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercentSync : public Napi::ObjectWrap<RgbAllPercentSync> {
public:
    explicit RgbAllPercentSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t bufLen_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercentAsync : public Napi::ObjectWrap<RgbAllPercentAsync> {
public:
    explicit RgbAllPercentAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t bufLen_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskPercentSync : public Napi::ObjectWrap<RgbMaskPercentSync> {
public:
    explicit RgbMaskPercentSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t bufLen_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskPercentAsync : public Napi::ObjectWrap<RgbMaskPercentAsync> {
public:
    explicit RgbMaskPercentAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t bufLen_;
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
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t bufLen_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsPercentAsync : public Napi::ObjectWrap<RgbRegionsPercentAsync> {
public:
    explicit RgbRegionsPercentAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t bufLen_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBoundsSync : public Napi::ObjectWrap<RgbAllBoundsSync> {
public:
    explicit RgbAllBoundsSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBoundsAsync : public Napi::ObjectWrap<RgbAllBoundsAsync> {
public:
    explicit RgbAllBoundsAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskBoundsSync : public Napi::ObjectWrap<RgbMaskBoundsSync> {
public:
    explicit RgbMaskBoundsSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskBoundsAsync : public Napi::ObjectWrap<RgbMaskBoundsAsync> {
public:
    explicit RgbMaskBoundsAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    std::string target_;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t pixDiff_;
    uint_fast32_t diffsPerc_;
    uint_fast32_t bitsetCount_;
    std::vector<bool> bitsetVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBoundsSync : public Napi::ObjectWrap<RgbRegionsBoundsSync> {
public:
    explicit RgbRegionsBoundsSync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBoundsAsync : public Napi::ObjectWrap<RgbRegionsBoundsAsync> {
public:
    explicit RgbRegionsBoundsAsync(const Napi::CallbackInfo &info);
    static void Init(const Napi::Env &env);
    static Napi::Object NewInstance(Napi::Env &env, const Napi::Object &config);
private:
    Napi::Value Compare(const Napi::CallbackInfo &info);
    static Napi::FunctionReference constructor;
    uint_fast32_t width_;
    uint_fast32_t height_;
    uint_fast32_t pixCount_;
    uint_fast32_t pixDepth_;
    uint_fast32_t minDiff_;
    uint_fast32_t regionsLen_;
    std::vector<Region> regionVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif