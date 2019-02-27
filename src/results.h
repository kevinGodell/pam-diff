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
ToJs(const Napi::Env &env, const std::vector<PercentResult> &percentResultVec, Napi::Array &resultsJs);

// all/mask bounds to js
void
ToJs(const Napi::Env &env, const BoundsResult &boundsResult, Napi::Array &resultsJs);

// regions bounds to js
void
ToJs(const Napi::Env &env, const std::vector<BoundsResult> &boundsResultVec, Napi::Array &resultsJs);

// all/mask blobs to js
void
ToJs(const Napi::Env &env, const BlobsResult &blobsResult, Napi::Array &resultsJs);

// draw bounding box in gray pixels for all/mask
void
DrawGray(const BoundsResult &boundsResult, uint_fast32_t width, uint_fast8_t *pixels);

// draw bounding box in gray pixels for regions
void
DrawGray(const std::vector<BoundsResult> &boundsResultVec, uint_fast32_t width, uint_fast8_t *pixels);

// draw blobs bounding box in gray pixels for all/mask
void
DrawGray(const BlobsResult &blobsResult, uint_fast32_t width, uint_fast8_t *pixels);

// draw bounding box in rgb pixels for all/mask
void
DrawRgb(const BoundsResult &boundsResult, uint_fast32_t width, uint_fast32_t pixDepth, uint_fast8_t *pixels);

// draw bounding box in rgb pixels for regions
void
DrawRgb(const std::vector<BoundsResult> &boundsResultVec, uint_fast32_t width, uint_fast32_t pixDepth, uint_fast8_t *pixels);

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env env, const uint_fast8_t *finalizeData);

#endif