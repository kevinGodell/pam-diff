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
    const std::string name;
    const uint_fast32_t pixDiff;
    const uint_fast32_t percent;
    const uint_fast32_t bitsetCount;
    const std::vector<bool> bitset;
};

struct BoundsResult {
    uint_fast32_t percent;
    uint_fast32_t minX;
    uint_fast32_t maxX;
    uint_fast32_t minY;
    uint_fast32_t maxY;
};

// absolute value
inline uint_fast32_t AbsUint(int_fast32_t n) {
    return static_cast<uint_fast32_t>((n > 0) ? n : -n);
}

// measure difference of gray bytes
inline uint_fast32_t GrayDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const uint_fast32_t i) {
    return AbsUint(buf0[i] - buf1[i]);
}

// measure difference of rgb(a) bytes using average
inline uint_fast32_t RgbDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const uint_fast32_t i) {
    return AbsUint(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
}

// math min
inline uint_fast32_t MinUint(const uint_fast32_t a, const uint_fast32_t b) {
    return (a < b) ? a : b;
}

// math max
inline uint_fast32_t MaxUint(const uint_fast32_t a, const uint_fast32_t b) {
    return (a > b) ? a : b;
}

// gray all percent
uint_fast32_t GrayAllPercent(uint_fast32_t pixCount, uint_fast32_t pixDiff, const uint_fast8_t *buf0,
                             const uint_fast8_t *buf1);

// gray mask percent
uint_fast32_t GrayMaskPercent(uint_fast32_t pixCount, uint_fast32_t pixDiff, uint_fast32_t bitsetCount,
                              const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions percent
std::vector<uint_fast32_t> GrayRegionsPercent(uint_fast32_t pixCount, uint_fast32_t minDiff, uint_fast32_t regionsLen,
                                              const std::vector<Region> &regionsVec, const uint_fast8_t *buf0,
                                              const uint_fast8_t *buf1);

// rgb all percent
uint_fast32_t RgbAllPercent(uint_fast32_t pixCount, uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t pixDiff,
                            const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb mask percent
uint_fast32_t RgbMaskPercent(uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t pixDiff,
                             uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0,
                             const uint_fast8_t *buf1);

// rgb regions percent
std::vector<uint_fast32_t> RgbRegionsPercent(uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t minDiff,
                                             uint_fast32_t regionsLen, const std::vector<Region> &regionsVec,
                                             const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray all bounds
BoundsResult GrayAllBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDiff,
                           const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray mask bounds
BoundsResult GrayMaskBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDiff, uint_fast32_t bitsetCount,
                            const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// gray regions bounds
std::vector<BoundsResult> GrayRegionsBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t minDiff,
                                            uint_fast32_t regionsLen, const std::vector<Region> &regionsVec,
                                            const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb all bounds
BoundsResult RgbAllBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDepth,
                          uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

// rgb mask bounds
BoundsResult RgbMaskBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth, uint_fast32_t pixDiff,
                           uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0,
                           const uint_fast8_t *buf1);

// rgb regions bounds
std::vector<BoundsResult> RgbRegionsBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth,
                                           uint_fast32_t minDiff, uint_fast32_t regionsLen,
                                           const std::vector<Region> &regionsVec, const uint_fast8_t *buf0,
                                           const uint_fast8_t *buf1);

// determine engine type
uint_fast32_t EngineType(uint_fast32_t pixDepth, const std::string &target, const std::string &returns, bool async);

// convert js regions to cpp
std::vector<Region> RegionsJsToCpp(uint_fast32_t pixLen, uint_fast32_t regionsLen, const Napi::Array &regionsJs);


/*
// gray all percent
inline uint_fast32_t GrayAllPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

// gray mask percent
inline uint_fast32_t GrayMaskPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

// gray regions percent
inline std::vector<uint_fast32_t> GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<uint_fast32_t> percentResultVec(regionsLen, 0);
    for (uint_fast32_t i = 0, r = 0; i < pixCount; i++) {
        uint_fast32_t diff = GrayDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (r = 0; r < regionsLen; r++) {
            if (regionsVec[r].bitset[i] == 0 || regionsVec[r].pixDiff > diff) continue;
            percentResultVec[r]++;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; r++) {
        percentResultVec[r] = percentResultVec[r] * 100 / regionsVec[r].bitsetCount;
    }
    return percentResultVec;
}

// rgb all percent
inline uint_fast32_t RgbAllPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += pixDepth) {
        if (pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

// rgb mask percent
inline uint_fast32_t RgbMaskPercent(const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += pixDepth, p++) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

// rgb regions percent
inline std::vector<uint_fast32_t> RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<uint_fast32_t> percentResultVec(regionsLen, 0);
    for (uint_fast32_t i = 0, p = 0, r = 0; i < bufLen; i += pixDepth, p++) {
        uint_fast32_t diff = RgbDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (r = 0; r < regionsLen; r++) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            percentResultVec[r]++;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; r++) {
        percentResultVec[r] = percentResultVec[r] * 100 / regionsVec[r].bitsetCount;
    }
    return percentResultVec;
}

// gray all bounds
inline BoundsResult GrayAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, x = 0, i = 0; y < height; y++) {
        for (x = 0; x < width; x++, i++) {
            if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
            minX = MinUint(minX, x);
            maxX = MaxUint(maxX, x);
            minY = MinUint(minY, y);
            maxY = MaxUint(maxY, y);
            diffs++;
        }
    }
    return BoundsResult {100 * diffs / pixCount, minX, maxX, minY, maxY};
}

// gray mask bounds
inline BoundsResult GrayMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, x = 0, i = 0; y < height; y++) {
        for (x = 0; x < width; x++, i++) {
            if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
            minX = MinUint(minX, x);
            maxX = MaxUint(maxX, x);
            minY = MinUint(minY, y);
            maxY = MaxUint(maxY, y);
            diffs++;
        }
    }
    return BoundsResult {100 * diffs / bitsetCount, minX, maxX, minY, maxY};
}

// gray regions bounds
inline std::vector<BoundsResult> GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BoundsResult> boundsResultVec(regionsLen, BoundsResult {0, width - 1 , 0, height - 1, 0});
    for (uint_fast32_t y = 0, x = 0, i = 0, r = 0; y < height; y++) {
        for (x = 0; x < width; x++, i++) {
            uint_fast32_t diff = GrayDiff(buf0, buf1, i);
            if (minDiff > diff) continue;
            for (r = 0; r < regionsLen; r++) {
                if (regionsVec[r].bitset[i] == 0 || regionsVec[r].pixDiff > diff) continue;
                boundsResultVec[r].minX = MinUint(boundsResultVec[r].minX, x);
                boundsResultVec[r].maxX = MaxUint(boundsResultVec[r].maxX, x);
                boundsResultVec[r].minY = MinUint(boundsResultVec[r].minY, y);
                boundsResultVec[r].maxY = MaxUint(boundsResultVec[r].maxY, y);
                boundsResultVec[r].percent++;
            }
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; r++) {
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
    }
    return boundsResultVec;
}

// rgb all bounds
inline BoundsResult RgbAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDepth, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, x = 0, i = 0; y < height; y++) {
        for (x = 0; x < width; x++, i += pixDepth) {
            if (pixDiff > RgbDiff(buf0, buf1, i)) continue;
            minX = MinUint(minX, x);
            maxX = MaxUint(maxX, x);
            minY = MinUint(minY, y);
            maxY = MaxUint(maxY, y);
            diffs++;
        }
    }
    return BoundsResult {100 * diffs / pixCount, minX, maxX, minY, maxY};
}

// rgb mask bounds
inline BoundsResult RgbMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t minX = width - 1, maxX = 0, minY = height - 1, maxY = 0, diffs = 0;
    for (uint_fast32_t y = 0, x = 0, i = 0, p = 0; y < height; y++) {
        for (x = 0; x < width; x++, i += pixDepth, p++) {
            if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, i)) continue;
            minX = MinUint(minX, x);
            maxX = MaxUint(maxX, x);
            minY = MinUint(minY, y);
            maxY = MaxUint(maxY, y);
            diffs++;
        }
    }
    return BoundsResult {100 * diffs / bitsetCount, minX, maxX, minY, maxY};
}

// rgb regions bounds
inline std::vector<BoundsResult> RgbRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BoundsResult> boundsResultVec(regionsLen, BoundsResult {0, width - 1 , 0, height - 1, 0});
    for (uint_fast32_t y = 0, x = 0, i = 0, p = 0, r = 0; y < height; y++) {
        for (x = 0; x < width; x++, i += pixDepth, p++) {
            uint_fast32_t diff = RgbDiff(buf0, buf1, i);
            if (minDiff > diff) continue;
            for (r = 0; r < regionsLen; r++) {
                if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
                boundsResultVec[r].minX = MinUint(boundsResultVec[r].minX, x);
                boundsResultVec[r].maxX = MaxUint(boundsResultVec[r].maxX, x);
                boundsResultVec[r].minY = MinUint(boundsResultVec[r].minY, y);
                boundsResultVec[r].maxY = MaxUint(boundsResultVec[r].maxY, y);
                boundsResultVec[r].percent++;
            }
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; r++) {
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
    }
    return boundsResultVec;
}

// determine engine type
inline uint_fast32_t EngineType(const uint_fast32_t pixDepth, const std::string &target, const std::string &response, const bool async) {
    uint_fast32_t value = 0;
    if (pixDepth == 3 || pixDepth == 4) {//dont add for pixDepth == 1
        value += 1;
    }
    if (target == "mask") {//dont add for target == "all"
        value += 10;
    } else if (target == "regions") {
        value += 20;
    }
    if (response == "bounds") {//dont add for target == "percent"
        value += 100;
    } else if (response == "blobs") {
        value += 200;
    }
    if (async) {
        value += 1000;
    }
    return value;
}

// convert js regions to cpp
inline std::vector<Region> RegionsJsToCpp(const uint_fast32_t pixLen, const uint_fast32_t regionsLen, const Napi::Array &regionsJs) {
    std::vector<Region> regionVec;
    regionVec.reserve(regionsLen);
    for (uint_fast32_t i = 0; i < regionsLen; i++) {
        const std::string name = regionsJs.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast32_t diff = regionsJs.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsJs.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsJs.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const bool *bitset = regionsJs.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<bool>>().Data();
        std::vector<bool> bitsetVec;
        bitsetVec.assign(bitset, bitset + pixLen);
        regionVec.push_back(Region {name, diff, percent, count, bitsetVec});
    }
    return regionVec;
}*/

#endif