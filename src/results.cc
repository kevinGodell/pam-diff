#include "results.h"
#include "engine.h"
#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

// all/mask percent to js
Napi::Array Results::ToJs(const Napi::Env &env, const std::string &name, const uint_fast32_t diffsPerc, const uint_fast32_t percentResult) {
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
Napi::Array Results::ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionVec, const std::vector<uint_fast32_t> &percentResultVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t r = 0, j = 0, percent = 0; r < regionsLen; r++) {
        percent = percentResultVec[r];
        Engine::Region region = regionVec[r];
        if (region.percent > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", region.name);
        obj.Set("percent", percent);
        resultsJs.Set(j++, obj);
    }
    return resultsJs;
}

// all/mask bounds to js
Napi::Array Results::ToJs(const Napi::Env &env, const std::string &name, const uint_fast32_t diffsPerc, const Engine::BoundsResult boundsResult) {
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
Napi::Array Results::ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionVec, const std::vector<Engine::BoundsResult> &boundsResultVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t r = 0, j = 0; r < regionsLen; r++) {
        Engine::BoundsResult boundsResult = boundsResultVec[r];
        Engine::Region region = regionVec[r];
        if (region.percent > boundsResult.percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", region.name);
        obj.Set("percent", boundsResult.percent);
        obj.Set("minX", boundsResult.minX);
        obj.Set("maxX", boundsResult.maxX);
        obj.Set("minY", boundsResult.minY);
        obj.Set("maxY", boundsResult.maxY);
        resultsJs.Set(j++, obj);
    }
    return resultsJs;
}