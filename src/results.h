#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <napi.h>

namespace Results {

    Napi::Array ConvertToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult);

    Napi::Array ConvertToJs(const Napi::Env &env, const uint_fast8_t regLen, const std::vector<Engine::Region> &regionsVec, const std::vector<uint_fast8_t> &resultsVec);

    Napi::Array ConvertToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const Engine::BoundsResult boundsResult);

}

#endif