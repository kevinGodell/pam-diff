#include "async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentWorker::GrayAllPercentWorker(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResult_(PercentResult{"all", 0, false}) {
}

void GrayAllPercentWorker::Execute() {
    GrayAllPercent(this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->percentResult_);
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

GrayMaskPercentWorker::GrayMaskPercentWorker(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResult_(PercentResult{"mask", 0, false}) {
}

void GrayMaskPercentWorker::Execute() {
    GrayMaskPercent(this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->percentResult_);
}

void GrayMaskPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->percentResult_.flagged) {
        ToJs(env, this->percentResult_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentWorker::GrayRegionsPercentWorker(const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResultVec_(std::vector<PercentResult>(regionsLen)) {
}

void GrayRegionsPercentWorker::Execute() {
    this->flaggedCount_ = GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_, this->percentResultVec_);
}

void GrayRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->regionsLen_, this->percentResultVec_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsWorker::GrayAllBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResult_(BoundsResult{"all", width - 1, 0, height - 1, 0, 0, false}), pixels_(nullptr) {
}

void GrayAllBoundsWorker::Execute() {
    GrayAllBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawGrayBounds(this->boundsResult_, this->width_, this->pixels_);
    }
}

void GrayAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBoundsWorker::GrayMaskBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResult_(BoundsResult{"mask", width - 1, 0, height - 1, 0, 0, false}), pixels_(nullptr) {
}

void GrayMaskBoundsWorker::Execute() {
    GrayMaskBounds(this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawGrayBounds(this->boundsResult_, this->width_, this->pixels_);
    }
}

void GrayMaskBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsWorker::GrayRegionsBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResultVec_(std::vector<BoundsResult>(regionsLen, BoundsResult{std::string(), width - 1, 0, height - 1, 0, 0, false})), pixels_(nullptr) {
}

void GrayRegionsBoundsWorker::Execute() {
    this->flaggedCount_ = GrayRegionsBounds(this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_, this->boundsResultVec_);
    if (this->flaggedCount_ > 0 && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawGrayBounds(this->regionsLen_, this->boundsResultVec_, this->width_, this->pixels_);
    }
}

void GrayRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->regionsLen_, this->boundsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentWorker::RgbAllPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResult_(PercentResult{"all", 0, false}) {
}

void RgbAllPercentWorker::Execute() {
    RgbAllPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->percentResult_);
}

void RgbAllPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->percentResult_.flagged) {
        ToJs(env, this->percentResult_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskPercentWorker::RgbMaskPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResult_(PercentResult{"mask", 0, false}) {
}

void RgbMaskPercentWorker::Execute() {
    RgbMaskPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->percentResult_);
}

void RgbMaskPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->percentResult_.flagged) {
        ToJs(env, this->percentResult_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentWorker::RgbRegionsPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResultVec_(std::vector<PercentResult>(regionsLen)) {
}

void RgbRegionsPercentWorker::Execute() {
    this->flaggedCount_ = RgbRegionsPercent(this->pixDepth_, this->pixCount_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_, this->percentResultVec_);
}

void RgbRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->regionsLen_, this->percentResultVec_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsWorker::RgbAllBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResult_(BoundsResult{"all", width - 1, 0, height - 1, 0, 0, false}), pixels_(nullptr) {
}

void RgbAllBoundsWorker::Execute() {
    RgbAllBounds(this->pixDepth_, this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawRgbBounds(this->boundsResult_, this->width_, this->pixDepth_, this->pixels_);
    }
}

void RgbAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskBoundsWorker::RgbMaskBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResult_(BoundsResult{"mask", width - 1, 0, height - 1, 0, 0, false}), pixels_(nullptr) {
}

void RgbMaskBoundsWorker::Execute() {
    RgbMaskBounds(this->pixDepth_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawRgbBounds(this->boundsResult_, this->width_, this->pixDepth_, this->pixels_);
    }
}

void RgbMaskBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsWorker::RgbRegionsBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResultVec_(std::vector<BoundsResult>(regionsLen, BoundsResult{std::string(), width - 1, 0, height - 1, 0, 0, false})), pixels_(nullptr) {
}

void RgbRegionsBoundsWorker::Execute() {
    this->flaggedCount_ = RgbRegionsBounds(this->pixDepth_, this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_, this->boundsResultVec_);
    if (this->flaggedCount_ > 0 && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawRgbBounds(this->regionsLen_, this->boundsResultVec_, this->width_, this->pixDepth_, this->pixels_);
    }
}

void RgbRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->regionsLen_, this->boundsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBlobsWorker::GrayAllBlobsWorker(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), blobsResult_(BlobsResult{"all", width - 1, 0, height - 1, 0, 0, false, std::vector<Blob>()}), pixels_(nullptr) {
}

void GrayAllBlobsWorker::Execute() {
    GrayAllBlobs(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->blobsResult_);
    if (this->blobsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawGrayBounds(this->blobsResult_, this->width_, this->pixels_);
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

GrayMaskBlobsWorker::GrayMaskBlobsWorker(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), blobsResult_(BlobsResult{"mask", width - 1, 0, height - 1, 0, 0, false, std::vector<Blob>()}), pixels_(nullptr) {
}

void GrayMaskBlobsWorker::Execute() {
    GrayMaskBlobs(this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->blobsResult_);
    if (this->blobsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawGrayBounds(this->blobsResult_, this->width_, this->pixels_);
    }
}

void GrayMaskBlobsWorker::OnOK() {
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