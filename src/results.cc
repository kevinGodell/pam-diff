#include "results.h"
#include "engine.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <napi.h>

void Results::ConvertToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult, Napi::Array &resultsJs) {
    if (percentResult >= diffsPerc) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("percent", percentResult);
        resultsJs[0u] = obj;
    }
}

void Results::ConvertToJs(const Napi::Env &env, const uint_fast8_t regLen, const std::vector<Engine::Region> &regionsVec, const std::vector<uint_fast32_t> &resultsVec, Napi::Array &resultsJs) {
    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * resultsVec[i] / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        resultsJs[j++] = obj;
    }
}