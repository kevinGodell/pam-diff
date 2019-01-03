#include "gray_async.h"
#include "diff.h"
#include "helper.h"
#include <napi.h>

class GrayDiffAllWorker : public Napi::AsyncWorker {

public:

    GrayDiffAllWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    }

    ~GrayDiffAllWorker() {}

    void Execute() {
        this->percentResult_ = MeasureDiffs(pixCount_, pixDiff_, buf0_, buf1_);
    }

    void OnOK() {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);

        Napi::Array jsArray = allResultsToJS(env, this->diffsPerc_, this->percentResult_);

        Callback().Call({env.Null(), jsArray});
    }

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    //const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

Napi::Value GrayDiffAllAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDiff = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *buf0 = info[3].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[5].As<Napi::Function>();

    GrayDiffAllWorker *grayDiffAllWorker = new GrayDiffAllWorker(pixCount, pixDiff, diffsPerc, buf0, buf1, cb);
    grayDiffAllWorker->Queue();

    return env.Undefined();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayDiffMaskWorker : public Napi::AsyncWorker {

public:

    GrayDiffMaskWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const uint_fast8_t *bitset, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitset_(bitset), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    }

    ~GrayDiffMaskWorker() {}

    void Execute() {
        this->percentResult_ = MeasureDiffs(pixCount_, pixDiff_, bitsetCount_, bitset_, buf0_, buf1_);
    }

    void OnOK() {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);

        Napi::Array jsArray = maskResultsToJS(env, this->diffsPerc_, this->percentResult_);

        Callback().Call({env.Null(), jsArray});
    }

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast32_t bitsetCount_;// number of 1's in bitset, others are 0
    //const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    const uint_fast8_t *bitset_;// bitset buffer
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

Napi::Value GrayDiffMaskAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDiff = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bitsetCount = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *bitset = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[7].As<Napi::Function>();

    GrayDiffMaskWorker *grayDiffMaskWorker = new GrayDiffMaskWorker(pixCount, pixDiff, diffsPerc, bitsetCount, bitset, buf0, buf1, cb);
    grayDiffMaskWorker->Queue();

    return env.Undefined();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayDiffRegionsWorker : public Napi::AsyncWorker {

public:

    GrayDiffRegionsWorker(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, Region *regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regions_(regions), buf0_(buf0), buf1_(buf1) {
    }

    ~GrayDiffRegionsWorker() {}

    void Execute() {
        MeasureDiffs(pixCount_, minDiff_, regionsLen_, regions_, buf0_, buf1_);
    }

    void OnOK() {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);

        Napi::Array jsArray = regionsResultsToJS(env, this->regionsLen_, this->regions_);

        delete[] this->regions_;

        Callback().Call({env.Null(), jsArray});
    }

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t minDiff_;// can only be 0 - 255
    const uint_fast8_t regionsLen_;// number of 1's in bitset, others are 0
    //const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    Region *regions_;// Region array
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    //uint_fast8_t percentResult_;// percent placeholder
};

Napi::Value GrayDiffRegionsAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t minDiff = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t regionsLen = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[3].As<Napi::Array>();
    const uint_fast8_t *buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[6].As<Napi::Function>();

    Region *regions = new Region[regionsLen]();

    for (uint_fast32_t i = 0; i < regionsLen; i++) {
        const std::string name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const uint_fast8_t *bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>().Data();
        regions[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }

    GrayDiffRegionsWorker *grayDiffRegionsWorker = new GrayDiffRegionsWorker(pixCount, minDiff, regionsLen, regions, buf0, buf1, cb);
    grayDiffRegionsWorker->Queue();

    return env.Undefined();
}