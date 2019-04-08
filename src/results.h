#ifndef SRC_RESULTS_H_
#define SRC_RESULTS_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// create js object and push to js array
inline void
SetPercentResult(const Napi::Env &env, const PercentResult &percentResult, Napi::Array &resultsJs, const uint32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", percentResult.name);
    obj.Set("percent", percentResult.percent);
    resultsJs.Set(index, obj);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// create js object and push to js array
inline void
SetBoundsResult(const Napi::Env &env, const BoundsResult &boundsResult, Napi::Array &resultsJs, const uint32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", boundsResult.name);
    obj.Set("percent", boundsResult.percent);
    obj.Set("minX", boundsResult.bounds.minX);
    obj.Set("maxX", boundsResult.bounds.maxX);
    obj.Set("minY", boundsResult.bounds.minY);
    obj.Set("maxY", boundsResult.bounds.maxY);
    resultsJs.Set(index, obj);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void
SetBlobsResult(const Napi::Env &env, const BlobsResult &blobsResult, Napi::Array &resultsJs, const uint32_t index = 0) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", blobsResult.name);
    obj.Set("percent", blobsResult.percent);
    obj.Set("minX", blobsResult.bounds.minX);
    obj.Set("maxX", blobsResult.bounds.maxX);
    obj.Set("minY", blobsResult.bounds.minY);
    obj.Set("maxY", blobsResult.bounds.maxY);
    Napi::Array blobsJs = Napi::Array::New(env);
    uint32_t j = 0;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void
SetGrayPixels(const Bounds &bounds, const Config &config, uint8_t *pixels) {
    uint32_t i = bounds.minY * config.width + bounds.minX;
    uint32_t j = bounds.maxY * config.width + bounds.minX;
    for (uint32_t x = bounds.minX; x <= bounds.maxX; ++x, ++i, ++j) {
        pixels[i] = 0xF0;// top
        pixels[j] = 0xF0;// bottom
    }
    i = (bounds.minY + 1) * config.width + bounds.minX;
    j = (bounds.minY + 1) * config.width + bounds.maxX;
    for (uint32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, i += config.width, j += config.width) {
        pixels[i] = 0xF0;// left
        pixels[j] = 0xF0;// right
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void
SetRgbPixels(const Bounds &bounds, const Config &config, uint8_t *pixels) {
    uint32_t inc = config.depth;
    uint32_t i = bounds.minY * config.width * config.depth + bounds.minX * config.depth;
    uint32_t j = bounds.maxY * config.width * config.depth + bounds.minX * config.depth;
    for (uint32_t x = bounds.minX; x <= bounds.maxX; ++x, i += inc, j += inc) {
        pixels[i] = 0xFF;// top
        pixels[i + 1] = 0xFF;
        pixels[i + 2] = 0x00;
        pixels[j] = 0xFF;// bottom
        pixels[j + 1] = 0xFF;
        pixels[j + 2] = 0x00;
    }
    inc = config.width * config.depth;
    i = (bounds.minY + 1) * config.width * config.depth + bounds.minX * config.depth;
    j = (bounds.minY + 1) * config.width * config.depth + bounds.maxX * config.depth;
    for (uint32_t y = bounds.minY, yLimit = bounds.maxY - 1; y < yLimit; ++y, i += inc, j += inc) {
        pixels[i] = 0xFF;// left
        pixels[i + 1] = 0xFF;
        pixels[i + 2] = 0x00;
        pixels[j] = 0xFF;// right
        pixels[j + 1] = 0xFF;
        pixels[j + 2] = 0x00;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AllPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

void
RegionPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

void
RegionsPercentCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AllBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

void
RegionBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

void
RegionsBoundsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AllBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

void
RegionBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

void
RegionsBlobsCallback(const Napi::Env &env, const Napi::Function &cb, const Results &results);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// free memory from heap allocated array used as Buffer data
void
DeleteExternalData(Napi::Env env, const uint8_t *finalizeData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif