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

    Napi::Array jsArray = allResultsToJS(env, diffsPerc, percentResult);

    cb.Call({env.Null(), jsArray});

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

    Napi::Array jsArray = maskResultsToJS(env, diffsPerc, percentResult);

    cb.Call({env.Null(), jsArray});

    return env.Undefined();
}

Napi::Value RgbDiffRegionsSync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast32_t pixCount = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t depth = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast8_t minDiff = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t regionsLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[4].As<Napi::Array>();
    const uint_fast8_t *buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[7].As<Napi::Function>();

    Region *regions = new Region[regionsLen]();

    for (uint_fast32_t i = 0; i < regionsLen; i++) {
        const std::string name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const uint_fast8_t *bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>().Data();
        regions[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }

    MeasureDiffs(pixCount, depth, minDiff, regionsLen, regions, buf0, buf1);

    Napi::Array jsArray = regionsResultsToJS(env, regionsLen, regions);

    delete[] regions;

    cb.Call({env.Null(), jsArray});

    return env.Undefined();
}