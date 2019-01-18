#include "engine.h"
#include <cstdint>
#include <string>
#include <vector>

// gray all percent
uint_fast32_t Engine::GrayAllPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

// gray mask percent
uint_fast32_t Engine::GrayMaskPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

// gray regions percent
std::vector<uint_fast32_t> Engine::GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
uint_fast32_t Engine::RgbAllPercent(const uint_fast32_t pixCount, const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += pixDepth) {
        if (pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

// rgb mask percent
uint_fast32_t Engine::RgbMaskPercent(const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += pixDepth, p++) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

// rgb regions percent
std::vector<uint_fast32_t> Engine::RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
Engine::BoundsResult Engine::GrayAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return Engine::BoundsResult {100 * diffs / pixCount, minX, maxX, minY, maxY};
}

// gray mask bounds
Engine::BoundsResult Engine::GrayMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return Engine::BoundsResult {100 * diffs / bitsetCount, minX, maxX, minY, maxY};
}

// gray regions bounds
std::vector<Engine::BoundsResult> Engine::GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<Engine::BoundsResult> boundsResultVec(regionsLen, Engine::BoundsResult {0, width - 1 , 0, height - 1, 0});
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
Engine::BoundsResult Engine::RgbAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDepth, const uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return Engine::BoundsResult {100 * diffs / pixCount, minX, maxX, minY, maxY};
}

// rgb mask bounds
Engine::BoundsResult Engine::RgbMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
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
    return Engine::BoundsResult {100 * diffs / bitsetCount, minX, maxX, minY, maxY};
}

// rgb regions bounds
std::vector<Engine::BoundsResult> Engine::RgbRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<Engine::BoundsResult> boundsResultVec(regionsLen, Engine::BoundsResult {0, width - 1 , 0, height - 1, 0});
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