#include "engine.h"
#include <stdint.h>
#include <tuple>
#include <string>
#include <vector>

//gray all percent
uint_fast8_t Engine::MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//gray mask percent
uint_fast8_t Engine::MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

//gray regions percent experiment return array containing percent number of diffs per region
std::vector<uint_fast8_t> Engine::MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<uint_fast8_t> resultsVec;
    resultsVec.reserve(regionsLen);
    std::vector<uint_fast32_t> diffsVec(regionsLen, 0);
    for (uint_fast32_t i = 0; i < pixCount; i++) {
         uint_fast8_t diff = GrayDiff(buf0, buf1, i);
         if (minDiff > diff) continue;
         for (uint_fast8_t r = 0; r < regionsLen; r++) {
             if (std::get<4>(regionsVec[r])[i] == 0 || std::get<1>(regionsVec[r]) > diff) continue;
             diffsVec[r]++;
         }
    }
    for (uint_fast8_t r = 0; r < regionsLen; r++) {
        resultsVec[r] = diffsVec[r] * 100 / std::get<3>(regionsVec[r]);
    }
    return resultsVec;
}

//rgb all percent
uint_fast8_t Engine::MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += depth) {
        if (pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//rgb mask percent
uint_fast8_t Engine::MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += depth, p++) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

//rgb regions percent
std::vector<uint_fast8_t> Engine::MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast32_t bufLen, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<uint_fast8_t> resultsVec;
    resultsVec.reserve(regionsLen);
    std::vector<uint_fast32_t> diffsVec(regionsLen, 0);
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += depth, p++) {
        uint_fast8_t diff = RgbDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (uint_fast8_t r = 0; r < regionsLen; r++) {
            if (std::get<4>(regionsVec[r])[p] == 0 || std::get<1>(regionsVec[r]) > diff) continue;
            diffsVec[r]++;
        }
    }
    for (uint_fast8_t r = 0; r < regionsLen; r++) {
        resultsVec[r] = diffsVec[r] * 100 / std::get<3>(regionsVec[r]);
    }
    return resultsVec;
}

//experiment

//gray all bounds
Engine::BoundsResult Engine::MeasureDiffs(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast16_t maxX = 0, maxY = 0, minX = width - 1, minY = height - 1;
    uint_fast32_t i = 0, diffs = 0;
    for (uint_fast16_t y = 0; y < height; y++) {
        for (uint_fast16_t x = 0; x < width; x++, i++) {
            if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
            minX = MinUint(minX, x);
            maxX = MaxUint(maxX, x);
            minY = MinUint(minY, y);
            maxY = MaxUint(maxY, y);
            diffs++;
        }
    }
    return BoundsResult {uint_fast8_t(100 * diffs / pixCount), minX, maxX, minY, maxY};
}

//gray mask bounds
Engine::BoundsResult Engine::MeasureDiffs(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast16_t maxX = 0, maxY = 0, minX = width - 1, minY = height - 1;
    uint_fast32_t i = 0, diffs = 0;
    for (uint_fast16_t y = 0; y < height; y++) {
        for (uint_fast16_t x = 0; x < width; x++, i++) {
            if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
            minX = MinUint(minX, x);
            maxX = MaxUint(maxX, x);
            minY = MinUint(minY, y);
            maxY = MaxUint(maxY, y);

            diffs++;
        }
    }
    return BoundsResult {uint_fast8_t(100 * diffs / bitsetCount), minX, maxX, minY, maxY};
}

/*std::vector<Engine::BoundsResult> Engine::MeasureDiffs(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<Engine::BoundsResult> resultsVec;
    resultsVec.reserve(regionsLen);

}*/

//if (x > maxX) maxX = x;
//if (y > maxY) maxY = y;
//if (x < minX) minX = x;
//if (y < minY) minY = y;