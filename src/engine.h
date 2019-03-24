#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

enum Engines {
    GRAY_ALL_PERCENT_SYNC = 0,
    GRAY_ALL_PERCENT_ASYNC = 1000,

    GRAY_REGION_PERCENT_SYNC = 10,
    GRAY_REGION_PERCENT_ASYNC = 1010,

    GRAY_REGIONS_PERCENT_SYNC = 20,
    GRAY_REGIONS_PERCENT_ASYNC = 1020,

    GRAY_ALL_BOUNDS_SYNC = 100,
    GRAY_ALL_BOUNDS_ASYNC = 1100,

    GRAY_REGION_BOUNDS_SYNC = 110,
    GRAY_REGION_BOUNDS_ASYNC = 1110,

    GRAY_REGIONS_BOUNDS_SYNC = 120,
    GRAY_REGIONS_BOUNDS_ASYNC = 1120,

    GRAY_ALL_BLOBS_SYNC = 200,
    GRAY_ALL_BLOBS_ASYNC = 1200,

    GRAY_REGION_BLOBS_SYNC = 210,
    GRAY_REGION_BLOBS_ASYNC = 1210,

    GRAY_REGIONS_BLOBS_SYNC = 220,
    GRAY_REGIONS_BLOBS_ASYNC = 1220,

    RGB_ALL_PERCENT_SYNC = 1,
    RGB_ALL_PERCENT_ASYNC = 1001,

    RGB_REGION_PERCENT_SYNC = 11,
    RGB_REGION_PERCENT_ASYNC = 1011,

    RGB_REGIONS_PERCENT_SYNC = 21,
    RGB_REGIONS_PERCENT_ASYNC = 1021,

    RGB_ALL_BOUNDS_SYNC = 101,
    RGB_ALL_BOUNDS_ASYNC = 1101,

    RGB_REGION_BOUNDS_SYNC = 111,
    RGB_REGION_BOUNDS_ASYNC = 1111,

    RGB_REGIONS_BOUNDS_SYNC = 121,
    RGB_REGIONS_BOUNDS_ASYNC = 1121,

    RGB_ALL_BLOBS_SYNC = 201,
    RGB_ALL_BLOBS_ASYNC = 1201,

    RGB_REGION_BLOBS_SYNC = 211,
    RGB_REGION_BLOBS_ASYNC = 1211,

    RGB_REGIONS_BLOBS_SYNC = 221,
    RGB_REGIONS_BLOBS_ASYNC = 1221
};

/*using DiffFuncPtr = uint_fast32_t (*)(const uint_fast8_t *, const uint_fast8_t *, const uint_fast32_t);

struct Dimensions2 {
    uint_fast32_t width;
    uint_fast32_t height;
    uint_fast32_t depth;
    uint_fast32_t pixelCount;
    uint_fast32_t byteLength;
    DiffFuncPtr diff;
};*/

struct Dimensions {
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
EngineType(uint_fast32_t pixDepth, const std::string &returns, bool async, uint_fast32_t regionsLength);

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
GrayAllPercent(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray region percent
PercentResult
GrayRegionPercent(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions percent
std::vector<PercentResult>
GrayRegionsPercent(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray all bounds
BoundsResult
GrayAllBounds(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray region bounds
BoundsResult
GrayRegionBounds(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions bounds
std::vector<BoundsResult>
GrayRegionsBounds(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray all blobs
BlobsResult
GrayAllBlobs(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray region blobs
BlobsResult
GrayRegionBlobs(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions blobs
std::vector<BlobsResult>
GrayRegionsBlobs(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all percent
PercentResult
RgbAllPercent(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb region percent
PercentResult
RgbRegionPercent(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions percent
std::vector<PercentResult>
RgbRegionsPercent(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all bounds
BoundsResult
RgbAllBounds(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb region bounds
BoundsResult
RgbRegionBounds(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions bounds
std::vector<BoundsResult>
RgbRegionsBounds(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all blobs
BlobsResult
RgbAllBlobs(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb region blobs
BlobsResult
RgbRegionBlobs(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions blobs
std::vector<BlobsResult>
RgbRegionsBlobs(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

#endif