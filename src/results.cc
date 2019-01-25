#include "results.h"
#include "engine.h"
#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

// all/mask percent to js
Napi::Array
ToJs(const Napi::Env &env, const std::string &name, const uint_fast32_t diffsPerc, const uint_fast32_t percentResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("percent", percentResult);
        resultsJs.Set(0u, obj);
    }
    return resultsJs;
}

// regions percent to js
Napi::Array
ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const std::vector<uint_fast32_t> &percentResultVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t r = 0, j = 0; r < regionsLen; ++r) {
        if (regionVec[r].percent > percentResultVec[r]) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", regionVec[r].name);
        obj.Set("percent", percentResultVec[r]);
        resultsJs.Set(j++, obj);
    }
    return resultsJs;
}

// all/mask bounds to js
Napi::Array
ToJs(const Napi::Env &env, const std::string &name, const uint_fast32_t diffsPerc, const BoundsResult &boundsResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.percent >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("percent", boundsResult.percent);
        obj.Set("minX", boundsResult.minX);
        obj.Set("maxX", boundsResult.maxX);
        obj.Set("minY", boundsResult.minY);
        obj.Set("maxY", boundsResult.maxY);
        resultsJs.Set(0u, obj);
    }
    return resultsJs;
}

// regions bounds to js
Napi::Array
ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const std::vector<BoundsResult> &boundsResultVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t r = 0, j = 0; r < regionsLen; ++r) {
        if (regionVec[r].percent > boundsResultVec[r].percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", regionVec[r].name);
        obj.Set("percent", boundsResultVec[r].percent);
        obj.Set("minX", boundsResultVec[r].minX);
        obj.Set("maxX", boundsResultVec[r].maxX);
        obj.Set("minY", boundsResultVec[r].minY);
        obj.Set("maxY", boundsResultVec[r].maxY);
        resultsJs.Set(j++, obj);
    }
    return resultsJs;
}