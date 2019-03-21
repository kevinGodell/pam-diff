#include "results.h"
#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

// all/mask percent to js
void
ToJs(const Napi::Env &env, const PercentResult &percentResult, Napi::Array &resultsJs) {
    SetPercentResult(env, percentResult, resultsJs);
}

// regions percent to js
void
ToJs(const Napi::Env &env, const std::vector<PercentResult> &percentResultVec, Napi::Array &resultsJs) {
    uint_fast32_t j = 0;
    for (const auto &percentResult : percentResultVec) {
        if (!percentResult.flagged) continue;
        SetPercentResult(env, percentResult, resultsJs, j++);
    }
}

// all/mask bounds to js
void
ToJs(const Napi::Env &env, const BoundsResult &boundsResult, Napi::Array &resultsJs) {
    SetBoundsResult(env, boundsResult, resultsJs);
}

// regions bounds to js
void
ToJs(const Napi::Env &env, const std::vector<BoundsResult> &boundsResultVec, Napi::Array &resultsJs) {
    uint_fast32_t j = 0;
    for (const auto &boundsResult : boundsResultVec) {
        if (!boundsResult.flagged) continue;
        SetBoundsResult(env, boundsResult, resultsJs, j++);
    }
}

// all/mask blobs to js
void
ToJs(const Napi::Env &env, const BlobsResult &blobsResult, Napi::Array &resultsJs) {
    SetBlobsResult(env, blobsResult, resultsJs);
}

// regions blobs to js
void
ToJs(const Napi::Env &env, const std::vector<BlobsResult> &blobsResultVec, Napi::Array &resultsJs) {
    uint_fast32_t j = 0;
    for (const auto &blobsResult : blobsResultVec) {
        if (!blobsResult.flagged) continue;
        SetBlobsResult(env, blobsResult, resultsJs, j++);
    }
}

// draw bounding box in gray pixels for all/mask
void
DrawGray(const BoundsResult &boundsResult, const uint_fast32_t width, uint_fast8_t *pixels) {
    SetGrayPixels(boundsResult.bounds, width, pixels);
}

// draw bounding box in gray pixels for regions
void
DrawGray(const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (const auto &boundsResult : boundsResultVec) {
        if (!boundsResult.flagged) continue;
        SetGrayPixels(boundsResult.bounds, width, pixels);
    }
}

// draw blobs bounding box in gray pixels for all/mask
void
DrawGray(const BlobsResult &blobsResult, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (const auto &blob : blobsResult.blobs) {
        if (!blob.flagged) continue;
        SetGrayPixels(blob.bounds, width, pixels);
    }
}

// draw blobs bounding box in gray pixels for regions
void
DrawGray(const std::vector<BlobsResult> &blobsResultVec, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (const auto &blobsResult : blobsResultVec) {
        if (!blobsResult.flagged) continue;
        for (const auto &blob : blobsResult.blobs) {
            if (!blob.flagged) continue;
            SetGrayPixels(blob.bounds, width, pixels);
        }
    }
}

// draw bounding box in rgb pixels for all/mask
void
DrawRgb(const BoundsResult &boundsResult, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    SetRgbPixels(boundsResult.bounds, width, pixDepth, pixels);
}

// draw bounding box in rgb pixels for regions
void
DrawRgb(const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    for (const auto &boundsResult : boundsResultVec) {
        if (!boundsResult.flagged) continue;
        SetRgbPixels(boundsResult.bounds, width, pixDepth, pixels);
    }
}

// draw blobs bounding box in rgb pixels for all/mask
void
DrawRgb(const BlobsResult &blobsResult, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    for (const auto &blob : blobsResult.blobs) {
        if (!blob.flagged) continue;
        SetRgbPixels(blob.bounds, width, pixDepth, pixels);
    }
}

// draw blobs bounding box in gray pixels for regions
void
DrawRgb(const std::vector<BlobsResult> &blobsResultVec, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    for (const auto &blobsResult : blobsResultVec) {
        if (!blobsResult.flagged) continue;
        for (const auto &blob : blobsResult.blobs) {
            if (!blob.flagged) continue;
            SetRgbPixels(blob.bounds, width, pixDepth, pixels);
        }
    }
}

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env /*&env*/, const uint_fast8_t *finalizeData) {
    delete[] finalizeData;
}