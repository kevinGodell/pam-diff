#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

enum Engines {// ordered by depth -> target -> response -> synchronicity
    GRAY_ALL_PERCENT_SYNC = 0,
    GRAY_ALL_PERCENT_ASYNC = 1,
    GRAY_ALL_BOUNDS_SYNC = 10,
    GRAY_ALL_BOUNDS_ASYNC = 11,
    GRAY_ALL_BLOBS_SYNC = 20,
    GRAY_ALL_BLOBS_ASYNC = 21,

    GRAY_REGION_PERCENT_SYNC = 100,
    GRAY_REGION_PERCENT_ASYNC = 101,
    GRAY_REGION_BOUNDS_SYNC = 110,
    GRAY_REGION_BOUNDS_ASYNC = 111,
    GRAY_REGION_BLOBS_SYNC = 120,
    GRAY_REGION_BLOBS_ASYNC = 121,

    GRAY_REGIONS_PERCENT_SYNC = 200,
    GRAY_REGIONS_PERCENT_ASYNC = 201,
    GRAY_REGIONS_BOUNDS_SYNC = 210,
    GRAY_REGIONS_BOUNDS_ASYNC = 211,
    GRAY_REGIONS_BLOBS_SYNC = 220,
    GRAY_REGIONS_BLOBS_ASYNC = 221,

    RGB_ALL_PERCENT_SYNC = 1000,
    RGB_ALL_PERCENT_ASYNC = 1001,
    RGB_ALL_BOUNDS_SYNC = 1010,
    RGB_ALL_BOUNDS_ASYNC = 1011,
    RGB_ALL_BLOBS_SYNC = 1020,
    RGB_ALL_BLOBS_ASYNC = 1021,

    RGB_REGION_PERCENT_SYNC = 1100,
    RGB_REGION_PERCENT_ASYNC = 1101,
    RGB_REGION_BOUNDS_SYNC = 1110,
    RGB_REGION_BOUNDS_ASYNC = 1111,
    RGB_REGION_BLOBS_SYNC = 1120,
    RGB_REGION_BLOBS_ASYNC = 1121,

    RGB_REGIONS_PERCENT_SYNC = 1200,
    RGB_REGIONS_PERCENT_ASYNC = 1201,
    RGB_REGIONS_BOUNDS_SYNC = 1210,
    RGB_REGIONS_BOUNDS_ASYNC = 1211,
    RGB_REGIONS_BLOBS_SYNC = 1220,
    RGB_REGIONS_BLOBS_ASYNC = 1221,
};

struct Config {
    uint_fast32_t width;
    uint_fast32_t height;
    uint_fast32_t depth;
    uint_fast32_t pixelCount;
    uint_fast32_t byteLength;
};

struct All {
    uint_fast32_t difference;
    uint_fast32_t percent;
};

struct Bounds {
    uint_fast32_t minX;
    uint_fast32_t maxX;
    uint_fast32_t minY;
    uint_fast32_t maxY;
};

struct Region {
    std::string name;// 24
    std::vector<bool> bitset;// 24
    uint_fast32_t bitsetCount;// 4
    uint_fast32_t difference;// 4
    uint_fast32_t percent;// 4
    Bounds bounds;
};

struct Regions {
    std::vector<Region> regions;
    std::vector<bool> bitset;
    uint_fast32_t difference;
    Bounds bounds;
};

struct PercentResult {
    std::string name;// 24
    uint_fast32_t percent;// 4
    bool flagged;// 1
};

struct BoundsResult {
    std::string name;
    Bounds bounds;
    uint_fast32_t percent;
    bool flagged;
};

struct Blob {
    uint_fast32_t label;
    Bounds bounds;
    uint_fast32_t percent;
    bool flagged;
};

struct BlobsResult {
    std::string name;// 24
    Bounds bounds;
    uint_fast32_t percent;// 4
    bool flagged;// 1
    std::vector<Blob> blobs;
};

// measure difference of gray bytes
inline uint_fast32_t
GrayDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const uint_fast32_t i) {
    return static_cast<uint_fast32_t>(std::abs(buf0[i] - buf1[i]));
}

// measure difference of rgb(a) bytes using average
inline uint_fast32_t
RgbDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t i) {
    return std::abs(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3u;
}

// set minimum x or y coord
inline void
SetMin(const uint_fast32_t &val, uint_fast32_t &min) {
    if (val < min) min = val;
}

// set maximum x or y coord
inline void
SetMax(const uint_fast32_t &val, uint_fast32_t &max) {
    if (val > max) max = val;
}

// determine engine type
uint_fast32_t
EngineType(uint_fast32_t depth, uint_fast32_t regionsLength, const std::string &response, bool async);

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
PercentResult
GrayAllPercent(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray region percent
PercentResult
GrayRegionPercent(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions percent
std::vector<PercentResult>
GrayRegionsPercent(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray all bounds
BoundsResult
GrayAllBounds(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray region bounds
BoundsResult
GrayRegionBounds(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions bounds
std::vector<BoundsResult>
GrayRegionsBounds(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray all blobs
BlobsResult
GrayAllBlobs(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray region blobs
BlobsResult
GrayRegionBlobs(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions blobs
std::vector<BlobsResult>
GrayRegionsBlobs(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all percent
PercentResult
RgbAllPercent(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb region percent
PercentResult
RgbRegionPercent(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions percent
std::vector<PercentResult>
RgbRegionsPercent(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all bounds
BoundsResult
RgbAllBounds(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb region bounds
BoundsResult
RgbRegionBounds(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions bounds
std::vector<BoundsResult>
RgbRegionsBounds(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all blobs
BlobsResult
RgbAllBlobs(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb region blobs
BlobsResult
RgbRegionBlobs(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions blobs
std::vector<BlobsResult>
RgbRegionsBlobs(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

#endif