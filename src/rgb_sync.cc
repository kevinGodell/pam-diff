#include "rgb_sync.h"
#include "diff.h"
#include "helper.h"
#include <stdint.h>
#include <napi.h>

Napi::Value RgbDiffAllSync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t depth = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDiff = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[6].As<Napi::Function>();

    uint_fast8_t percentResult = MeasureDiffs(pixCount, depth, pixDiff, buf0, buf1);

    Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback

    allResultsToJs(env, diffsPerc, percentResult, resultsJs);

    cb.Call({env.Null(), resultsJs});

    return env.Undefined();
}

Napi::Value RgbDiffMaskSync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t depth = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDiff = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bitsetCount = info[4].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *bitset = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf0 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[7].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[8].As<Napi::Function>();

    uint_fast8_t percentResult = MeasureDiffs(pixCount, depth, pixDiff, bitsetCount, bitset, buf0, buf1);

    Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback

    maskResultsToJs(env, diffsPerc, percentResult, resultsJs);

    cb.Call({env.Null(), resultsJs});

    return env.Undefined();
}

Napi::Value RgbDiffRegionsSync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t depth = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t minDiff = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t regionsLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsJs = info[4].As<Napi::Array>();
    const uint_fast8_t *buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[7].As<Napi::Function>();

    Region *regionsCpp = new Region[regionsLen]();// create array of type Region on heap

    regionsJsToCpp(regionsLen, regionsJs, regionsCpp);// convert js array to cpp array

    MeasureDiffs(pixCount, depth, minDiff, regionsLen, regionsCpp, buf0, buf1);// pass data to measure diffs

    Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback

    regionsResultsToJs(env, regionsLen, regionsCpp, resultsJs);//  convert cpp array to js results array

    delete[] regionsCpp;

    cb.Call({env.Null(), resultsJs});

    return env.Undefined();
}