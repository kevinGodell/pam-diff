#include "diff.h"

//gray all diff
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//gray mask diff
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const uint_fast8_t *bitset, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (bitset[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

//gray regions diff
void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regLen, Region *regionsCpp, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    for (uint_fast32_t i = 0, j = 0, diff = 0; i < pixCount; i++) {
         diff = GrayDiff(buf0, buf1, i);
         if (minDiff > diff) continue;
         for (j = 0; j < regLen; j++) {
            if (std::get<4>(regionsCpp[j])[i] && diff >= std::get<1>(regionsCpp[j])) std::get<5>(regionsCpp[j])++;
         }
    }
}

//rgb all diff
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t bufLen = pixCount * depth;
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += depth) {
        if (pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//rgb mask diff
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const uint_fast8_t *bitset, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t bufLen = pixCount * depth;
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += depth, p++) {
        if (bitset[p] == 0 || pixDiff > RgbDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

//rgb regions diff
void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t minDiff, const uint_fast8_t regLen, Region *regionsCpp, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t bufLen = pixCount * depth;
    for (uint_fast32_t i = 0, j = 0, p = 0, diff = 0; i < bufLen; i += depth, p++) {
        diff = RgbDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (j = 0; j < regLen; j++) {
            if (std::get<4>(regionsCpp[j])[p] && diff >= std::get<1>(regionsCpp[j])) std::get<5>(regionsCpp[j])++;
        }
    }
}