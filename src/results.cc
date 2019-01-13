#include "results.h"
#include "engine.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <napi.h>

// all/mask percent to js
Napi::Array Results::ToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const uint_fast32_t percentResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
    return resultsJs;
}

// regions percent to js
Napi::Array Results::ToJs(const Napi::Env &env, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const std::vector<uint_fast32_t> &resultsVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regionsLen; i++) {
        percent = resultsVec[i];
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        resultsJs[j++] = obj;
    }
    return resultsJs;
}

// all/mask bounds to js
Napi::Array Results::ToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const Engine::BoundsResult boundsResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.percent >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("percent", boundsResult.percent);
        obj.Set("minX", boundsResult.minX);
        obj.Set("maxX", boundsResult.maxX);
        obj.Set("minY", boundsResult.minY);
        obj.Set("maxY", boundsResult.maxY);
        resultsJs[0u] = obj;
    }
    return resultsJs;
}

// regions bounds to js
Napi::Array Results::ToJs(const Napi::Env &env, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const std::vector<Engine::BoundsResult> &boundsResultVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0; i < regionsLen; i++) {
        Engine::BoundsResult boundsResult = boundsResultVec[i];
        if (std::get<2>(regionsVec[i]) > boundsResult.percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", boundsResult.percent);
        obj.Set("minX", boundsResult.minX);
        obj.Set("maxX", boundsResult.maxX);
        obj.Set("minY", boundsResult.minY);
        obj.Set("maxY", boundsResult.maxY);
        resultsJs[j++] = obj;
    }
    return resultsJs;
}