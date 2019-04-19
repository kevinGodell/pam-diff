#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#ifndef GRAY
#define GRAY 0xF0;
#endif

#ifndef RED
#define RED 0xFF;
#endif

#ifndef GREEN
#define GREEN 0xFF;
#endif

#ifndef BLUE
#define BLUE 0x00;
#endif

#include "engine.h"
#include "napi.h"
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// create js object and push to js array
inline void
SetPercentResult(const Napi::Env &env, const Result &result, Napi::Array &resultsJs, const uint32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", result.name);
    obj.Set("percent", result.percent);
    resultsJs.Set(index, obj);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// create js object and push to js array
inline void
SetBoundsResult(const Napi::Env &env, const Result &result, Napi::Array &resultsJs, const uint32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", result.name);
    obj.Set("percent", result.percent);
    obj.Set("minX", result.bounds.minX);
    obj.Set("maxX", result.bounds.maxX);
    obj.Set("minY", result.bounds.minY);
    obj.Set("maxY", result.bounds.maxY);
    resultsJs.Set(index, obj);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// create js object and push to js array
inline void
SetBlobsResult(const Napi::Env &env, const Result &result, Napi::Array &resultsJs, const uint32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", result.name);
    obj.Set("percent", result.percent);
    obj.Set("minX", result.bounds.minX);
    obj.Set("maxX", result.bounds.maxX);
    obj.Set("minY", result.bounds.minY);
    obj.Set("maxY", result.bounds.maxY);
    Napi::Array blobsJs = Napi::Array::New(env);
    uint32_t j = 0;
    for (const auto &blob : result.blobs) {
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// draw bounding box in gray pixels
inline void
SetGrayPixels(const Bounds &bounds, const Config &config, uint8_t *pixels) {
    uint32_t i = bounds.minY * config.width + bounds.minX;
    uint32_t j = bounds.maxY * config.width + bounds.minX;
    for (uint32_t x = bounds.minX; x <= bounds.maxX; ++x, ++i, ++j) {
        pixels[i] = GRAY;// top
        pixels[j] = GRAY;// bottom
    }
    i = (bounds.minY + 1) * config.width + bounds.minX;
    j = (bounds.minY + 1) * config.width + bounds.maxX;
    for (uint32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, i += config.width, j += config.width) {
        pixels[i] = GRAY;// left
        pixels[j] = GRAY;// right
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// draw bounding box in rgb(a) pixels
inline void
SetRgbPixels(const Bounds &bounds, const Config &config, uint8_t *pixels) {
    uint32_t inc = config.depth;
    uint32_t i = bounds.minY * config.width * config.depth + bounds.minX * config.depth;
    uint32_t j = bounds.maxY * config.width * config.depth + bounds.minX * config.depth;
    for (uint32_t x = bounds.minX; x <= bounds.maxX; ++x, i += inc, j += inc) {
        pixels[i] = RED;// top
        pixels[i + 1] = GREEN;
        pixels[i + 2] = BLUE;
        pixels[j] = RED;// bottom
        pixels[j + 1] = GREEN;
        pixels[j + 2] = BLUE;
    }
    inc = config.width * config.depth;
    i = (bounds.minY + 1) * config.width * config.depth + bounds.minX * config.depth;
    j = (bounds.minY + 1) * config.width * config.depth + bounds.maxX * config.depth;
    for (uint32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, i += inc, j += inc) {
        pixels[i] = RED;// left
        pixels[i + 1] = GREEN;
        pixels[i + 2] = BLUE;
        pixels[j] = RED;// right
        pixels[j + 1] = GREEN;
        pixels[j + 2] = BLUE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PercentCallback(const Napi::Env &env, const Napi::Function &cb, CallbackData &callbackData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
BoundsCallback(const Napi::Env &env, const Napi::Function &cb, CallbackData &callbackData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlobsCallback(const Napi::Env &env, const Napi::Function &cb, CallbackData &callbackData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env env, const uint8_t *finalizeData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif