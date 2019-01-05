#ifndef SRC_HELPER_H_
#define SRC_HELPER_H_

#include "diff.h"
#include <stdint.h>
#include <tuple>
#include <string>
#include <napi.h>

inline void allResultsToJs(const Napi::Env &env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult, Napi::Array &resultsJs) {
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
}

inline void maskResultsToJs(const Napi::Env &env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult, Napi::Array &resultsJs) {
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
}

inline void regionsResultsToJs(const Napi::Env &env, const uint_fast8_t regLen, const Region *regionsCpp, Napi::Array &resultsJs) {
    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * std::get<5>(regionsCpp[i]) / std::get<3>(regionsCpp[i]);
        if (std::get<2>(regionsCpp[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsCpp[i]));
        obj.Set("percent", percent);
        resultsJs[j++] = obj;
    }
}

inline void regionsJsToCpp(const uint_fast8_t regionsLen, const Napi::Array &regionsJs, Region *regionsCpp) {
    for (uint_fast32_t i = 0; i < regionsLen; i++) {
        const std::string name = regionsJs.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsJs.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsJs.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsJs.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const uint_fast8_t *bitset = regionsJs.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>().Data();
        regionsCpp[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }
}

#endif