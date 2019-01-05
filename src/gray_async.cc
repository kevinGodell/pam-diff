#include "gray_async.h"
#include "diff.h"
#include "helper.h"
#include <stdint.h>
#include <napi.h>

class GrayDiffAllWorker : public Napi::AsyncWorker {

public:

    GrayDiffAllWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    }

    ~GrayDiffAllWorker() {}

    void Execute() {
        this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
    }

    void OnOK() {
        const Napi::Env env = Env();

        Napi::HandleScope scope(env);

        Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback

        allResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);

        Callback().Call({env.Null(), resultsJs});
    }

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
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
        this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitset_, this->buf0_, this->buf1_);
    }

    void OnOK() {
        const Napi::Env env = Env();

        Napi::HandleScope scope(env);

        Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback

        maskResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);

        Callback().Call({env.Null(), resultsJs});
    }

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast32_t bitsetCount_;// number of 1's in bitset, others are 0
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
        : Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionsCpp_(regions), buf0_(buf0), buf1_(buf1) {
    }

    ~GrayDiffRegionsWorker() {}

    void Execute() {
        MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsCpp_, this->buf0_, this->buf1_);
    }

    void OnOK() {
        const Napi::Env env = Env();

        Napi::HandleScope scope(env);

        Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback

        regionsResultsToJs(env, this->regionsLen_, this->regionsCpp_, resultsJs);

        delete[] this->regionsCpp_;

        Callback().Call({env.Null(), resultsJs});
    }

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t minDiff_;// can only be 0 - 255
    const uint_fast8_t regionsLen_;// number of 1's in bitset, others are 0
    Region *regionsCpp_;// Region array
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
};

Napi::Value GrayDiffRegionsAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t minDiff = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t regionsLen = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsJs = info[3].As<Napi::Array>();
    const uint_fast8_t *buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[6].As<Napi::Function>();

    Region *regionsCpp = new Region[regionsLen]();// create array of type Region on heap

    regionsJsToCpp(regionsLen, regionsJs, regionsCpp);// convert js array to cpp array

    GrayDiffRegionsWorker *grayDiffRegionsWorker = new GrayDiffRegionsWorker(pixCount, minDiff, regionsLen, regionsCpp, buf0, buf1, cb);
    grayDiffRegionsWorker->Queue();

    return env.Undefined();
}