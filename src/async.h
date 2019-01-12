#ifndef SRC_ASYNC_H_
#define SRC_ASYNC_H_

#include "engine.h"
#include <stdint.h>
#include <vector>
#include <napi.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercent : public Napi::AsyncWorker {
    public:
        GrayAllPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast32_t pixCount_;
        const uint_fast8_t pixDiff_;
        const uint_fast8_t diffsPerc_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast8_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskPercent : public Napi::AsyncWorker {
    public:
        GrayMaskPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast32_t pixCount_;
        const uint_fast8_t pixDiff_;
        const uint_fast8_t diffsPerc_;
        const uint_fast32_t bitsetCount_;
        const std::vector<bool> &bitsetVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast8_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsPercent : public Napi::AsyncWorker {
    public:
        GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast32_t pixCount_;
        const uint_fast8_t minDiff_;
        const uint_fast8_t regionsLen_;
        const std::vector<Engine::Region> &regionsVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        std::vector<uint_fast8_t> resultsVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercent : public Napi::AsyncWorker {
    public:
        RgbAllPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast32_t pixCount_;
        const uint_fast8_t pixDepth_;
        const uint_fast8_t pixDiff_;
        const uint_fast8_t diffsPerc_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast8_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskPercent : public Napi::AsyncWorker {
    public:
        RgbMaskPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast32_t pixCount_;
        const uint_fast8_t pixDepth_;
        const uint_fast8_t pixDiff_;
        const uint_fast8_t diffsPerc_;
        const uint_fast32_t bitsetCount_;
        const std::vector<bool> &bitsetVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast8_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsPercent : public Napi::AsyncWorker {
    public:
        RgbRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast32_t pixCount_;
        const uint_fast8_t pixDepth_;
        const uint_fast8_t minDiff_;
        const uint_fast8_t regionsLen_;
        const std::vector<Engine::Region> &regionsVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        std::vector<uint_fast8_t> resultsVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBounds : public Napi::AsyncWorker {
    public:
        GrayAllBounds(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast16_t width_;
        const uint_fast16_t height_;
        const uint_fast32_t pixCount_;
        const uint_fast8_t pixDiff_;
        const uint_fast8_t diffsPerc_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        Engine::BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBounds : public Napi::AsyncWorker {
    public:
        GrayMaskBounds(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast16_t width_;
        const uint_fast16_t height_;
        const uint_fast32_t pixCount_;
        const uint_fast8_t pixDiff_;
        const uint_fast8_t diffsPerc_;
        const uint_fast32_t bitsetCount_;
        const std::vector<bool> &bitsetVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        Engine::BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*class GrayRegionsBounds : public Napi::AsyncWorker {
    public:
        GrayRegionsBounds(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute();
        void OnOK();
    private:
        const uint_fast16_t width_;
        const uint_fast16_t height_;
        const uint_fast32_t pixCount_;
        const uint_fast8_t minDiff_;
        const uint_fast8_t regionsLen_;
        const std::vector<Engine::Region> &regionsVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        std::vector<uint_fast8_t> resultsVec_;
};*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif