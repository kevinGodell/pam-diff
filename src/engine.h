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
    const std::string name;// 24
    const std::vector<bool> bitset;// 24
    const int_fast32_t pixDiff;// 4
    const uint_fast32_t bitsetCount;// 4
    const uint_fast32_t percent;// 4
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
RegionsJsToCpp(uint_fast32_t pixCount, uint_fast32_t regionsLen, const Napi::Array &regionsJs);

// gray all percent
inline void
GrayAllPercent(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    percentResult = PercentResult{"all", 0, false};
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / pixCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// gray mask percent
inline void
GrayMaskPercent(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    percentResult = PercentResult{"mask", 0, false};
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (bitsetVec[p] == 0 || pixDiff > GrayDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / bitsetCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// gray regions percent
inline uint_fast32_t
GrayRegionsPercent(const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
    percentResultVec = std::vector<PercentResult>(regionsLen, PercentResult{std::string(), 0, false});
    for (uint_fast32_t p = 0, r = 0; p < pixCount; ++p) {
        const int_fast32_t diff = GrayDiff(buf0, buf1, p);
        if (minDiff > diff) continue;
        for (r = 0; r < regionsLen; ++r) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            ++percentResultVec[r].percent;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].name = regionsVec[r].name;
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (percentResultVec[r].percent >= regionsVec[r].percent) {
            percentResultVec[r].flagged = true;
            ++flaggedCount;
        }
    }
    return flaggedCount;
}

// gray all bounds
inline void
GrayAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    boundsResult = BoundsResult{"all", width - 1, 0, height - 1, 0, 0, false};
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / pixCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// gray mask bounds
inline void
GrayMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    boundsResult = BoundsResult{"mask", width - 1, 0, height - 1, 0, 0, false};
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (bitsetVec[p] == 0 || pixDiff > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / bitsetCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// gray regions bounds
inline uint_fast32_t
GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
    boundsResultVec = std::vector<BoundsResult>(regionsLen, BoundsResult{std::string(), width - 1, 0, height - 1, 0, 0, false});
    for (uint_fast32_t y = 0, x = 0, i = 0, r = 0; y < height; ++y) {
        for (x = 0; x < width; ++x, ++i) {
            const int_fast32_t diff = GrayDiff(buf0, buf1, i);
            if (minDiff > diff) continue;
            for (r = 0; r < regionsLen; ++r) {
                if (regionsVec[r].bitset[i] == 0 || regionsVec[r].pixDiff > diff) continue;
                SetMin(x, boundsResultVec[r].minX);
                SetMax(x, boundsResultVec[r].maxX);
                SetMin(y, boundsResultVec[r].minY);
                SetMax(y, boundsResultVec[r].maxY);
                ++boundsResultVec[r].percent;
            }
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        boundsResultVec[r].name = regionsVec[r].name;
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (boundsResultVec[r].percent >= regionsVec[r].percent) {
            boundsResultVec[r].flagged = true;
            ++flaggedCount;
        }
    }
    return flaggedCount;
}

// rgb all percent
inline void
RgbAllPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    percentResult = PercentResult{"all", 0, false};
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / pixCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// rgb mask percent
inline void
RgbMaskPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    percentResult = PercentResult{"mask", 0, false};
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / bitsetCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// rgb regions percent
inline uint_fast32_t
RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
    percentResultVec = std::vector<PercentResult>(regionsLen, PercentResult{std::string(), 0, false});
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        const int_fast32_t diff = RgbDiff(buf0, buf1, p * pixDepth);
        if (minDiff > diff) continue;
        for (uint_fast32_t r = 0; r < regionsLen; ++r) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            ++percentResultVec[r].percent;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].name = regionsVec[r].name;
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (percentResultVec[r].percent >= regionsVec[r].percent) {
            percentResultVec[r].flagged = true;
            ++flaggedCount;
        }
    }
    return flaggedCount;
}

// rgb all bounds
inline void
RgbAllBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    boundsResult = BoundsResult{"all", width - 1, 0, height - 1, 0, 0, false};
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / pixCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// rgb mask bounds
inline void
RgbMaskBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    boundsResult = BoundsResult{"mask", width - 1, 0, height - 1, 0, 0, false};
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / bitsetCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// rgb regions bounds
inline uint_fast32_t
RgbRegionsBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
    boundsResultVec = std::vector<BoundsResult>(regionsLen, BoundsResult{std::string(), width - 1, 0, height - 1, 0, 0, false});
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            const int_fast32_t diff = RgbDiff(buf0, buf1, p * pixDepth);
            if (minDiff > diff) continue;
            for (uint_fast32_t r = 0; r < regionsLen; ++r) {
                if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
                SetMin(x, boundsResultVec[r].minX);
                SetMax(x, boundsResultVec[r].maxX);
                SetMin(y, boundsResultVec[r].minY);
                SetMax(y, boundsResultVec[r].maxY);
                ++boundsResultVec[r].percent;
            }
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        boundsResultVec[r].name = regionsVec[r].name;
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (boundsResultVec[r].percent >= regionsVec[r].percent) {
            boundsResultVec[r].flagged = true;
            ++flaggedCount;
        }
    }
    return flaggedCount;
}

#endif