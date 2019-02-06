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
    const std::vector<bool> bitset;
    const int_fast32_t pixDiff;
    const uint_fast32_t bitsetCount;
    const uint_fast32_t percent;
    const std::string name;
};

struct PercentResult {
    uint_fast32_t percent;
    //std::string name;
    bool flagged;
};

struct BoundsResult {
    uint_fast32_t minX;
    uint_fast32_t maxX;
    uint_fast32_t minY;
    uint_fast32_t maxY;
    uint_fast32_t percent;
    //std::string name;
    bool flagged;
    // todo maybe include pointer to copied and edited pixels
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

// gray all percent
inline PercentResult
GrayAllPercent(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
        ++diffs;
    }
    uint_fast32_t percent = 100 * diffs / pixCount;
    return PercentResult{percent, percent >= diffsPerc};
}

// gray mask percent
inline PercentResult
GrayMaskPercent(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (bitsetVec[p] == 0 || pixDiff > GrayDiff(buf0, buf1, p)) continue;
        ++diffs;
    }
    uint_fast32_t percent = 100 * diffs / bitsetCount;
    return PercentResult{percent, percent >= diffsPerc};
}

// gray regions percent
inline std::vector<PercentResult>
GrayRegionsPercent(const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<PercentResult> percentResultVec(regionsLen, PercentResult{0, false});
    for (uint_fast32_t p = 0, r = 0; p < pixCount; ++p) {
        const int_fast32_t diff = GrayDiff(buf0, buf1, p);
        if (minDiff > diff) continue;
        for (r = 0; r < regionsLen; ++r) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            ++percentResultVec[r].percent;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        percentResultVec[r].flagged = percentResultVec[r].percent >= regionsVec[r].percent;
    }
    return percentResultVec;
}

// gray all bounds
inline BoundsResult
GrayAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, minX);
            SetMax(x, maxX);
            SetMin(y, minY);
            SetMax(y, maxY);
            ++diffs;
        }
    }
    uint_fast32_t percent = 100 * diffs / pixCount;
    return BoundsResult{minX, maxX, minY, maxY, percent, percent >= diffsPerc};
}

// gray mask bounds
inline BoundsResult
GrayMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (bitsetVec[p] == 0 || pixDiff > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, minX);
            SetMax(x, maxX);
            SetMin(y, minY);
            SetMax(y, maxY);
            ++diffs;
        }
    }
    uint_fast32_t percent = 100 * diffs / bitsetCount;
    return BoundsResult{minX, maxX, minY, maxY, percent, percent >= diffsPerc};
}

// gray regions bounds
inline std::vector<BoundsResult>
GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BoundsResult> boundsResultVec(regionsLen, BoundsResult{width - 1, 0, height - 1, 0, 0, false});
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
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        boundsResultVec[r].flagged = boundsResultVec[r].percent >= regionsVec[r].percent;
    }
    return boundsResultVec;
}

// gray regions bounds
inline std::vector<BoundsResult>
GrayRegionsBounds2(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t &resultsCount) {
    std::vector<BoundsResult> boundsResultVec(regionsLen, BoundsResult{width - 1, 0, height - 1, 0, 0, false});
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
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (boundsResultVec[r].percent >= regionsVec[r].percent) {
            boundsResultVec[r].flagged = true;
            ++resultsCount;
        }
    }
    return boundsResultVec;
}

// rgb all percent
inline PercentResult
RgbAllPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++diffs;
    }
    uint_fast32_t percent = 100 * diffs / pixCount;
    return PercentResult{percent, percent >= diffsPerc};
}

// rgb mask percent
inline PercentResult
RgbMaskPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++diffs;
    }
    uint_fast32_t percent = 100 * diffs / bitsetCount;
    return PercentResult{percent, percent >= diffsPerc};
}

// rgb regions percent
inline std::vector<PercentResult>
RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<PercentResult> percentResultVec(regionsLen, PercentResult{0, false});
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        const int_fast32_t diff = RgbDiff(buf0, buf1, p * pixDepth);
        if (minDiff > diff) continue;
        for (uint_fast32_t r = 0; r < regionsLen; ++r) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            ++percentResultVec[r].percent;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        percentResultVec[r].flagged = percentResultVec[r].percent >= regionsVec[r].percent;
    }
    return percentResultVec;
}

// rgb all bounds
inline BoundsResult
RgbAllBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
            SetMin(x, minX);
            SetMax(x, maxX);
            SetMin(y, minY);
            SetMax(y, maxY);
            ++diffs;
        }
    }
    uint_fast32_t percent = 100 * diffs / pixCount;
    return BoundsResult{minX, maxX, minY, maxY, percent, percent >= diffsPerc};
}

// rgb mask bounds
inline BoundsResult
RgbMaskBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
            SetMin(x, minX);
            SetMax(x, maxX);
            SetMin(y, minY);
            SetMax(y, maxY);
            ++diffs;
        }
    }
    uint_fast32_t percent = 100 * diffs / bitsetCount;
    return BoundsResult{minX, maxX, minY, maxY, percent, percent >= diffsPerc};
}

// rgb regions bounds
inline std::vector<BoundsResult>
RgbRegionsBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BoundsResult> boundsResultVec(regionsLen, BoundsResult{width - 1, 0, height - 1, 0, 0, false});
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
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        boundsResultVec[r].flagged = boundsResultVec[r].percent >= regionsVec[r].percent;
    }
    return boundsResultVec;
}

// determine engine type
uint_fast32_t
EngineType(uint_fast32_t pixDepth, const std::string &target, const std::string &returns, bool async);

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(uint_fast32_t pixLen, uint_fast32_t regionsLen, const Napi::Array &regionsJs);

#endif