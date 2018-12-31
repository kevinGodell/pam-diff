#include <stdint.h>
#include "diff.h"

//absolute value
inline uint_fast8_t AbsUint(int_fast16_t n) {
    return (n > 0) ? n : -n;
}

//measure difference of gray bytes
inline uint_fast8_t GrayDiff(const uint_fast8_t* buffer0, const uint_fast8_t* buffer1, uint_fast32_t i) {
    return AbsUint(buffer0[i] - buffer1[i]);
}

//measure difference of rgb/rgba bytes
inline uint_fast8_t RgbDiff(const uint_fast8_t* buffer0, const uint_fast8_t* buffer1, uint_fast32_t i) {
    return AbsUint(buffer0[i] + buffer0[i + 1] + buffer0[i + 2] - buffer1[i] - buffer1[i + 1] - buffer1[i + 2]) / 3;
}

//returns percent of changed pixels
uint_fast8_t DiffsPercent(const uint_fast32_t pixDiff, const uint_fast32_t pixCount, const uint_fast8_t* buffer0, const uint_fast8_t* buffer1 /*, uint_fast8_t (*pixelDiff)(const uint_fast8_t* buffer0, const uint_fast8_t* buffer1, uint_fast32_t i)*/) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buffer0, buffer1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//returns percent of changed pixels
uint_fast8_t DiffsPercent(const uint_fast32_t pixDiff, const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t* buffer0, const uint_fast8_t* buffer1 /*, uint_fast8_t (*pixelDiff)(const uint_fast8_t* buffer0, const uint_fast8_t* buffer1, uint_fast32_t i)*/) {
    uint_fast32_t bufLen = pixCount * depth;
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += depth) {
        if (pixDiff > RgbDiff(buffer0, buffer1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}