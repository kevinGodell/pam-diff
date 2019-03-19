#ifndef SRC_ASYNC_H_
#define SRC_ASYNC_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayAllPercentWorker : public Napi::AsyncWorker {
public:
    GrayAllPercentWorker(const Dimensions &dimensions, const All &all, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Dimensions dimensions_;
    const All all_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    PercentResult percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionPercentWorker : public Napi::AsyncWorker {
public:
    GrayRegionPercentWorker(const Dimensions &dimensions, const Region &region, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Dimensions dimensions_;
    const Region region_;
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
    GrayRegionsPercentWorker(const Dimensions &dimensions, const Regions &regions, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Dimensions dimensions_;
    const Regions regions_;
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
    GrayAllBoundsWorker(const Dimensions &dimensions, const All &all, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const Dimensions dimensions_;
    const All all_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;

    // out
    BoundsResult boundsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionBoundsWorker : public Napi::AsyncWorker {
public:
    GrayRegionBoundsWorker(const Dimensions &dimensions, const Region &region, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Dimensions dimensions_;
    const Region region_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;

    // out
    BoundsResult boundsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GrayRegionsBoundsWorker : public Napi::AsyncWorker {
public:
    GrayRegionsBoundsWorker(const Dimensions &dimensions, const Regions &regions, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Dimensions dimensions_;
    const Regions regions_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;

    // out
    uint_fast32_t flaggedCount_;
    std::vector<BoundsResult> boundsResultVec_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class GrayAllBlobsWorker : public Napi::AsyncWorker {
public:
    GrayAllBlobsWorker(uint_fast32_t width, uint_fast32_t height, uint_fast32_t pixCount, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const uint_fast32_t pixCount_;
    const uint_fast32_t pixDiff_;
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

class GrayRegionBlobsWorker : public Napi::AsyncWorker {
public:
    GrayRegionBlobsWorker(uint_fast32_t width, uint_fast32_t height, const Bounds &bounds, uint_fast32_t pixDiff, uint_fast32_t diffsPerc, uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const uint_fast32_t width_;
    const uint_fast32_t height_;
    const Bounds bounds_;
    const uint_fast32_t pixDiff_;
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
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif