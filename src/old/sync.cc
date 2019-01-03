#include <napi.h>
#include "diff.h"
#include "sync.h"
#include <iostream>

Napi::Array DiffAllSync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t pixDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t pixCount = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDepth = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();

DiffsPercentStruct asf;
    uint_fast8_t percentResult = 0;
    switch (pixDepth) {
        case 1:// gray
            //percentResult = DiffsPercent(pixDiff, pixCount, buf0, buf1);
            //percentResult = 0;
            asf = DiffsPercent(pixDiff, pixCount, buf0, buf1);
            //std::cout << +asf.percent << std::endl;
            percentResult = asf.percent;
            break;
        case 3:// rgb
        case 4:// rgba
            percentResult = DiffsPercent(pixDiff, pixCount, pixDepth, buf0, buf1);
            break;
    }

    Napi::Array resultsArray = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", percentResult);
        resultsArray["0"] = obj;
    }
    return resultsArray;
}

Napi::Array DiffMaskSync(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const uint_fast8_t pixDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t pixCount = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t pixDepth = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *bitset = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast32_t bitsetCount = info[5].As<Napi::Number>().Uint32Value();
    const uint_fast8_t *buf0 = info[6].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[7].As<Napi::Buffer<uint_fast8_t>>().Data();

    uint_fast8_t percentResult = 0;
    switch (pixDepth) {
        case 1:// gray
            percentResult = DiffsPercent(pixDiff, pixCount, bitset, bitsetCount, buf0, buf1);
            break;
        case 3:// rgb
        case 4:// rgba
            percentResult = DiffsPercent(pixDiff, pixCount, pixDepth, bitset, bitsetCount, buf0, buf1);
            break;
    }

    Napi::Array resultsArray = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", percentResult);
        resultsArray["0"] = obj;
    }
    return resultsArray;
}