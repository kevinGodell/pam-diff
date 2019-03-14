#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

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

    GRAY_ALL_BLOBS_SYNC = 200,
    GRAY_MASK_BLOBS_SYNC = 210,
    GRAY_REGIONS_BLOBS_SYNC = 220,
    GRAY_ALL_BLOBS_ASYNC = 1200,
    GRAY_MASK_BLOBS_ASYNC = 1210,
    //GRAY_REGIONS_BLOBS_ASYNC = 1220,
    //RGB_ALL_BLOBS_SYNC = 201,
    //RGB_MASK_BLOBS_SYNC = 211,
    //RGB_REGIONS_BLOBS_SYNC = 221,
    //RGB_ALL_BLOBS_ASYNC = 1201,
    //RGB_MASK_BLOBS_ASYNC = 1211,
    //RGB_REGIONS_BLOBS_ASYNC = 1221
};

struct Region {
    const std::string name;// 24
    const std::vector<bool> bitset;// 24
    const int_fast32_t pixDiff;// 4
    const uint_fast32_t bitsetCount;// 4
    const uint_fast32_t percent;// 4
    const uint_fast32_t minX;
    const uint_fast32_t maxX;
    const uint_fast32_t minY;
    const uint_fast32_t maxY;
};

struct PercentResult {
    std::string name;// 24
    uint_fast32_t percent;// 4
    bool flagged;// 1
};

struct BoundsResult {
    std::string name;// 24
    uint_fast32_t minX;// 4
    uint_fast32_t maxX;// 4
    uint_fast32_t minY;// 4
    uint_fast32_t maxY;// 4
    uint_fast32_t percent;// 4
    bool flagged;// 1
};

struct Blob {
    uint_fast32_t label;
    uint_fast32_t minX;
    uint_fast32_t maxX;
    uint_fast32_t minY;
    uint_fast32_t maxY;
    uint_fast32_t percent;
    bool flagged;
};

struct BlobsResult {
    std::string name;// 24
    uint_fast32_t minX;// 4
    uint_fast32_t maxX;// 4
    uint_fast32_t minY;// 4
    uint_fast32_t maxY;// 4
    uint_fast32_t percent;// 4
    bool flagged;// 1
    std::vector<Blob> blobs;
};

// absolute value
inline int_fast32_t
AbsInt(int_fast32_t n) {
    return n < 0 ? -n : n;
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
EngineType(uint_fast32_t pixDepth, const std::string &target, const std::string &returns, bool async);

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(uint_fast32_t pixCount, const Napi::Array &regionsJs);

// gray all percent
void
GrayAllPercent(uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult);

// gray mask percent
void
GrayMaskPercent(uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult);

// gray regions percent
uint_fast32_t
GrayRegionsPercent(uint_fast32_t pixCount, int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec);

// gray all bounds
void
GrayAllBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult);

// gray mask bounds
void
GrayMaskBounds(uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult);

// gray regions bounds
uint_fast32_t
GrayRegionsBounds(uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec);

// gray all blobs
void
GrayAllBlobs(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BlobsResult &blobsResult);

// gray mask blobs
void
GrayMaskBlobs(uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BlobsResult &blobsResult);

// gray regions blobs
uint_fast32_t
GrayRegionsBlobs(uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BlobsResult> &blobsResultVec);

// rgb all percent
void
RgbAllPercent(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult);

// rgb mask percent
void
RgbMaskPercent(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult);

// rgb regions percent
uint_fast32_t
RgbRegionsPercent(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec);

// rgb all bounds
void
RgbAllBounds(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult);

// rgb mask bounds
void
RgbMaskBounds(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult);

// rgb regions bounds
uint_fast32_t
RgbRegionsBounds(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec);

#endif