#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

enum Engines {// ordered by depth -> target -> response
    GRAY_ALL_PERCENT = 0,
    GRAY_ALL_BOUNDS = 1,
    GRAY_ALL_BLOBS = 2,

    GRAY_REGION_PERCENT = 10,
    GRAY_REGION_BOUNDS = 11,
    GRAY_REGION_BLOBS = 12,

    GRAY_REGIONS_PERCENT = 20,
    GRAY_REGIONS_BOUNDS = 21,
    GRAY_REGIONS_BLOBS = 22,

    RGB_ALL_PERCENT = 100,
    RGB_ALL_BOUNDS = 101,
    RGB_ALL_BLOBS = 102,

    RGB_REGION_PERCENT = 110,
    RGB_REGION_BOUNDS = 111,
    RGB_REGION_BLOBS = 112,

    RGB_REGIONS_PERCENT = 120,
    RGB_REGIONS_BOUNDS = 121,
    RGB_REGIONS_BLOBS = 122,
};

struct Config {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t pixelCount;
    uint32_t byteLength;
    bool draw;

    Config() = default;

    Config(uint32_t _width, uint32_t _height, uint32_t _depth, bool _draw) : width(_width), height(_height), depth(_depth), pixelCount(_width * _height), byteLength(_width * _height * _depth), draw(_draw) {}
};

struct All {
    std::string name;
    uint32_t difference;
    uint32_t percent;
};

struct Bounds {
    uint32_t minX;
    uint32_t maxX;
    uint32_t minY;
    uint32_t maxY;

    void expandTo(uint32_t x, uint32_t y) {
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }
};

struct Region {
    std::string name;// 24
    std::vector<bool> bitset;// 24
    uint32_t bitsetCount;// 4
    uint32_t difference;// 4
    uint32_t percent;// 4
    Bounds bounds;
};

struct Blob {
    uint32_t label;
    Bounds bounds;
    uint32_t percent;
    bool flagged;

    Blob() = default;

    explicit Blob(Bounds _bounds) : label(0), bounds(_bounds), percent(0), flagged(false) {}
};

struct Pixels {
    std::unique_ptr<uint8_t[]> ptr;
    //uint8_t *ptr;
    uint32_t size;

    Pixels() : ptr(nullptr), size(0) {}

    //Pixels(uint8_t *_ptr, uint32_t _size) : ptr(_ptr), size(_size) {}
};

struct Result {
    const char *name;
    uint32_t percent;
    Bounds bounds;
    std::vector<Blob> blobs;
    bool flagged;

    Result() : name(nullptr), percent(0), bounds(Bounds{0, 0, 0, 0}), blobs(std::vector<Blob>()), flagged(false) {}

    explicit Result(const char *_name) : name(_name), percent(0), bounds(Bounds{0, 0, 0, 0}), blobs(std::vector<Blob>()), flagged(false) {}

    Result(const char *_name, Bounds _bounds) : name(_name), percent(0), bounds(_bounds), blobs(std::vector<Blob>()), flagged(false) {}
};

// placeholder for CPP results that will be converted to JS values
struct CallbackData {
    std::vector<Result> results;
    Pixels pixels;
};

typedef std::function<void(const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData)> ExecuteFunc;

typedef std::function<void(const Napi::Env &env, const Napi::Function &cb, CallbackData &callbackData)> CallbackFunc;

// absolute value
inline uint32_t
AbsInt(int32_t n) {
    return static_cast<uint32_t>(n < 0 ? -n : n);
}

// measure difference of gray bytes
inline uint32_t
GrayDiff(const uint8_t *buf0, const uint8_t *buf1, const uint32_t i) {
    //return static_cast<uint32_t>(std::abs(buf0[i] - buf1[i]));
    return AbsInt(buf0[i] - buf1[i]);
}

// measure difference of rgb(a) bytes using average
inline uint32_t
RgbDiff(const uint8_t *buf0, const uint8_t *buf1, uint32_t i) {
    //return std::abs(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3u;
    return AbsInt(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3u;
}

// convert js bitset to cpp
std::vector<bool>
BitsetJsToCpp(const Napi::Buffer<bool> &bitsetJs);

// convert js region to cpp
Region
RegionJsToCpp(const Napi::Object &regionJs);

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(const Napi::Array &regionsJs);

// gray all percent
void
GrayAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray single region percent
void
GrayRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray multi regions bounds
void
GrayRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray all bounds
void
GrayAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray single region bounds
void
GrayRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray multi regions bounds
void
GrayRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray all blobs
void
GrayAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray single region blobs
void
GrayRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// gray multi regions blobs
void
GrayRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb all percent
void
RgbAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb single region percent
void
RgbRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb multi regions bounds
void
RgbRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb all bounds
void
RgbAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb single region bounds
void
RgbRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb multi regions bounds
void
RgbRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb all blobs
void
RgbAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb single region blobs
void
RgbRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// rgb multi regions blobs
void
RgbRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData);

// determine engine type
uint32_t
EngineType(uint32_t depth, uint32_t regionsLength, const std::string &response);

// set execute and callback functions
void
SetFunctions(const Napi::Object &config, ExecuteFunc &executeFunc, CallbackFunc &callbackFunc);

#endif