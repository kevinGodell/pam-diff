#include "results.h"
#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

// all/mask percent to js
void
ToJs(const Napi::Env &env, const PercentResult &percentResult, Napi::Array &resultsJs) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", percentResult.name);
    obj.Set("percent", percentResult.percent);
    resultsJs.Set(0u, obj);
}

// regions percent to js
void
ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<PercentResult> &percentResultVec, Napi::Array &resultsJs) {
    for (uint_fast32_t r = 0, j = 0; r < regionsLen; ++r) {
        if (!percentResultVec[r].flagged) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", percentResultVec[r].name);
        obj.Set("percent", percentResultVec[r].percent);
        resultsJs.Set(j++, obj);
    }
}

// all/mask bounds to js
void
ToJs(const Napi::Env &env, const BoundsResult &boundsResult, Napi::Array &resultsJs) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", boundsResult.name);
    obj.Set("percent", boundsResult.percent);
    obj.Set("minX", boundsResult.minX);
    obj.Set("maxX", boundsResult.maxX);
    obj.Set("minY", boundsResult.minY);
    obj.Set("maxY", boundsResult.maxY);
    resultsJs.Set(0u, obj);
}

// regions bounds to js
void
ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, Napi::Array &resultsJs) {
    for (uint_fast32_t r = 0, j = 0; r < regionsLen; ++r) {
        if (!boundsResultVec[r].flagged) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", boundsResultVec[r].name);
        obj.Set("percent", boundsResultVec[r].percent);
        obj.Set("minX", boundsResultVec[r].minX);
        obj.Set("maxX", boundsResultVec[r].maxX);
        obj.Set("minY", boundsResultVec[r].minY);
        obj.Set("maxY", boundsResultVec[r].maxY);
        resultsJs.Set(j++, obj);
    }
}

// draw bounding box in gray pixels for all/mask
void
DrawGrayBounds(const BoundsResult &boundsResult, const uint_fast32_t width, uint_fast8_t *pixels) {
    uint_fast32_t indexMinY = boundsResult.minY * width;
    uint_fast32_t indexMaxY = boundsResult.maxY * width;
    for (uint_fast32_t x = boundsResult.minX; x < boundsResult.maxX; ++x) {
        pixels[indexMinY + x] = 0x00;
        pixels[indexMaxY + x] = 0x00;
    }
    for (uint_fast32_t y = boundsResult.minY; y < boundsResult.maxY; ++y) {
        uint_fast32_t indexY = y * width;
        pixels[indexY + boundsResult.minX] = 0x00;
        pixels[indexY + boundsResult.maxX] = 0x00;
    }
}

// draw bounding box in gray pixels for regions
void
DrawGrayBounds(const uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (uint_fast32_t i = 0; i < regionsLen; ++i) {
        if (!boundsResultVec[i].flagged) continue;
        uint_fast32_t indexMinY = boundsResultVec[i].minY * width;
        uint_fast32_t indexMaxY = boundsResultVec[i].maxY * width;
        for (uint_fast32_t x = boundsResultVec[i].minX; x < boundsResultVec[i].maxX; ++x) {
            pixels[indexMinY + x] = 0x00;
            pixels[indexMaxY + x] = 0x00;
        }
        for (uint_fast32_t y = boundsResultVec[i].minY; y < boundsResultVec[i].maxY; ++y) {
            uint_fast32_t indexY = y * width;
            pixels[indexY + boundsResultVec[i].minX] = 0x00;
            pixels[indexY + boundsResultVec[i].maxX] = 0x00;
        }
    }
}

// draw bounding box in rgb pixels for all/mask
void
DrawRgbBounds(const BoundsResult &boundsResult, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    uint_fast32_t indexMinY = boundsResult.minY * width;
    uint_fast32_t indexMaxY = boundsResult.maxY * width;
    for (uint_fast32_t x = boundsResult.minX; x < boundsResult.maxX; ++x) {
        pixels[(indexMinY + x) * pixDepth] = 0x00;
        pixels[(indexMaxY + x) * pixDepth] = 0x00;
    }
    for (uint_fast32_t y = boundsResult.minY; y < boundsResult.maxY; ++y) {
        uint_fast32_t indexY = y * width;
        pixels[(indexY + boundsResult.minX) * pixDepth] = 0x00;
        pixels[(indexY + boundsResult.maxX) * pixDepth] = 0x00;
    }
}

// draw bounding box in rgb pixels for regions
void
DrawRgbBounds(const uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    for (uint_fast32_t i = 0; i < regionsLen; ++i) {
        if (!boundsResultVec[i].flagged) continue;
        uint_fast32_t indexMinY = boundsResultVec[i].minY * width;
        uint_fast32_t indexMaxY = boundsResultVec[i].maxY * width;
        for (uint_fast32_t x = boundsResultVec[i].minX; x < boundsResultVec[i].maxX; ++x) {
            pixels[(indexMinY + x) * pixDepth] = 0x00;
            pixels[(indexMaxY + x) * pixDepth] = 0x00;
        }
        for (uint_fast32_t y = boundsResultVec[i].minY; y < boundsResultVec[i].maxY; ++y) {
            uint_fast32_t indexY = y * width;
            pixels[(indexY + boundsResultVec[i].minX) * pixDepth] = 0x00;
            pixels[(indexY + boundsResultVec[i].maxX) * pixDepth] = 0x00;
        }
    }
}

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env /*&env*/, const uint_fast8_t *finalizeData) {
    delete[] finalizeData;
}

// all/mask bounds to js
void
ToJs(const Napi::Env &env, const BlobsResult &blobsResult, Napi::Array &resultsJs) {

    Napi::Object obj = Napi::Object::New(env);

    obj.Set("name", blobsResult.name);
    obj.Set("percent", blobsResult.percent);
    obj.Set("minX", blobsResult.minX);
    obj.Set("maxX", blobsResult.maxX);
    obj.Set("minY", blobsResult.minY);
    obj.Set("maxY", blobsResult.maxY);

    Napi::Array blobs = Napi::Array::New(env);

    for (uint_fast32_t b = 0, j = 0; b < blobsResult.blobs.size(); ++b) {
        if (!blobsResult.blobs[b].flagged) continue;
        Napi::Object blob = Napi::Object::New(env);
        blob.Set("label", blobsResult.blobs[b].label);
        blob.Set("percent", blobsResult.blobs[b].percent);
        blob.Set("minX", blobsResult.blobs[b].minX);
        blob.Set("maxX", blobsResult.blobs[b].maxX);
        blob.Set("minY", blobsResult.blobs[b].minY);
        blob.Set("maxY", blobsResult.blobs[b].maxY);
        blobs.Set(j++, blob);
    }

    obj.Set("blobs", blobs);
    resultsJs.Set(0u, obj);
}

// draw bounding box in gray pixels for all/mask
void
DrawGrayBounds(const BlobsResult &blobsResult, const uint_fast32_t width, uint_fast8_t *pixels) {

    for (uint_fast32_t i = 0; i < blobsResult.blobs.size(); ++i) {
        if (!blobsResult.blobs[i].flagged) continue;
        uint_fast32_t indexMinY = blobsResult.blobs[i].minY * width;
        uint_fast32_t indexMaxY = blobsResult.blobs[i].maxY * width;
        for (uint_fast32_t x = blobsResult.blobs[i].minX; x < blobsResult.blobs[i].maxX; ++x) {
            pixels[indexMinY + x] = 0x00;
            pixels[indexMaxY + x] = 0x00;
        }
        for (uint_fast32_t y = blobsResult.blobs[i].minY; y < blobsResult.blobs[i].maxY; ++y) {
            uint_fast32_t indexY = y * width;
            pixels[indexY + blobsResult.blobs[i].minX] = 0x00;
            pixels[indexY + blobsResult.blobs[i].maxX] = 0x00;
        }
    }
}