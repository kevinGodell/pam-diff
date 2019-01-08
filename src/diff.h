#ifndef SRC_DIFF_H_
#define SRC_DIFF_H_

#include <stdint.h>
#include <tuple>
#include <string>
#include <vector>

//todo get rid of "diffs" entry in tuple
//region     =             name,        diff,          percent,       count,         bitset,              diffs
using Region = std::tuple <std::string, uint_fast32_t, uint_fast32_t, uint_fast32_t, const uint_fast8_t*, uint_fast32_t>;

//absolute value
inline uint_fast8_t AbsUint(int_fast16_t n) {
    return (n > 0) ? n : -n;
}

//measure difference of gray bytes
inline uint_fast8_t GrayDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t i) {
    return AbsUint(buf0[i] - buf1[i]);
}

//measure difference of rgb(a) bytes
inline uint_fast8_t RgbDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t i) {
    return AbsUint(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
}

//gray all
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);
//grall all inline

inline uint_fast8_t GrayAllPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / pixCount;
}

//gray mask
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const uint_fast8_t *bitset, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

//vec
inline uint_fast8_t GrayMaskPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < pixCount; i++) {
        if (bitsetVec[i] == 0 || pixDiff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
    }
    return 100 * diffs / bitsetCount;
}

//gray regions
void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regLen, Region *regionsArr, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

//gray regions vec
//void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regLen, std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

//gray regions vec and return vec
inline void GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<uint_fast32_t> &resultsVec) {
    for (uint_fast32_t i = 0, j = 0, diff = 0; i < pixCount; i++) {
         diff = GrayDiff(buf0, buf1, i);
         if (minDiff > diff) continue;
         for (j = 0; j < regLen; j++) {
            if (std::get<4>(regionsVec[j])[i] && diff >= std::get<1>(regionsVec[j])) resultsVec[j]++;
         }
    }
}

//rgb(a) all
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

//rgb(a) mask
uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const uint_fast8_t *bitset, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

//rgb(a) regions
void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t minDiff, const uint_fast8_t regLen, Region *regionsArr, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

#endif
