#ifndef HELPER_H_
#define HELPER_H_

#include "diff.h"
#include <napi.h>

inline Napi::Array allResultsToJS(const Napi::Env env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult) {
    Napi::Array resultsArray = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", percentResult);
        resultsArray["0"] = obj;
    }
    return resultsArray;
}

inline Napi::Array maskResultsToJS(const Napi::Env env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult) {
    Napi::Array jsArray = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", percentResult);
        jsArray["0"] = obj;
    }
    return jsArray;
}

inline Napi::Array regionsResultsToJS(const Napi::Env env, const uint_fast8_t regLen, Region *regions) {
    Napi::Array jsArray = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * std::get<5>(regions[i]) / std::get<3>(regions[i]);
        if (std::get<2>(regions[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regions[i]));
        obj.Set("percent", percent);
        jsArray[j++] = obj;
    }
    return jsArray;
}

#endif