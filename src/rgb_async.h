#ifndef SRC_RGB_ASYNC_H_
#define SRC_RGB_ASYNC_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllPercentWorker : public Napi::AsyncWorker {
public:
    RgbAllPercentWorker(const Config &config, const All &all, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Config &config_;
    const All &all_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    PercentResult percentResult_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionPercentWorker : public Napi::AsyncWorker {
public:
    RgbRegionPercentWorker(const Config &config, const Region &region, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Config &config_;
    const Region &region_;
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
    RgbRegionsPercentWorker(const Config &config, const Regions &regions, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Config &config_;
    const Regions &regions_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1ref_;

    // out
    std::vector<PercentResult> percentResultVec_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBoundsWorker : public Napi::AsyncWorker {
public:
    RgbAllBoundsWorker(const Config &config, const All &all, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const Config &config_;
    const All &all_;
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

class RgbRegionBoundsWorker : public Napi::AsyncWorker {
public:
    RgbRegionBoundsWorker(const Config &config, const Region &region, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Config &config_;
    const Region &region_;
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

class RgbRegionsBoundsWorker : public Napi::AsyncWorker {
public:
    RgbRegionsBoundsWorker(const Config &config, const Regions &regions, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const Config &config_;
    const Regions &regions_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;

    // out
    std::vector<BoundsResult> boundsResultVec_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbAllBlobsWorker : public Napi::AsyncWorker {
public:
    RgbAllBlobsWorker(const Config &config, const All &all, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const Config &config_;
    const All &all_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;

    // out
    BlobsResult blobsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RgbRegionBlobsWorker : public Napi::AsyncWorker {
public:
    RgbRegionBlobsWorker(const Config &config, const Region &region, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const Config &config_;
    const Region &region_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;

    // out
    BlobsResult blobsResult_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class RgbRegionsBlobsWorker : public Napi::AsyncWorker {
public:
    RgbRegionsBlobsWorker(const Config &config, const Regions &regions, bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

protected:
    // in
    const Config &config_;
    const Regions &regions_;
    const bool draw_;
    const uint_fast8_t *buf0_;
    const uint_fast8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf0Ref_;
    const Napi::Reference<Napi::Buffer<uint_fast8_t>> buf1Ref_;

    // out
    std::vector<BlobsResult> blobsResultVec_;
    uint_fast8_t *pixels_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif