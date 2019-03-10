#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

// create js object and push to js array
inline void
SetPercentResult(const Napi::Env &env, const PercentResult &percentResult, Napi::Array &resultsJs, const uint_fast32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", percentResult.name);
    obj.Set("percent", percentResult.percent);
    resultsJs.Set(index, obj);
}

// create js object and push to js array
inline void
SetBoundsResult(const Napi::Env &env, const BoundsResult &boundsResult, Napi::Array &resultsJs, const uint_fast32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", boundsResult.name);
    obj.Set("percent", boundsResult.percent);
    obj.Set("minX", boundsResult.minX);
    obj.Set("maxX", boundsResult.maxX);
    obj.Set("minY", boundsResult.minY);
    obj.Set("maxY", boundsResult.maxY);
    resultsJs.Set(index, obj);
}

inline void
SetBlobsResult(const Napi::Env &env, const BlobsResult &blobsResult, Napi::Array &resultsJs, const uint_fast32_t index = 0) {
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
    resultsJs.Set(index, obj);
}

inline void
SetGrayPixels(const uint_fast32_t minX, const uint_fast32_t maxX, const uint_fast32_t minY, const uint_fast32_t maxY, const uint_fast32_t width, uint_fast8_t *pixels) {
    uint_fast8_t *firstPtr = &pixels[minY * width + minX];
    uint_fast8_t *secondPtr = &pixels[maxY * width + minX];
    for (uint_fast32_t x = minX; x <= maxX; ++x, ++firstPtr, ++secondPtr) {
        *firstPtr = 0x80;
        *secondPtr = 0x80;
    }
    firstPtr = &pixels[minY * width + minX + width];
    secondPtr = &pixels[minY * width + maxX + width];
    for (uint_fast32_t y = minY, yLimit = maxY - 1; y < yLimit; ++y, firstPtr += width, secondPtr += width) {
        *firstPtr = 0x80;
        *secondPtr = 0x80;
    }
}

inline void
SetRgbPixels(const uint_fast32_t minX, const uint_fast32_t maxX, const uint_fast32_t minY, const uint_fast32_t maxY, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    uint_fast32_t inc = 1;
    if (pixDepth > 3) {
        inc += (pixDepth - 3);
    }
    uint_fast8_t *firstPtr = &pixels[minY * width * pixDepth + minX * pixDepth];
    uint_fast8_t *secondPtr = &pixels[maxY * width * pixDepth + minX * pixDepth];
    for (uint_fast32_t x = minX; x <= maxX; ++x, firstPtr += inc, secondPtr += inc) {
        *firstPtr = 0xAF;
        *(++firstPtr) = 0x33;
        *(++firstPtr) = 0xFF;
        *secondPtr = 0xAF;
        *(++secondPtr) = 0x33;
        *(++secondPtr) = 0xFF;
    }
    inc = width * pixDepth;
    firstPtr = &pixels[minY * width * pixDepth + minX * pixDepth + width * pixDepth];
    secondPtr = &pixels[minY * width * pixDepth + maxX * pixDepth + width * pixDepth];
    for (uint_fast32_t y = minY, yLimit = maxY - 1; y < yLimit; ++y, firstPtr += inc, secondPtr += inc) {
        *firstPtr = 0xAF;
        *(firstPtr + 1) = 0x33;
        *(firstPtr + 2) = 0xFF;
        *secondPtr = 0xAF;
        *(secondPtr + 1) = 0x33;
        *(secondPtr + 2) = 0xFF;
    }
}

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

// regions blobs to js
void
ToJs(const Napi::Env &env, const std::vector<BlobsResult> &blobsResultVec, Napi::Array &resultsJs);

// draw bounding box in gray pixels for all/mask
void
DrawGray(const BoundsResult &boundsResult, uint_fast32_t width, uint_fast8_t *pixels);

// draw bounding box in gray pixels for regions
void
DrawGray(const std::vector<BoundsResult> &boundsResultVec, uint_fast32_t width, uint_fast8_t *pixels);

// draw blobs bounding box in gray pixels for all/mask
void
DrawGray(const BlobsResult &blobsResult, uint_fast32_t width, uint_fast8_t *pixels);

// draw blobs bounding box in gray pixels for regions
void
DrawGray(const std::vector<BlobsResult> &blobsResultVec, uint_fast32_t width, uint_fast8_t *pixels);

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