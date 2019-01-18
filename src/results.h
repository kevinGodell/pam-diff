#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

namespace Results {

    // all/mask percent to js
    Napi::Array ToJs(const Napi::Env &env, const std::string &name, uint_fast32_t diffsPerc, uint_fast32_t percentResult);

    // regions percent to js
    Napi::Array ToJs(const Napi::Env &env, uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionsVec, const std::vector<uint_fast32_t> &percentResultsVec);

    // all/mask bounds to js
    Napi::Array ToJs(const Napi::Env &env, const std::string &name, uint_fast32_t diffsPerc, Engine::BoundsResult boundsResult);

    // regions bounds to js
    Napi::Array ToJs(const Napi::Env &env, uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionsVec, const std::vector<Engine::BoundsResult> &boundsResultsVec);

}

#endif