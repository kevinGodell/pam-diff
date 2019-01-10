#include "engine.h"
#include <stdint.h>
#include <tuple>
#include <string>
#include <vector>

//gray all
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//gray mask
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

//gray regions
void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<uint_fast32_t> &resultsVec) {
    for (uint_fast32_t i = 0, j = 0, diff = 0; i < pixCount; i++) {
         diff = GrayDiff(buf0, buf1, i);
         if (minDiff > diff) continue;
         for (j = 0; j < regLen; j++) {
             if (std::get<4>(regionsVec[j])[i] && diff >= std::get<1>(regionsVec[j])) resultsVec[j]++;
         }
    }
}

//rgb all
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t bufLen = pixCount * depth;
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += depth) {
        if (pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//rgb mask
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t bufLen = pixCount * depth;
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += depth, p++) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

//rgb regions
void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t minDiff, const uint_fast8_t regLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<uint_fast32_t> &resultsVec) {
    uint_fast32_t bufLen = pixCount * depth;
    for (uint_fast32_t i = 0, j = 0, p = 0, diff = 0; i < bufLen; i += depth, p++) {
        diff = RgbDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (j = 0; j < regLen; j++) {
            if (std::get<4>(regionsVec[j])[p] && diff >= std::get<1>(regionsVec[j])) resultsVec[j]++;
        }
    }
}