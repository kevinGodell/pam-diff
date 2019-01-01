#include <napi.h>
#include "diff.h"
#include "async.h"

class AllDiffWorker : public Napi::AsyncWorker {

public:

    AllDiffWorker(const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDiff_(pixDiff), diffsPerc_(diffsPerc), pixCount_(pixCount), pixDepth_(pixDepth), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    }

    ~AllDiffWorker() {}

    void Execute() {
        switch (this->pixDepth_) {
            case 1:
                this->percentResult_ = DiffsPercent(pixDiff_, pixCount_, buf0_, buf1_);
                break;
            case 3:
            case 4:
                this->percentResult_ = DiffsPercent(pixDiff_, pixCount_, pixDepth_, buf0_, buf1_);
                break;
        }
    }

    void OnOK() {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);
        Napi::Array resultsArray = Napi::Array::New(env);
        if (this->percentResult_ >= this->diffsPerc_) {
            Napi::Object obj = Napi::Object::New(env);
            obj.Set("name", "all");
            obj.Set("percent", percentResult_);
            //resultsArray["0"] = obj;
            //resultsArray.Set(int(0), obj);
            resultsArray.Set(0u, obj);
        }
        //return first value null since there is no error
        Callback().Call({env.Null(), resultsArray});
    }

private:

    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

Napi::Value AllDiffAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t pixDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t pixCount = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDepth = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[6].As<Napi::Function>();
    AllDiffWorker *allDiffWorker = new AllDiffWorker(pixDiff, diffsPerc, pixCount, pixDepth, buf0, buf1, cb);
    allDiffWorker->Queue();
    return env.Undefined();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MaskDiffWorker : public Napi::AsyncWorker {

public:

    MaskDiffWorker(const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t *bitset, const uint_fast32_t bitsetCount, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDiff_(pixDiff), diffsPerc_(diffsPerc), pixCount_(pixCount), pixDepth_(pixDepth), bitset_(bitset), bitsetCount_(bitsetCount), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    }

    ~MaskDiffWorker() {}

    void Execute() {
        switch (this->pixDepth_) {
            case 1:
                this->percentResult_ = DiffsPercent(pixDiff_, pixCount_, bitset_, bitsetCount_, buf0_, buf1_);
                break;
            case 3:
            case 4:
                this->percentResult_ = DiffsPercent(pixDiff_, pixCount_, pixDepth_, bitset_, bitsetCount_, buf0_, buf1_);
                break;
        }
    }

    void OnOK() {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);
        Napi::Array resultsArray = Napi::Array::New(env);
        if (this->percentResult_ >= this->diffsPerc_) {
            Napi::Object obj = Napi::Object::New(env);
            obj.Set("name", "mask");
            obj.Set("percent", percentResult_);
            //resultsArray["0"] = obj;
            //resultsArray.Set(int(0), obj);
            resultsArray.Set(0u, obj);
        }
        //return first value null since there is no error
        Callback().Call({env.Null(), resultsArray});
    }

private:

    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDepth_;// bytes per pixel 1(gray) 3(rgb) 4(rgba)
    const uint_fast8_t *bitset_;// bitset buffer
    const uint_fast32_t bitsetCount_;// number of 1's in bitset, others are 0
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

Napi::Value MaskDiffAsync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t pixDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t pixCount = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDepth = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *bitset = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast32_t bitsetCount = info[5].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *buf0 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[7].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[8].As<Napi::Function>();
    MaskDiffWorker *maskDiffWorker = new MaskDiffWorker(pixDiff, diffsPerc, pixCount, pixDepth, bitset, bitsetCount, buf0, buf1, cb);
    maskDiffWorker->Queue();
    return env.Undefined();
}