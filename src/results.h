#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <napi.h>

namespace Results {
    void ConvertToJs(const Napi::Env &env, const std::string name, const uint_fast8_t diffsPerc, const uint_fast8_t percentResult, Napi::Array &resultsJs);
    void ConvertToJs(const Napi::Env &env, const uint_fast8_t regLen, const std::vector<Region> &regionsVec, const std::vector<uint_fast32_t> &resultsVec, Napi::Array &resultsJs);
}

#endif