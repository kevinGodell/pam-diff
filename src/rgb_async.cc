#include "rgb_async.h"
#include "diff.h"
#include "helper.h"
#include <stdint.h>
#include <napi.h>

class RgbDiffAllWorker : public Napi::AsyncWorker {

public:

    RgbDiffAllWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    }

    ~RgbDiffAllWorker() {}

    void Execute() {
        this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->buf0_, this->buf1_);
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
    const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

Napi::Value RgbDiffAllAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDepth = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDiff = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[6].As<Napi::Function>();

    RgbDiffAllWorker *rgbDiffAllWorker = new RgbDiffAllWorker(pixCount, pixDepth, pixDiff, diffsPerc, buf0, buf1, cb);
    rgbDiffAllWorker->Queue();

    return env.Undefined();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbDiffMaskWorker : public Napi::AsyncWorker {

public:

    RgbDiffMaskWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const uint_fast8_t *bitset, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitset_(bitset), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    }

    ~RgbDiffMaskWorker() {}

    void Execute() {
        this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->bitsetCount_, this->bitset_, this->buf0_, this->buf1_);
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
    const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast32_t bitsetCount_;// number of 1's in bitset, others are 0
    const uint_fast8_t *bitset_;// bitset buffer
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

Napi::Value RgbDiffMaskAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDepth = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDiff = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bitsetCount = info[4].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *bitset = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf0 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[7].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[8].As<Napi::Function>();

    RgbDiffMaskWorker *rgbDiffMaskWorker = new RgbDiffMaskWorker(pixCount, pixDepth, pixDiff, diffsPerc, bitsetCount, bitset, buf0, buf1, cb);
    rgbDiffMaskWorker->Queue();

    return env.Undefined();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbDiffRegionsWorker : public Napi::AsyncWorker {

public:

    RgbDiffRegionsWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, Region *regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), minDiff_(minDiff), regionsLen_(regionsLen), regionsCpp_(regions), buf0_(buf0), buf1_(buf1) {
    }

    ~RgbDiffRegionsWorker() {}

    void Execute() {
        MeasureDiffs(this->pixCount_, this->pixDepth_, this->minDiff_, this->regionsLen_, this->regionsCpp_, this->buf0_, this->buf1_);
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
    const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    const uint_fast8_t minDiff_;// can only be 0 - 255
    const uint_fast8_t regionsLen_;// number of 1's in bitset, others are 0
    Region *regionsCpp_;// Region array
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
};

Napi::Value RgbDiffRegionsAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDepth = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t minDiff = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t regionsLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsJs = info[4].As<Napi::Array>();
    const uint_fast8_t *buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[7].As<Napi::Function>();

    Region *regionsCpp = new Region[regionsLen]();// create array of type Region on heap

    regionsJsToCpp(regionsLen, regionsJs, regionsCpp);// convert js array to cpp array

    RgbDiffRegionsWorker *rgbDiffRegionsWorker = new RgbDiffRegionsWorker(pixCount, pixDepth, minDiff, regionsLen, regionsCpp, buf0, buf1, cb);
    rgbDiffRegionsWorker->Queue();

    return env.Undefined();
}