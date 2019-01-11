#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include <stdint.h>
#include <tuple>
#include <string>
#include <vector>

namespace Engine {

    using Region = std::tuple <const std::string, const uint_fast32_t, const uint_fast32_t, const uint_fast32_t, const std::vector<bool>>;

    //absolute value
    inline uint_fast8_t AbsUint(int_fast16_t n) {
        return (n > 0) ? n : -n;
    }

    //measure difference of gray bytes
    inline uint_fast8_t GrayDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t i) {
        return AbsUint(buf0[i] - buf1[i]);
    }

    //measure difference of rgb(a) bytes using average
    inline uint_fast8_t RgbDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t i) {
        return AbsUint(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
    }

    //gray all percent
    uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    //gray mask percent
    uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    //gray regions percent
    void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<uint_fast32_t> &resultsVec);

    //rgb all percent
    uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    //rgb mask percent
    uint_fast8_t MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    //rgb regions percent
    void MeasureDiffs(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t minDiff, const uint_fast8_t regLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<uint_fast32_t> &resultsVec);

}

#endif