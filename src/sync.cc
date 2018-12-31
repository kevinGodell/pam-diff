#include <napi.h>
#include "diff.h"
#include "sync.h"

Napi::Array PixelsDiffSync(const Napi::CallbackInfo &info) {
    const Napi::Env           env = info.Env();
    const uint_fast8_t   pixDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast8_t diffsPerc = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t pixCount = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast8_t  pixDepth = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast8_t*  buffer0 = info[4].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t*  buffer1 = info[5].As<Napi::Buffer<uint_fast8_t>>().Data();

    uint_fast8_t percentResult;
    switch(pixDepth) {
        case 1:
            percentResult = DiffsPercent(pixDiff, pixCount, buffer0, buffer1);
            break;
        case 3:
        case 4:
            percentResult = DiffsPercent(pixDiff, pixCount, pixDepth, buffer0, buffer1);
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