#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include <cstdint>
#include <string>
#include <vector>

namespace Engine {

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
    uint_fast32_t GrayAllPercent(uint_fast32_t pixCount, uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray mask percent
    uint_fast32_t GrayMaskPercent(uint_fast32_t pixCount, uint_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray regions percent
    std::vector<uint_fast32_t> GrayRegionsPercent(uint_fast32_t pixCount, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb all percent
    uint_fast32_t RgbAllPercent(uint_fast32_t pixCount, uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb mask percent
    uint_fast32_t RgbMaskPercent(uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb regions percent
    std::vector<uint_fast32_t> RgbRegionsPercent(uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray all bounds
    BoundsResult GrayAllBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray mask bounds
    BoundsResult GrayMaskBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // gray regions bounds
    std::vector<BoundsResult> GrayRegionsBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb all bounds
    BoundsResult RgbAllBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDepth, uint_fast32_t pixDiff, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb mask bounds
    BoundsResult RgbMaskBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth, uint_fast32_t pixDiff, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

    // rgb regions bounds
    std::vector<BoundsResult> RgbRegionsBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

}

#endif