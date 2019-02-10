#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

// all/mask percent to js
void
ToJs(const Napi::Env &env, const PercentResult &percentResult, Napi::Array &resultsJs);

// regions percent to js
void
ToJs(const Napi::Env &env, uint_fast32_t regionsLen, const std::vector<PercentResult> &percentResultsVec, Napi::Array &resultsJs);

// all/mask bounds to js
void
ToJs(const Napi::Env &env, const BoundsResult &boundsResult, Napi::Array &resultsJs);

// regions bounds to js
void
ToJs(const Napi::Env &env, uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultsVec, Napi::Array &resultsJs);

// draw bounding box in gray pixels for all/mask
void
DrawGrayBounds(const BoundsResult &boundsResult, uint_fast32_t width, uint_fast8_t *pixels);

// draw bounding box in gray pixels for regions
void
DrawGrayBounds(uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, uint_fast32_t width, uint_fast8_t *pixels);

// draw bounding box in rgb pixels for all/mask
void
DrawRgbBounds(const BoundsResult &boundsResult, uint_fast32_t width, uint_fast32_t pixDepth, uint_fast8_t *pixels);

// draw bounding box in rgb pixels for regions
void
DrawRgbBounds(uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, uint_fast32_t width, uint_fast32_t pixDepth, uint_fast8_t *pixels);

#endif