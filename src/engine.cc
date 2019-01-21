#include "engine.h"
#include <cstdint>
#include <string>
#include <vector>
#include <napi.h>

// gray all percent
uint_fast32_t
GrayAllPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

// gray mask percent
uint_fast32_t
GrayMaskPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount,
                const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

// gray regions percent
std::vector<uint_fast32_t>
GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast32_t minDiff, const uint_fast32_t regionsLen,
                   const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
uint_fast32_t
RgbAllPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff,
              const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += pixDepth) {
        if (pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

// rgb mask percent
uint_fast32_t
RgbMaskPercent(const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount,
               const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += pixDepth, p++) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

// rgb regions percent
std::vector<uint_fast32_t>
RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t minDiff, const uint_fast32_t regionsLen,
                  const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
BoundsResult
GrayAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDiff,
              const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return BoundsResult{100 * diffs / pixCount, minX, maxX, minY, maxY};
}

// gray mask bounds
BoundsResult
GrayMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount,
               const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return BoundsResult{100 * diffs / bitsetCount, minX, maxX, minY, maxY};
}

// gray regions bounds
std::vector<BoundsResult>
GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t minDiff, const uint_fast32_t regionsLen,
                  const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BoundsResult> boundsResultVec(regionsLen, BoundsResult{0, width - 1, 0, height - 1, 0});
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
BoundsResult
RgbAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDepth,
             const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return BoundsResult{100 * diffs / pixCount, minX, maxX, minY, maxY};
}

// rgb mask bounds
BoundsResult
RgbMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t pixDiff,
              const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return BoundsResult{100 * diffs / bitsetCount, minX, maxX, minY, maxY};
}

// rgb regions bounds
std::vector<BoundsResult>
RgbRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t minDiff,
                 const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BoundsResult> boundsResultVec(regionsLen, BoundsResult{0, width - 1, 0, height - 1, 0});
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
uint_fast32_t
EngineType(const uint_fast32_t pixDepth, const std::string &target, const std::string &response, const bool async) {
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
std::vector<Region>
RegionsJsToCpp(const uint_fast32_t pixLen, const uint_fast32_t regionsLen, const Napi::Array &regionsJs) {
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
        regionVec.push_back(Region{name, diff, percent, count, bitsetVec});
    }
    return regionVec;
}