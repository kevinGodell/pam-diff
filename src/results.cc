#include "results.h"
#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

// all/mask percent to js
Napi::Array
ToJs(const Napi::Env &env, const std::string &name, const PercentResult &percentResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (percentResult.flagged) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("percent", percentResult.percent);
        resultsJs.Set(0u, obj);
    }
    return resultsJs;
}

// regions percent to js
Napi::Array
ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const std::vector<PercentResult> &percentResultVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t r = 0, j = 0; r < regionsLen; ++r) {
        if (!percentResultVec[r].flagged) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", regionVec[r].name);
        obj.Set("percent", percentResultVec[r].percent);
        resultsJs.Set(j++, obj);
    }
    return resultsJs;
}

// all/mask bounds to js
Napi::Array
ToJs(const Napi::Env &env, const std::string &name, const BoundsResult &boundsResult) {
    Napi::Array resultsJs = Napi::Array::New(env);
    if (boundsResult.flagged) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("percent", boundsResult.percent);
        obj.Set("minX", boundsResult.minX);
        obj.Set("maxX", boundsResult.maxX);
        obj.Set("minY", boundsResult.minY);
        obj.Set("maxY", boundsResult.maxY);
        resultsJs.Set(0u, obj);
    }
    return resultsJs;
}

// regions bounds to js
Napi::Array
ToJs(const Napi::Env &env, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const std::vector<BoundsResult> &boundsResultVec) {
    Napi::Array resultsJs = Napi::Array::New(env);
    for (uint_fast32_t r = 0, j = 0; r < regionsLen; ++r) {
        if (!boundsResultVec[r].flagged) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", regionVec[r].name);
        obj.Set("percent", boundsResultVec[r].percent);
        obj.Set("minX", boundsResultVec[r].minX);
        obj.Set("maxX", boundsResultVec[r].maxX);
        obj.Set("minY", boundsResultVec[r].minY);
        obj.Set("maxY", boundsResultVec[r].maxY);
        resultsJs.Set(j++, obj);
    }
    return resultsJs;
}

// draw bounding box in gray pixels
void
DrawGrayBounds(const Napi::Array &resultsJs, const uint_fast32_t width, uint_fast8_t *pixels) {
    for (uint_fast32_t i = 0; i < resultsJs.Length(); ++i) {
        Napi::Object obj = resultsJs.Get(i).As<Napi::Object>();
        uint_fast32_t minX = obj.Get("minX").As<Napi::Number>().Uint32Value();
        uint_fast32_t maxX = obj.Get("maxX").As<Napi::Number>().Uint32Value();
        uint_fast32_t minY = obj.Get("minY").As<Napi::Number>().Uint32Value();
        uint_fast32_t maxY = obj.Get("maxY").As<Napi::Number>().Uint32Value();
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

// draw bounding box in rgb pixels
void
DrawRgbBounds(const Napi::Array &resultsJs, const uint_fast32_t width, const uint_fast32_t pixDepth, uint_fast8_t *pixels) {
    for (uint_fast32_t i = 0; i < resultsJs.Length(); ++i) {
        Napi::Object obj = resultsJs.Get(i).As<Napi::Object>();
        uint_fast32_t minX = obj.Get("minX").As<Napi::Number>().Uint32Value();
        uint_fast32_t maxX = obj.Get("maxX").As<Napi::Number>().Uint32Value();
        uint_fast32_t minY = obj.Get("minY").As<Napi::Number>().Uint32Value();
        uint_fast32_t maxY = obj.Get("maxY").As<Napi::Number>().Uint32Value();
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