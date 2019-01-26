#ifndef SRC_ASYNC_H_
#define SRC_ASYNC_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercentWorker : public Napi::AsyncWorker {
public:
    GrayAllPercentWorker(uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskPercentWorker : public Napi::AsyncWorker {
public:
    GrayMaskPercentWorker(uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsPercentWorker : public Napi::AsyncWorker {
public:
    GrayRegionsPercentWorker(uint_fast32_t pixCount, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixCount_;
    const int_fast32_t minDiff_;
    const uint_fast32_t regionsLen_;
    const std::vector<Region> &regionVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    std::vector<uint_fast32_t> percentResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBoundsWorker : public Napi::AsyncWorker {
public:
    GrayAllBoundsWorker(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBoundsWorker : public Napi::AsyncWorker {
public:
    GrayMaskBoundsWorker(uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBoundsWorker : public Napi::AsyncWorker {
public:
    GrayRegionsBoundsWorker(uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t minDiff_;
    const uint_fast32_t regionsLen_;
    const std::vector<Region> &regionVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    std::vector<BoundsResult> boundsResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercentWorker : public Napi::AsyncWorker {
public:
    RgbAllPercentWorker(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixDepth_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskPercentWorker : public Napi::AsyncWorker {
public:
    RgbMaskPercentWorker(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixDepth_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    uint_fast32_t percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsPercentWorker : public Napi::AsyncWorker {
public:
    RgbRegionsPercentWorker(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixDepth_;
    const uint_fast32_t pixCount_;
    const int_fast32_t minDiff_;
    const uint_fast32_t regionsLen_;
    const std::vector<Region> &regionVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    std::vector<uint_fast32_t> percentResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBoundsWorker : public Napi::AsyncWorker {
public:
    RgbAllBoundsWorker(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    const uint_fast32_t pixDepth_;
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskBoundsWorker : public Napi::AsyncWorker {
public:
    RgbMaskBoundsWorker(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixDepth_;
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    BoundsResult boundsResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBoundsWorker : public Napi::AsyncWorker {
public:
    RgbRegionsBoundsWorker(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    const uint_fast32_t pixDepth_;
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t minDiff_;
    const uint_fast32_t regionsLen_;
    const std::vector<Region> &regionVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;
    std::vector<BoundsResult> boundsResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif