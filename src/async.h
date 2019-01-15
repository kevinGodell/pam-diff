#ifndef SRC_ASYNC_H_
#define SRC_ASYNC_H_

#include "engine.h"
#include <cstdint>
#include <vector>
#include <napi.h>

/*

class naming scheme = TypeTargetResponse

GrayAllPercent = gray pixels, target all, respond with percent

"Mask" sub classes add bitsetCount_ and bitset_

"Rgb" sub classes add pixDepth_ and bufLen_

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercent : public Napi::AsyncWorker {
    public:
        GrayAllPercent(const std::string &target, uint_fast32_t pixCount, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const std::string target_;
        const uint_fast32_t pixCount_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskPercent : public Napi::AsyncWorker {
    public:
        GrayMaskPercent(const std::string &target, uint_fast32_t pixCount, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const std::string target_;
        const uint_fast32_t pixCount_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast32_t bitsetCount_;
        const std::vector<bool> &bitsetVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsPercent : public Napi::AsyncWorker {
    public:
        GrayRegionsPercent(uint_fast32_t pixCount, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const uint_fast32_t pixCount_;
        const uint_fast32_t minDiff_;
        const uint_fast32_t regionsLen_;
        const std::vector<Engine::Region> &regionVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        std::vector<uint_fast32_t> percentResultVec_;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercent : public Napi::AsyncWorker {
    public:
        RgbAllPercent(const std::string &target, uint_fast32_t pixCount, uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const std::string target_;
        const uint_fast32_t pixCount_;
        const uint_fast32_t pixDepth_;
        const uint_fast32_t bufLen_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskPercent : public Napi::AsyncWorker {
    public:
        RgbMaskPercent(const std::string &target, uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const std::string target_;
        const uint_fast32_t pixDepth_;
        const uint_fast32_t bufLen_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast32_t bitsetCount_;
        const std::vector<bool> &bitsetVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsPercent : public Napi::AsyncWorker {
    public:
        RgbRegionsPercent(uint_fast32_t pixDepth, uint_fast32_t bufLen, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const uint_fast32_t pixDepth_;
        const uint_fast32_t bufLen_;
        const uint_fast32_t minDiff_;
        const uint_fast32_t regionsLen_;
        const std::vector<Engine::Region> &regionVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        std::vector<uint_fast32_t> percentResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBounds : public Napi::AsyncWorker {
    public:
        GrayAllBounds(const std::string &target, uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    protected:
        const std::string target_;
        const uint_fast32_t width_;
        const uint_fast32_t height_;
        const uint_fast32_t pixCount_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        Engine::BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBounds : public Napi::AsyncWorker {
    public:
        GrayMaskBounds(const std::string &target, uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const std::string target_;
        const uint_fast32_t width_;
        const uint_fast32_t height_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast32_t bitsetCount_;
        const std::vector<bool> &bitsetVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        Engine::BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBounds : public Napi::AsyncWorker {
    public:
        GrayRegionsBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const uint_fast32_t width_;
        const uint_fast32_t height_;
        const uint_fast32_t minDiff_;
        const uint_fast32_t regionsLen_;
        const std::vector<Engine::Region> &regionVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        std::vector<Engine::BoundsResult> boundsResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBounds : public Napi::AsyncWorker {
    public:
        RgbAllBounds(const std::string &target, uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDepth, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    protected:
        const std::string target_;
        const uint_fast32_t width_;
        const uint_fast32_t height_;
        const uint_fast32_t pixCount_;
        const uint_fast32_t pixDepth_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        Engine::BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskBounds : public Napi::AsyncWorker {
    public:
        RgbMaskBounds(const std::string &target, uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const std::string target_;
        const uint_fast32_t width_;
        const uint_fast32_t height_;
        const uint_fast32_t pixDepth_;
        const uint_fast32_t pixDiff_;
        const uint_fast32_t diffsPerc_;
        const uint_fast32_t bitsetCount_;
        const std::vector<bool> &bitsetVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        Engine::BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBounds : public Napi::AsyncWorker {
    public:
        RgbRegionsBounds(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixDepth, uint_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Engine::Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
        void Execute() override;
        void OnOK() override;
    private:
        const uint_fast32_t width_;
        const uint_fast32_t height_;
        const uint_fast32_t pixDepth_;
        const uint_fast32_t minDiff_;
        const uint_fast32_t regionsLen_;
        const std::vector<Engine::Region> &regionVec_;
        const uint_fast8_t *buf0_;
        const uint_fast8_t *buf1_;
        std::vector<Engine::BoundsResult> boundsResultVec_;
};

#endif