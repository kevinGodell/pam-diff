#include "gray_async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentWorker::GrayAllPercentWorker(const Dimensions &dimensions, const All &all, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          all_(all),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          percentResult_(PercentResult{"all", 0, false}) {
}

void GrayAllPercentWorker::Execute() {
    GrayAllPercent(this->dimensions_, this->all_, this->buf0_, this->buf1_, this->percentResult_);
}

void GrayAllPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->percentResult_.flagged) {
        ToJs(env, this->percentResult_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionPercentWorker::GrayRegionPercentWorker(const Dimensions &dimensions, const Region &region, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
        dimensions_(dimensions),
        region_(region),
        buf0_(napiBuf0.Data()),
        buf1_(napiBuf1.Data()),
        buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
        buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
        percentResult_(PercentResult{region.name, 0, false}) {
}

void GrayRegionPercentWorker::Execute() {
    GrayRegionPercent(this->dimensions_, this->region_, this->buf0_, this->buf1_, this->percentResult_);
}

void GrayRegionPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->percentResult_.flagged) {
        ToJs(env, this->percentResult_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentWorker::GrayRegionsPercentWorker(const Dimensions &dimensions, const Regions &regions, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
        dimensions_(dimensions),
        regions_(regions),
        buf0_(napiBuf0.Data()),
        buf1_(napiBuf1.Data()),
        buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
        buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
        percentResultVec_(std::vector<PercentResult>(regions.regions.size())) {
}

void GrayRegionsPercentWorker::Execute() {
    this->flaggedCount_ = GrayRegionsPercent(this->dimensions_, this->regions_, this->buf0_, this->buf1_, this->percentResultVec_);
}

void GrayRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->percentResultVec_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsWorker::GrayAllBoundsWorker(const Dimensions &dimensions, const All &all, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
        dimensions_(dimensions),
        all_(all),
        draw_(draw),
        buf0_(napiBuf0.Data()),
        buf1_(napiBuf1.Data()),
        buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
        buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
        boundsResult_(BoundsResult{"all", dimensions.width - 1, 0, dimensions.height - 1, 0, 0, false}),
        pixels_(nullptr) {
}

void GrayAllBoundsWorker::Execute() {
    GrayAllBounds(this->dimensions_, this->all_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawGray(this->boundsResult_, this->dimensions_.width, this->pixels_);
    }
}

void GrayAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionBoundsWorker::GrayRegionBoundsWorker(const Dimensions &dimensions, const Region &region, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
        dimensions_(dimensions),
        region_(region),
        draw_(draw),
        buf0_(napiBuf0.Data()),
        buf1_(napiBuf1.Data()),
        buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
        buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
        boundsResult_(BoundsResult{region.name, region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY, 0, false}),
        pixels_(nullptr) {
}

void GrayRegionBoundsWorker::Execute() {
    GrayRegionBounds(this->dimensions_, this->region_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawGray(this->boundsResult_, this->dimensions_.width, this->pixels_);
    }
}

void GrayRegionBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsWorker::GrayRegionsBoundsWorker(const Dimensions &dimensions, const Regions &regions, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
        dimensions_(dimensions),
        regions_(regions),
        draw_(draw),
        buf0_(napiBuf0.Data()),
        buf1_(napiBuf1.Data()),
        buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
        buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
        boundsResultVec_(std::vector<BoundsResult>(regions.regions.size(), BoundsResult{std::string(), regions.bounds.maxX, regions.bounds.minX, regions.bounds.maxY, regions.bounds.minY, 0, false})),
        pixels_(nullptr) {
}

void GrayRegionsBoundsWorker::Execute() {
    this->flaggedCount_ = GrayRegionsBounds(this->dimensions_, this->regions_, this->buf0_, this->buf1_, this->boundsResultVec_);
    if (this->flaggedCount_ > 0 && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawGray(this->boundsResultVec_, this->dimensions_.width, this->pixels_);
    }
}

void GrayRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->boundsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
GrayAllBlobsWorker::GrayAllBlobsWorker(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), blobsResult_(BlobsResult{"all", width - 1, 0, height - 1, 0, 0, false, std::vector<Blob>()}), pixels_(nullptr) {
}

void GrayAllBlobsWorker::Execute() {
    //GrayAllBlobs(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->blobsResult_);
    if (this->blobsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawGray(this->blobsResult_, this->width_, this->pixels_);
    }
}

void GrayAllBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->blobsResult_.flagged) {
        ToJs(env, this->blobsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionBlobsWorker::GrayRegionBlobsWorker(const uint_fast32_t width, const uint_fast32_t height, const Bounds &bounds, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), bounds_(bounds), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), blobsResult_(BlobsResult{"mask", bounds.maxX, bounds.minX, bounds.maxY, bounds.minY, 0, false, std::vector<Blob>()}), pixels_(nullptr) {
}

void GrayRegionBlobsWorker::Execute() {
    //GrayRegionBlobs(this->width_, this->height_, this->bounds_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->blobsResult_);
    if (this->blobsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawGray(this->blobsResult_, this->width_, this->pixels_);
    }
}

void GrayRegionBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->blobsResult_.flagged) {
        ToJs(env, this->blobsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////