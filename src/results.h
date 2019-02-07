#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

// all/mask percent to js
Napi::Array
ToJs(const Napi::Env &env, const std::string &name, const PercentResult &percentResult);

// regions percent to js
Napi::Array
ToJs(const Napi::Env &env, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const std::vector<PercentResult> &percentResultsVec);

// all/mask bounds to js
Napi::Array
ToJs(const Napi::Env &env, const std::string &name, const BoundsResult &boundsResult);

// regions bounds to js
Napi::Array
ToJs(const Napi::Env &env, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const std::vector<BoundsResult> &boundsResultsVec);

// draw bounding box in gray pixels
void
DrawGrayBounds(const Napi::Array &resultsJs, uint_fast32_t width, uint_fast8_t *pixels);

// draw bounding box in rgb pixels
void
DrawRgbBounds(const Napi::Array &resultsJs, uint_fast32_t width, uint_fast32_t pixDepth, uint_fast8_t *pixels);

// all/mask bounds to js
void
ToJs(const Napi::Env &env, const std::string &name, const BoundsResult &boundsResult, Napi::Array &resultsJs);

// regions bounds to js
void
ToJs(const Napi::Env &env, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const std::vector<BoundsResult> &boundsResultsVec, Napi::Array &resultsJs);

// draw bounding box in gray pixels for all/mask
void
DrawGrayBounds2(const BoundsResult &boundsResult, const uint_fast32_t width, uint_fast8_t *pixels);

// draw bounding box in gray pixels for regions
void
DrawGrayBounds2(const uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, uint_fast8_t *pixels);

#endif