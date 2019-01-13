#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <napi.h>

namespace Results {

    // all/mask percent to js
    Napi::Array ToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const uint_fast32_t percentResult);

    // regions percent to js
    Napi::Array ToJs(const Napi::Env &env, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const std::vector<uint_fast32_t> &percentResultVec);

    // all/mask bounds to js
    Napi::Array ToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const Engine::BoundsResult boundsResult);

    // regions bounds to js
    Napi::Array ToJs(const Napi::Env &env, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const std::vector<Engine::BoundsResult> &boundsResultVec);
}

#endif