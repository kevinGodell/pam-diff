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
    obj.Set("minX", boundsResult.bounds.minX);
    obj.Set("maxX", boundsResult.bounds.maxX);
    obj.Set("minY", boundsResult.bounds.minY);
    obj.Set("maxY", boundsResult.bounds.maxY);
    resultsJs.Set(index, obj);
}

inline void
SetBlobsResult(const Napi::Env &env, const BlobsResult &blobsResult, Napi::Array &resultsJs, const uint_fast32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", blobsResult.name);
    obj.Set("percent", blobsResult.percent);
    obj.Set("minX", blobsResult.bounds.minX);
    obj.Set("maxX", blobsResult.bounds.maxX);
    obj.Set("minY", blobsResult.bounds.minY);
    obj.Set("maxY", blobsResult.bounds.maxY);
    Napi::Array blobsJs = Napi::Array::New(env);
    uint_fast32_t j = 0;
    for (const auto &blob : blobsResult.blobs) {
        if (!blob.flagged) continue;
        Napi::Object blobJs = Napi::Object::New(env);
        blobJs.Set("label", blob.label);
        blobJs.Set("percent", blob.percent);
        blobJs.Set("minX", blob.bounds.minX);
        blobJs.Set("maxX", blob.bounds.maxX);
        blobJs.Set("minY", blob.bounds.minY);
        blobJs.Set("maxY", blob.bounds.maxY);
        blobsJs.Set(j++, blobJs);
    }
    obj.Set("blobs", blobsJs);
    resultsJs.Set(index, obj);
}

/*inline void
SetGrayPixels(const uint_fast32_t minX, const uint_fast32_t maxX, const uint_fast32_t minY, const uint_fast32_t maxY, const uint_fast32_t width, uint_fast8_t *pixels) {
    uint_fast8_t *firstPtr = &pixels[minY * width + minX];
    uint_fast8_t *secondPtr = &pixels[maxY * width + minX];
    for (uint_fast32_t x = minX; x <= maxX; ++x, ++firstPtr, ++secondPtr) {
        *firstPtr = 0x80;// top
        *secondPtr = 0x80;// bottom
    }
    firstPtr = &pixels[(minY + 1) * width + minX];
    secondPtr = &pixels[(minY + 1) * width + maxX];
    for (uint_fast32_t y = minY, yLimit = maxY - 1; y < yLimit; ++y, firstPtr += width, secondPtr += width) {
        *firstPtr = 0x80;// left
        *secondPtr = 0x80;// right
    }
}*/

/*inline void
SetGrayPixels(const Bounds &bounds, const Dimensions &dimensions, uint_fast8_t *pixels) {
    uint_fast8_t *firstPtr = &pixels[bounds.minY * dimensions.width + bounds.minX];
    uint_fast8_t *secondPtr = &pixels[bounds.maxY * dimensions.width + bounds.minX];
    for (uint_fast32_t x = bounds.minX; x <= bounds.maxX; ++x, ++firstPtr, ++secondPtr) {
        *firstPtr = 0x80;// top
        *secondPtr = 0x80;// bottom
    }
    firstPtr = &pixels[(bounds.minY + 1) * dimensions.width + bounds.minX];
    secondPtr = &pixels[(bounds.minY + 1) * dimensions.width + bounds.maxX];
    for (uint_fast32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, firstPtr += dimensions.width, secondPtr += dimensions.width) {
        *firstPtr = 0x80;// left
        *secondPtr = 0x80;// right
    }
}*/

inline void
SetGrayPixels(const Bounds &bounds, const Dimensions &dimensions, uint_fast8_t *pixels) {
    uint_fast32_t i = bounds.minY * dimensions.width + bounds.minX;
    uint_fast32_t j = bounds.maxY * dimensions.width + bounds.minX;
    for (uint_fast32_t x = bounds.minX; x <= bounds.maxX; ++x, ++i, ++j) {
        pixels[i] = 0x80;// top
        pixels[j] = 0x80;// bottom
    }
    i = (bounds.minY + 1) * dimensions.width + bounds.minX;
    j = (bounds.minY + 1) * dimensions.width + bounds.maxX;
    for (uint_fast32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, i += dimensions.width, j += dimensions.width) {
        pixels[i] = 0x80;// left
        pixels[j] = 0x80;// right
    }
}

/*inline void
SetRgbPixels(const Bounds &bounds, const Dimensions &dimensions, uint_fast8_t *pixels) {
    uint_fast32_t inc = dimensions.depth == 4 ? 2 : dimensions.depth == 3 ? 1 : 0;
    uint_fast8_t *firstPtr = &pixels[bounds.minY * dimensions.width * dimensions.depth + bounds.minX * dimensions.depth];
    uint_fast8_t *secondPtr = &pixels[bounds.maxY * dimensions.width * dimensions.depth + bounds.minX * dimensions.depth];
    for (uint_fast32_t x = bounds.minX; x <= bounds.maxX; ++x, firstPtr += inc, secondPtr += inc) {
        *firstPtr = 0xAF;// top
        *(++firstPtr) = 0x33;
        *(++firstPtr) = 0xFF;
        *secondPtr = 0xAF;// bottom
        *(++secondPtr) = 0x33;
        *(++secondPtr) = 0xFF;
    }
    inc = dimensions.width * dimensions.depth;
    firstPtr = &pixels[(bounds.minY + 1) * dimensions.width * dimensions.depth + bounds.minX * dimensions.depth];
    secondPtr = &pixels[(bounds.minY + 1) * dimensions.width * dimensions.depth + bounds.maxX * dimensions.depth];
    for (uint_fast32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, firstPtr += inc, secondPtr += inc) {
        *firstPtr = 0xAF;// left
        *(firstPtr + 1) = 0x33;
        *(firstPtr + 2) = 0xFF;
        *secondPtr = 0xAF;// right
        *(secondPtr + 1) = 0x33;
        *(secondPtr + 2) = 0xFF;
    }
}*/

inline void
SetRgbPixels(const Bounds &bounds, const Dimensions &dimensions, uint_fast8_t *pixels) {
    uint_fast32_t inc = dimensions.depth;
    uint_fast32_t i = bounds.minY * dimensions.width * dimensions.depth + bounds.minX * dimensions.depth;
    uint_fast32_t j = bounds.maxY * dimensions.width * dimensions.depth + bounds.minX * dimensions.depth;
    for (uint_fast32_t x = bounds.minX; x <= bounds.maxX; ++x, i += inc, j += inc) {
        pixels[i] = 0xAF;// top
        pixels[i + 1] = 0x33;
        pixels[i + 2] = 0xFF;
        pixels[j] = 0xAF;// bottom
        pixels[j + 1] = 0x33;
        pixels[j + 2] = 0xFF;
    }
    inc = dimensions.width * dimensions.depth;
    i = (bounds.minY + 1) * dimensions.width * dimensions.depth + bounds.minX * dimensions.depth;
    j = (bounds.minY + 1) * dimensions.width * dimensions.depth + bounds.maxX * dimensions.depth;
    for (uint_fast32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, i += inc, j += inc) {
        pixels[i] = 0xAF;// left
        pixels[i + 1] = 0x33;
        pixels[i + 2] = 0xFF;
        pixels[j] = 0xAF;// right
        pixels[j+ 1] = 0x33;
        pixels[j + 2] = 0xFF;
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
DrawGray(const BoundsResult &boundsResult, const Dimensions &dimensions, uint_fast8_t *pixels);

// draw bounding box in gray pixels for regions
void
DrawGray(const std::vector<BoundsResult> &boundsResultVec, const Dimensions &dimensions, uint_fast8_t *pixels);

// draw blobs bounding box in gray pixels for all/mask
void
DrawGray(const BlobsResult &blobsResult, const Dimensions &dimensions, uint_fast8_t *pixels);

// draw blobs bounding box in gray pixels for regions
void
DrawGray(const std::vector<BlobsResult> &blobsResultVec, const Dimensions &dimensions, uint_fast8_t *pixels);

// draw bounding box in rgb pixels for all/mask
void
DrawRgb(const BoundsResult &boundsResult, const Dimensions &dimensions, uint_fast8_t *pixels);

// draw bounding box in rgb pixels for regions
void
DrawRgb(const std::vector<BoundsResult> &boundsResultVec, const Dimensions &dimensions, uint_fast8_t *pixels);

// draw blobs bounding box in rgb pixels for all/mask
void
DrawRgb(const BlobsResult &blobsResult, const Dimensions &dimensions, uint_fast8_t *pixels);

// draw blobs bounding box in rgb pixels for regions
void
DrawRgb(const std::vector<BlobsResult> &blobsResultVec, const Dimensions &dimensions, uint_fast8_t *pixels);

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env env, const uint_fast8_t *finalizeData);

#endif