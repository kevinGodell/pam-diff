#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

enum Engines {
    GRAY_ALL_PERCENT_SYNC = 0,
    GRAY_MASK_PERCENT_SYNC = 10,
    GRAY_REGIONS_PERCENT_SYNC = 20,
    GRAY_ALL_PERCENT_ASYNC = 1000,
    GRAY_MASK_PERCENT_ASYNC = 1010,
    GRAY_REGIONS_PERCENT_ASYNC = 1020,
    RGB_ALL_PERCENT_SYNC = 1,
    RGB_MASK_PERCENT_SYNC = 11,
    RGB_REGIONS_PERCENT_SYNC = 21,
    RGB_ALL_PERCENT_ASYNC = 1001,
    RGB_MASK_PERCENT_ASYNC = 1011,
    RGB_REGIONS_PERCENT_ASYNC = 1021,
    GRAY_ALL_BOUNDS_SYNC = 100,
    GRAY_MASK_BOUNDS_SYNC = 110,
    GRAY_REGIONS_BOUNDS_SYNC = 120,
    GRAY_ALL_BOUNDS_ASYNC = 1100,
    GRAY_MASK_BOUNDS_ASYNC = 1110,
    GRAY_REGIONS_BOUNDS_ASYNC = 1120,
    RGB_ALL_BOUNDS_SYNC = 101,
    RGB_MASK_BOUNDS_SYNC = 111,
    RGB_REGIONS_BOUNDS_SYNC = 121,
    RGB_ALL_BOUNDS_ASYNC = 1101,
    RGB_MASK_BOUNDS_ASYNC = 1111,
    RGB_REGIONS_BOUNDS_ASYNC = 1121,
    /* not yet implemented
    GRAY_ALL_BLOBS_SYNC = 200,
    GRAY_MASK_BLOBS_SYNC = 210,
    GRAY_REGIONS_BLOBS_SYNC = 220,
    GRAY_ALL_BLOBS_ASYNC = 1200,
    GRAY_MASK_BLOBS_ASYNC = 1210,
    GRAY_REGIONS_BLOBS_ASYNC = 1220,
    RGB_ALL_BLOBS_SYNC = 201,
    RGB_MASK_BLOBS_SYNC = 211,
    RGB_REGIONS_BLOBS_SYNC = 221,
    RGB_ALL_BLOBS_ASYNC = 1201,
    RGB_MASK_BLOBS_ASYNC = 1211,
    RGB_REGIONS_BLOBS_ASYNC = 1221
     */
};

struct Region {
    const std::vector<bool> bitset;
    const int_fast32_t pixDiff;
    const uint_fast32_t bitsetCount;
    const uint_fast32_t percent;
    const std::string name;
};

struct BoundsResult {
    uint_fast32_t minX;
    uint_fast32_t maxX;
    uint_fast32_t minY;
    uint_fast32_t maxY;
    uint_fast32_t percent;
};

// absolute value
inline int_fast32_t
AbsInt(int_fast32_t n) {
    return n > 0 ? n : -n;
}

// measure difference of gray bytes
inline int_fast32_t
GrayDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const uint_fast32_t i) {
    return AbsInt(buf0[i] - buf1[i]);
}

// measure difference of rgb(a) bytes using average
inline int_fast32_t
RgbDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const uint_fast32_t i) {
    return AbsInt(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
}

// math min
inline uint_fast32_t
MinUint(const uint_fast32_t a, const uint_fast32_t b) {
    return (a < b) ? a : b;
}

// math max
inline uint_fast32_t
MaxUint(const uint_fast32_t a, const uint_fast32_t b) {
    return (a > b) ? a : b;
}

// gray all percent
uint_fast32_t
GrayAllPercent(uint_fast32_t pixCount, int_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray mask percent
uint_fast32_t
GrayMaskPercent(uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions percent
std::vector<uint_fast32_t>
GrayRegionsPercent(uint_fast32_t pixCount, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray all bounds
BoundsResult
GrayAllBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray mask bounds
BoundsResult
GrayMaskBounds(uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions bounds
std::vector<BoundsResult>
GrayRegionsBounds(uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all percent
uint_fast32_t
RgbAllPercent(uint_fast32_t pixCount, uint_fast32_t pixDepth, int_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb mask percent
uint_fast32_t
RgbMaskPercent(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions percent
std::vector<uint_fast32_t>
RgbRegionsPercent(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all bounds
BoundsResult
RgbAllBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDepth, int_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb mask bounds
BoundsResult
RgbMaskBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth, int_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb regions bounds
std::vector<BoundsResult>
RgbRegionsBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// determine engine type
uint_fast32_t
EngineType(uint_fast32_t pixDepth, const std::string &target, const std::string &returns, bool async);

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(uint_fast32_t pixLen, uint_fast32_t regionsLen, const Napi::Array &regionsJs);

#endif