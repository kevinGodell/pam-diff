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
    // in
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    PercentResult percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskPercentWorker : public Napi::AsyncWorker {
public:
    GrayMaskPercentWorker(uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    PercentResult percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsPercentWorker : public Napi::AsyncWorker {
public:
    GrayRegionsPercentWorker(uint_fast32_t pixCount, int_fast32_t minDiff, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t pixCount_;
    const int_fast32_t minDiff_;
    const std::vector<Region> &regionVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    uint_fast32_t flaggedCount_;
    std::vector<PercentResult> percentResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBoundsWorker : public Napi::AsyncWorker {
public:
    GrayAllBoundsWorker(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    BoundsResult boundsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBoundsWorker : public Napi::AsyncWorker {
public:
    GrayMaskBoundsWorker(uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    BoundsResult boundsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBoundsWorker : public Napi::AsyncWorker {
public:
    GrayRegionsBoundsWorker(uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, const std::vector<Region> &regionVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t minDiff_;
    const std::vector<Region> &regionVec_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    uint_fast32_t flaggedCount_;
    std::vector<BoundsResult> boundsResultVec_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercentWorker : public Napi::AsyncWorker {
public:
    RgbAllPercentWorker(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t pixDepth_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    PercentResult percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskPercentWorker : public Napi::AsyncWorker {
public:
    RgbMaskPercentWorker(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
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

    // out
    PercentResult percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsPercentWorker : public Napi::AsyncWorker {
public:
    RgbRegionsPercentWorker(uint_fast32_t pixDepth, uint_fast32_t pixCount, int_fast32_t minDiff, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t pixDepth_;
    const uint_fast32_t pixCount_;
    const int_fast32_t minDiff_;
    const std::vector<Region> &regionVec_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    uint_fast32_t flaggedCount_;
    std::vector<PercentResult> percentResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBoundsWorker : public Napi::AsyncWorker {
public:
    RgbAllBoundsWorker(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const uint_fast32_t pixDepth_;
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    BoundsResult boundsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbMaskBoundsWorker : public Napi::AsyncWorker {
public:
    RgbMaskBoundsWorker(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t pixDepth_;
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    BoundsResult boundsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionsBoundsWorker : public Napi::AsyncWorker {
public:
    RgbRegionsBoundsWorker(uint_fast32_t pixDepth, uint_fast32_t width, uint_fast32_t height, int_fast32_t minDiff, const std::vector<Region> &regionVec, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const uint_fast32_t pixDepth_;
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t minDiff_;
    const std::vector<Region> &regionVec_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    uint_fast32_t flaggedCount_;
    std::vector<BoundsResult> boundsResultVec_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllBlobsWorker : public Napi::AsyncWorker {
public:
    GrayAllBlobsWorker(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, int_fast32_t pixDiff, uint_fast32_t diffsPerc, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const uint_fast32_t pixCount_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    BlobsResult blobsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayMaskBlobsWorker : public Napi::AsyncWorker {
public:
    GrayMaskBlobsWorker(uint_fast32_t width, uint_fast32_t height, int_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const int_fast32_t pixDiff_;
    const uint_fast32_t diffsPerc_;
    const uint_fast32_t bitsetCount_;
    const std::vector<bool> &bitsetVec_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;
    const size_t buf1Size_;

    // out
    BlobsResult blobsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif