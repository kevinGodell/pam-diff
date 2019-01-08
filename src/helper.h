#ifndef SRC_HELPER_H_
#define SRC_HELPER_H_

#include "diff.h"
#include <stdint.h>
#include <tuple>
#include <string>
#include <napi.h>

enum {//if not service impacting, may always return bounds(min max x y), in which case filtering will be blobs or no blobs
    GRAY_ALL_PERCENT_SYNC = 0,
    GRAY_MASK_PERCENT_SYNC = 10,
    GRAY_REGIONS_PERCENT_SYNC = 20,
    GRAY_ALL_BOUNDS_SYNC = 100,
    GRAY_MASK_BOUNDS_SYNC = 110,
    GRAY_REGIONS_BOUNDS_SYNC = 120,
    GRAY_ALL_BLOBS_SYNC = 200,
    GRAY_MASK_BLOBS_SYNC = 210,
    GRAY_REGIONS_BLOBS_SYNC = 220,
    GRAY_ALL_PERCENT_ASYNC = 1000,
    GRAY_MASK_PERCENT_ASYNC = 1010,
    GRAY_REGIONS_PERCENT_ASYNC = 1020,
    GRAY_ALL_BOUND_ASYNC = 1100,
    GRAY_MASK_BOUNDS_ASYNC = 1110,
    GRAY_REGIONS_BOUNDS_ASYNC = 1120,
    GRAY_ALL_BLOBS_ASYNC = 1200,
    GRAY_MASK_BLOBS_ASYNC = 1210,
    GRAY_REGIONS_BLOBS_ASYNC = 1220
};

inline uint_fast32_t engineType(const uint_fast32_t depth, const std::string target, const std::string returns, const bool async) {
    uint_fast32_t value = 0;
    //dont add for depth == 1
    if (depth == 3) {
        value += 1;
    } else if (depth == 4) {
        value += 2;
    }
    //dont add for target == "all"
    if (target == "mask") {
        value += 10;
    } else if (target == "regions") {
        value += 20;
    }
    //dont add for target == "percent"
    if (returns == "bounds") {
        value += 100;
    } else if (returns == "blobs") {
        value += 200;
    }
    if (async) {
        value += 1000;
    }
    return value;
}

inline void allResultsToJs(const Napi::Env &env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult, Napi::Array &resultsJs) {
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
}

inline Napi::Array allResultsToJs(const Napi::Env &env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
    return resultsJs;
}

inline void maskResultsToJs(const Napi::Env &env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult, Napi::Array &resultsJs) {
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
}

inline Napi::Array maskResultsToJs(const Napi::Env &env, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
    return resultsJs;
}

inline void regionsResultsToJs(const Napi::Env &env, const uint_fast8_t regLen, Region *regionsCpp, Napi::Array &resultsJs) {
    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * std::get<5>(regionsCpp[i]) / std::get<3>(regionsCpp[i]);
        std::get<5>(regionsCpp[i]) = 0;//have to reset to 0 in case re-using, todo will have to separate input from output separate object
        if (std::get<2>(regionsCpp[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsCpp[i]));
        obj.Set("percent", percent);
        resultsJs[j++] = obj;
    }
}

//Napi::Array resultsJs = Napi::Array::New(env);

//vec
inline Napi::Array regionsResultsToJs(const Napi::Env &env, const uint_fast8_t regLen, std::vector<Region> &regionsVec, std::vector<uint_fast32_t> &resultsVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * resultsVec[i] / std::get<3>(regionsVec[i]);
        //std::get<5>(regionsVec[i]) = 0;//have to reset to 0 in case re-using, todo will have to separate input from output separate object
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        resultsJs[j++] = obj;
    }
    return resultsJs;
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

inline void regionsJsToCpp(const uint_fast8_t regionsLen, const Napi::Array &regionsJs, std::vector<Region> &regionsVec) {
    for (uint_fast32_t i = 0; i < regionsLen; i++) {
        const std::string name = regionsJs.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsJs.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsJs.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsJs.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const uint_fast8_t *bitset = regionsJs.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>().Data();
        //regionsCpp[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
        regionsVec.push_back(std::make_tuple(name, diff, percent, count, bitset, 0));
    }
}

#endif