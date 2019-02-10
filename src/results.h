#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

/*
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
*/

// all/mask percent to js
inline void
ToJs(const Napi::Env &env, const PercentResult &percentResult, Napi::Array &resultsJs) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", percentResult.name);
    obj.Set("percent", percentResult.percent);
    resultsJs.Set(0u, obj);
}

// regions percent to js
inline void
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
inline void
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
inline void
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
inline void
DrawGrayBounds(const BoundsResult &boundsResult, const uint_fast32_t width, uint_fast8_t *pixels) {
    uint_fast32_t minX = boundsResult.minX;
    uint_fast32_t maxX = boundsResult.maxX;
    uint_fast32_t minY = boundsResult.minY;
    uint_fast32_t maxY = boundsResult.maxY;
    uint_fast32_t indexMinY = minY * width;
    uint_fast32_t indexMaxY = maxY * width;
    for (uint_fast32_t x = minX; x < maxX; ++x) {
        pixels[indexMinY + x] = 0x00;
        pixels[indexMaxY + x] = 0x00;
    }
    for (uint_fast32_t y = minY; y < maxY; ++y) {
        uint_fast32_t indexY = y * width;
        pixels[indexY + minX] = 0x00;
        pixels[indexY + maxX] = 0x00;
    }
}

// draw bounding box in gray pixels for regions
inline void
DrawGrayBounds(const uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (uint_fast32_t i = 0; i < regionsLen; ++i) {
        if (!boundsResultVec[i].flagged) continue;
        uint_fast32_t minX = boundsResultVec[i].minX;
        uint_fast32_t maxX = boundsResultVec[i].maxX;
        uint_fast32_t minY = boundsResultVec[i].minY;
        uint_fast32_t maxY = boundsResultVec[i].maxY;
        uint_fast32_t indexMinY = minY * width;
        uint_fast32_t indexMaxY = maxY * width;
        for (uint_fast32_t x = minX; x < maxX; ++x) {
            pixels[indexMinY + x] = 0x00;
            pixels[indexMaxY + x] = 0x00;
        }
        for (uint_fast32_t y = minY; y < maxY; ++y) {
            uint_fast32_t indexY = y * width;
            pixels[indexY + minX] = 0x00;
            pixels[indexY + maxX] = 0x00;
        }
    }
}

// draw bounding box in rgb pixels for all/mask
inline void
DrawRgbBounds(const BoundsResult &boundsResult, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    uint_fast32_t minX = boundsResult.minX;
    uint_fast32_t maxX = boundsResult.maxX;
    uint_fast32_t minY = boundsResult.minY;
    uint_fast32_t maxY = boundsResult.maxY;
    uint_fast32_t indexMinY = minY * width;
    uint_fast32_t indexMaxY = maxY * width;
    for (uint_fast32_t x = minX; x < maxX; ++x) {
        pixels[(indexMinY + x) * pixDepth] = 0x00;
        pixels[(indexMaxY + x) * pixDepth] = 0x00;
    }
    for (uint_fast32_t y = minY; y < maxY; ++y) {
        uint_fast32_t indexY = y * width;
        pixels[(indexY + minX) * pixDepth] = 0x00;
        pixels[(indexY + maxX) * pixDepth] = 0x00;
    }
}

// draw bounding box in rgb pixels for regions
inline void
DrawRgbBounds(const uint_fast32_t regionsLen, const std::vector<BoundsResult> &boundsResultVec, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    for (uint_fast32_t i = 0; i < regionsLen; ++i) {
        if (!boundsResultVec[i].flagged) continue;
        uint_fast32_t minX = boundsResultVec[i].minX;
        uint_fast32_t maxX = boundsResultVec[i].maxX;
        uint_fast32_t minY = boundsResultVec[i].minY;
        uint_fast32_t maxY = boundsResultVec[i].maxY;
        uint_fast32_t indexMinY = minY * width;
        uint_fast32_t indexMaxY = maxY * width;
        for (uint_fast32_t x = minX; x < maxX; ++x) {
            pixels[(indexMinY + x) * pixDepth] = 0x00;
            pixels[(indexMaxY + x) * pixDepth] = 0x00;
        }
        for (uint_fast32_t y = minY; y < maxY; ++y) {
            uint_fast32_t indexY = y * width;
            pixels[(indexY + minX) * pixDepth] = 0x00;
            pixels[(indexY + maxX) * pixDepth] = 0x00;
        }
    }
}

#endif