#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include <stdint.h>
#include <tuple>
#include <string>
#include <vector>

namespace Engine {

    // todo switch to struct         name               pixDiff              percent              bitsetCount          bitset
    using Region = std::tuple <const std::string, const uint_fast32_t, const uint_fast32_t, const uint_fast32_t, const std::vector<bool>>;

    struct BoundsResult
    {
        uint_fast32_t percent;
        uint_fast32_t minX;
        uint_fast32_t maxX;
        uint_fast32_t minY;
        uint_fast32_t maxY;
    };

    // absolute value
    inline uint_fast8_t AbsUint(int_fast32_t n) {
        return (n > 0) ? n : -n;
    }

    // measure difference of gray bytes
    inline uint_fast8_t GrayDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t i) {
        return AbsUint(buf0[i] - buf1[i]);
    }

    // measure difference of rgb(a) bytes using average
    inline uint_fast8_t RgbDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, uint_fast32_t i) {
        return AbsUint(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
    }

    // math min
    inline uint_fast32_t MinUint(uint_fast32_t a, uint_fast32_t b) {
        return (a < b) ? a : b;
    }

    // math max
    inline uint_fast32_t MaxUint(uint_fast32_t a, uint_fast32_t b) {
        return (a > b) ? a : b;
    }

    // gray all percent
    uint_fast32_t GrayAllPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray mask percent
    uint_fast32_t GrayMaskPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray regions percent
    std::vector<uint_fast32_t> GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb all percent
    uint_fast32_t RgbAllPercent(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb mask percent
    uint_fast32_t RgbMaskPercent(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb regions percent
    std::vector<uint_fast32_t> RgbRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast32_t bufLen, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray all bounds
    BoundsResult GrayAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray mask bounds
    BoundsResult GrayMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray regions bounds
    std::vector<Engine::BoundsResult> GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb all bounds
    BoundsResult RgbAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb mask bounds
    BoundsResult RgbMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast8_t depth, const uint_fast8_t pixDiff, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb regions bounds
    std::vector<Engine::BoundsResult> RgbRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast8_t depth, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

}

#endif