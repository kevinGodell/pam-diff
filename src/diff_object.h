#ifndef SRC_DIFF_OBJECT_H_
#define SRC_DIFF_OBJECT_H_

#include "diff.h"
#include <napi.h>

class Example : public Napi::ObjectWrap<Example> {

    public:

    static void Init(const Napi::Env &env, Napi::Object &exports);// initializes class and adds it to exports

    Example(const Napi::CallbackInfo &info);// constructor

    ~Example();

    private:

    static Napi::FunctionReference constructor;

    std::string myValue_;// used as a test for setter/getter

    std::string target_;// all, mask, region

    std::string filter_;// percent, bounds, blobs

    uint_fast32_t width_;// pixel width

    uint_fast32_t height_;// pixel height

    uint_fast32_t depth_;// 1, 3, 4

    uint_fast32_t pixCount_;// width * height * depth

    uint_fast8_t pixDiff_;// 1 - 255

    uint_fast8_t diffsPerc_;// 1 - 100

    uint_fast32_t bitsetCount_;// mask, count of 1's in bitset

    uint_fast8_t *bitset_;// mask, buffer of 0's and 1's based on index of targeted pixel

    uint_fast32_t minDiff_;// regions, minimum pixDiff when comparing all set regions

    uint_fast32_t regionsLen_;// regions, number of regions

    Region *regionsCpp_;// regions, cpp array of regions
    //Napi::Array regionsJs_;// regions, js array of objects

    bool async_;// using async or sync

    void Destroy();

    void Compare(const Napi::CallbackInfo &info);// instance method

    void SetMyValue(const Napi::CallbackInfo &info, const Napi::Value &value);// setter

    Napi::Value GetMyValue(const Napi::CallbackInfo &info);// getter

    Napi::Value (Example::*comparePtr_)(const uint_fast8_t *, const uint_fast8_t *, const Napi::Function &);// function pointer

    Napi::Value GrayAllPercentSync(const uint_fast8_t * buf0, const uint_fast8_t * buf1, const Napi::Function &cb);

    Napi::Value GrayMaskPercentSync(const uint_fast8_t * buf0, const uint_fast8_t * buf1, const Napi::Function &cb);

    Napi::Value GrayRegionsPercentSync(const uint_fast8_t * buf0, const uint_fast8_t * buf1, const Napi::Function &cb);
};

#endif