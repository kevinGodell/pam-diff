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

void
GrayAllPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.percentResult.flagged) {
        ToJs(env, results.percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
}

void
GrayRegionPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.percentResult.flagged) {
        ToJs(env, results.percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
}

// regions percent to js
void
ToJs(const Napi::Env &env, const std::vector<PercentResult> &percentResultVec, Napi::Array &resultsJs) {
    uint_fast32_t j = 0;
    for (const auto &percentResult : percentResultVec) {
        SetPercentResult(env, percentResult, resultsJs, j++);
    }
}

void
GrayRegionsPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!results.percentResults.empty()) {
        ToJs(env, results.percentResults, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
}

// all/mask bounds to js
void
ToJs(const Napi::Env &env, const BoundsResult &boundsResult, Napi::Array &resultsJs) {
    SetBoundsResult(env, boundsResult, resultsJs);
}

void
GrayAllBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.boundsResult.flagged) {
        ToJs(env, results.boundsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

void
GrayRegionBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.boundsResult.flagged) {
        ToJs(env, results.boundsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint_fast8_t> pixelBuffer = Napi::Buffer<uint_fast8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixelBuffer});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

// regions bounds to js
void
ToJs(const Napi::Env &env, const std::vector<BoundsResult> &boundsResultVec, Napi::Array &resultsJs) {
    uint_fast32_t j = 0;
    for (const auto &boundsResult : boundsResultVec) {
        SetBoundsResult(env, boundsResult, resultsJs, j++);
    }
}

void
GrayRegionsBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!results.boundsResults.empty()) {
        ToJs(env, results.boundsResults, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

// all/mask blobs to js
void
ToJs(const Napi::Env &env, const BlobsResult &blobsResult, Napi::Array &resultsJs) {
    SetBlobsResult(env, blobsResult, resultsJs);
}

void
GrayAllBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.blobsResult.flagged) {
        ToJs(env, results.blobsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

void
GrayRegionBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.blobsResult.flagged) {
        ToJs(env, results.blobsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

// regions blobs to js
void
ToJs(const Napi::Env &env, const std::vector<BlobsResult> &blobsResultVec, Napi::Array &resultsJs) {
    uint_fast32_t j = 0;
    for (const auto &blobsResult : blobsResultVec) {
        SetBlobsResult(env, blobsResult, resultsJs, j++);
    }
}

void
GrayRegionsBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!results.blobsResults.empty()) {
        ToJs(env, results.blobsResults, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

// draw bounding box in gray pixels for all/mask
void
DrawGray(const BoundsResult &boundsResult, const Config &config, uint_fast8_t *pixels) {
    SetGrayPixels(boundsResult.bounds, config, pixels);
}

// draw bounding box in gray pixels for regions
void
DrawGray(const std::vector<BoundsResult> &boundsResultVec, const Config &config, uint_fast8_t *pixels) {
    for (const auto &boundsResult : boundsResultVec) {
        SetGrayPixels(boundsResult.bounds, config, pixels);
    }
}

// draw blobs bounding box in gray pixels for all/mask
void
DrawGray(const BlobsResult &blobsResult, const Config &config, uint_fast8_t *pixels) {
    for (const auto &blob : blobsResult.blobs) {
        if (!blob.flagged) continue;
        SetGrayPixels(blob.bounds, config, pixels);
    }
}

// draw blobs bounding box in gray pixels for regions
void
DrawGray(const std::vector<BlobsResult> &blobsResultVec, const Config &config, uint_fast8_t *pixels) {
    for (const auto &blobsResult : blobsResultVec) {
        for (const auto &blob : blobsResult.blobs) {
            if (!blob.flagged) continue;
            SetGrayPixels(blob.bounds, config, pixels);
        }
    }
}

// draw bounding box in rgb pixels for all/mask
void
DrawRgb(const BoundsResult &boundsResult, const Config &config, uint_fast8_t *pixels) {
    SetRgbPixels(boundsResult.bounds, config, pixels);
}

// draw bounding box in rgb pixels for regions
void
DrawRgb(const std::vector<BoundsResult> &boundsResultVec, const Config &config, uint_fast8_t *pixels) {
    for (const auto &boundsResult : boundsResultVec) {
        SetRgbPixels(boundsResult.bounds, config, pixels);
    }
}

// draw blobs bounding box in rgb pixels for all/mask
void
DrawRgb(const BlobsResult &blobsResult, const Config &config, uint_fast8_t *pixels) {
    for (const auto &blob : blobsResult.blobs) {
        if (!blob.flagged) continue;
        SetRgbPixels(blob.bounds, config, pixels);
    }
}

// draw blobs bounding box in gray pixels for regions
void
DrawRgb(const std::vector<BlobsResult> &blobsResultVec, const Config &config, uint_fast8_t *pixels) {
    for (const auto &blobsResult : blobsResultVec) {
        for (const auto &blob : blobsResult.blobs) {
            if (!blob.flagged) continue;
            SetRgbPixels(blob.bounds, config, pixels);
        }
    }
}

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env /*&env*/, const uint_fast8_t *finalizeData) {
    delete[] finalizeData;
}