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

// draw bounding box in gray pixels for all/mask
void
DrawGray(const BoundsResult &boundsResult, const uint_fast32_t width, uint_fast8_t *pixels) {
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
DrawGray(const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (const auto &boundsResult : boundsResultVec) {
        if (!boundsResult.flagged) continue;
        uint_fast32_t indexMinY = boundsResult.minY * width;
        uint_fast32_t indexMaxY = boundsResult.maxY * width;
        uint_fast8_t *topPtr = &pixels[indexMinY + boundsResult.minX];
        uint_fast8_t *bottomPtr = &pixels[indexMaxY + boundsResult.minX];
        uint_fast8_t *leftPtr = &pixels[indexMinY + boundsResult.minX + width];
        uint_fast8_t *rightPtr = &pixels[indexMinY + boundsResult.maxX + width];
        for (uint_fast32_t x = boundsResult.minX; x <= boundsResult.maxX; ++x, ++topPtr, ++bottomPtr) {
            //pixels[indexMinY + x] = 0x00;
            //pixels[indexMaxY + x] = 0x00;
            *topPtr = 0x80;
            *bottomPtr = 0x80;
        }
        for (uint_fast32_t y = boundsResult.minY, yLimit = boundsResult.maxY - 1; y < yLimit; ++y, leftPtr += width, rightPtr += width) {
            //uint_fast32_t indexY = y * width;
            //pixels[indexY + boundsResult.minX] = 0x00;
            //pixels[indexY + boundsResult.maxX] = 0x00;
            *leftPtr = 0x80;
            *rightPtr = 0x80;
        }
    }
}

// draw bounding box in rgb pixels for all/mask
void
DrawRgb(const BoundsResult &boundsResult, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
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
DrawRgb(const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    for (const auto &boundsResult : boundsResultVec) {
        if (!boundsResult.flagged) continue;
        uint_fast32_t indexMinY = boundsResult.minY * width;
        uint_fast32_t indexMaxY = boundsResult.maxY * width;
        for (uint_fast32_t x = boundsResult.minX; x < boundsResult.maxX; ++x) {

            /*uint_fast8_t *ptr = &pixels[(indexMinY + x) * pixDepth];
            *ptr = 0xFF;
            *(++ptr) = 0x00;
            *(++ptr) = 0x00;

            ptr = &pixels[(indexMaxY + x) * pixDepth];
            *ptr = 0xFF;
            *(++ptr) = 0x00;
            *(++ptr) = 0x00;*/

            pixels[(indexMinY + x) * pixDepth] = 0x00;
            pixels[(indexMaxY + x) * pixDepth] = 0x00;
        }
        for (uint_fast32_t y = boundsResult.minY; y < boundsResult.maxY; ++y) {
            uint_fast32_t indexY = y * width;

            /*uint_fast8_t *ptr = &pixels[(indexY + boundsResult.minX) * pixDepth];
            *ptr = 0xFF;
            *(++ptr) = 0x00;
            *(++ptr) = 0x00;

            ptr = &pixels[(indexY + boundsResult.maxX) * pixDepth];
            *ptr = 0xFF;
            *(++ptr) = 0x00;
            *(++ptr) = 0x00;*/


            pixels[(indexY + boundsResult.minX) * pixDepth] = 0x00;
            pixels[(indexY + boundsResult.maxX) * pixDepth] = 0x00;
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
    Napi::Array blobsJs = Napi::Array::New(env);
    uint_fast32_t j = 0;
    for (const auto &blob : blobsResult.blobs) {
        if (!blob.flagged) continue;
        Napi::Object blobJs = Napi::Object::New(env);
        blobJs.Set("label", blob.label);
        blobJs.Set("percent", blob.percent);
        blobJs.Set("minX", blob.minX);
        blobJs.Set("maxX", blob.maxX);
        blobJs.Set("minY", blob.minY);
        blobJs.Set("maxY", blob.maxY);
        blobsJs.Set(j++, blobJs);
    }
    obj.Set("blobs", blobsJs);
    resultsJs.Set(0u, obj);
}

// draw blobs bounding box in gray pixels for all/mask
void
DrawGray(const BlobsResult &blobsResult, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (const auto &blob : blobsResult.blobs) {
        if (!blob.flagged) continue;
        uint_fast32_t indexMinY = blob.minY * width;
        uint_fast32_t indexMaxY = blob.maxY * width;
        for (uint_fast32_t x = blob.minX; x < blob.maxX; ++x) {
            pixels[indexMinY + x] = 0x00;
            pixels[indexMaxY + x] = 0x00;
        }
        for (uint_fast32_t y = blob.minY; y < blob.maxY; ++y) {
            uint_fast32_t indexY = y * width;
            pixels[indexY + blob.minX] = 0x00;
            pixels[indexY + blob.maxX] = 0x00;
        }
    }
}