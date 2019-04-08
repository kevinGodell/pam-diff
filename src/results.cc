#include "results.h"
#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AllPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.percentResult.flagged) {
        SetPercentResult(env, results.percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RegionPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.percentResult.flagged) {
        SetPercentResult(env, results.percentResult, resultsJs);
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RegionsPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!results.percentResults.empty()) {
        uint32_t j = 0;
        for (const auto &percentResult : results.percentResults) {
            if (!percentResult.flagged) continue;
            SetPercentResult(env, percentResult, resultsJs, j++);
        }
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AllBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.boundsResult.flagged) {
        SetBoundsResult(env, results.boundsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint8_t> pixels = Napi::Buffer<uint8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RegionBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.boundsResult.flagged) {
        SetBoundsResult(env, results.boundsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint8_t> pixelBuffer = Napi::Buffer<uint8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixelBuffer});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RegionsBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!results.boundsResults.empty()) {// todo remove this check
        uint32_t j = 0;
        for (const auto &boundsResult : results.boundsResults) {
            if (!boundsResult.flagged) continue;
            SetBoundsResult(env, boundsResult, resultsJs, j++);
        }
        if (results.pixels.ptr) {
            const Napi::Buffer<uint8_t> pixels = Napi::Buffer<uint8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AllBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.blobsResult.flagged) {
        SetBlobsResult(env, results.blobsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint8_t> pixels = Napi::Buffer<uint8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RegionBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (results.blobsResult.flagged) {
        SetBlobsResult(env, results.blobsResult, resultsJs);
        if (results.pixels.ptr) {
            const Napi::Buffer<uint8_t> pixels = Napi::Buffer<uint8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RegionsBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results) {
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!results.blobsResults.empty()) {//todo change this since not using vector size to determine if has positive results
        uint32_t j = 0;
        for (const auto &blobsResult : results.blobsResults) {
            if (!blobsResult.flagged) continue;
            SetBlobsResult(env, blobsResult, resultsJs, j++);
        }
        if (results.pixels.ptr) {
            const Napi::Buffer<uint8_t> pixels = Napi::Buffer<uint8_t>::New(env, results.pixels.ptr, results.pixels.size, DeleteExternalData);
            cb.Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    cb.Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env /*&env*/, const uint8_t *finalizeData) {
    delete[] finalizeData;
}