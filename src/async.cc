#include "async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentWorker::GrayAllPercentWorker(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void GrayAllPercentWorker::Execute() {
    this->percentResult_ = GrayAllPercent(this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_);
}

void GrayAllPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    const Napi::Array resultsJs = ToJs(env, "all", this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercentWorker::GrayMaskPercentWorker(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void GrayMaskPercentWorker::Execute() {
    this->percentResult_ = GrayMaskPercent(this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void GrayMaskPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    const Napi::Array resultsJs = ToJs(env, "mask", this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentWorker::GrayRegionsPercentWorker(const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void GrayRegionsPercentWorker::Execute() {
    this->percentResultVec_ = GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_);
}

void GrayRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    const Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, this->percentResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsWorker::GrayAllBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()) {
}

void GrayAllBoundsWorker::Execute() {
    GrayAllBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = std::vector<uint_fast8_t>{this->buf1_, this->buf1_ + this->buf1Size_};
        DrawGrayBounds(this->boundsResult_, this->width_, this->pixels_.data());
    }
}

void GrayAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, this->pixels_.data(), this->buf1Size_);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs, env.Null()});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBoundsWorker::GrayMaskBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()) {
}

void GrayMaskBoundsWorker::Execute() {
    GrayMaskBounds(this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = std::vector<uint_fast8_t>{this->buf1_, this->buf1_ + this->buf1Size_};
        DrawGrayBounds(this->boundsResult_, this->width_, this->pixels_.data());
    }
}

void GrayMaskBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, this->pixels_.data(), this->buf1Size_);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs, env.Null()});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsWorker::GrayRegionsBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()) {
}

void GrayRegionsBoundsWorker::Execute() {
    this->flaggedCount_ = GrayRegionsBounds(this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_, this->boundsResultVec_);
    if (this->flaggedCount_ > 0 && this->draw_) {
        this->pixels_ = std::vector<uint_fast8_t>{this->buf1_, this->buf1_ + this->buf1Size_};
        DrawGrayBounds(this->regionsLen_, this->boundsResultVec_, this->width_, this->pixels_.data());
    }
}

void GrayRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->regionsLen_, this->boundsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, this->pixels_.data(), this->buf1Size_);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs, env.Null()});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentWorker::RgbAllPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbAllPercentWorker::Execute() {
    this->percentResult_ = RgbAllPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_);
}

void RgbAllPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    const Napi::Array resultsJs = ToJs(env, "all", this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskPercentWorker::RgbMaskPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbMaskPercentWorker::Execute() {
    this->percentResult_ = RgbMaskPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void RgbMaskPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    const Napi::Array resultsJs = ToJs(env, "mask", this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentWorker::RgbRegionsPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbRegionsPercentWorker::Execute() {
    this->percentResultVec_ = RgbRegionsPercent(this->pixDepth_, this->pixCount_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_);
}

void RgbRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    const Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, this->percentResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsWorker::RgbAllBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()) {
}

void RgbAllBoundsWorker::Execute() {
    RgbAllBounds(this->pixDepth_, this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = std::vector<uint_fast8_t>{this->buf1_, this->buf1_ + this->buf1Size_};
        DrawRgbBounds(this->boundsResult_, this->width_, this->pixDepth_, this->pixels_.data());
    }
}

void RgbAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, this->pixels_.data(), this->buf1Size_);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs, env.Null()});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskBoundsWorker::RgbMaskBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()) {
}

void RgbMaskBoundsWorker::Execute() {
    RgbMaskBounds(this->pixDepth_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = std::vector<uint_fast8_t>{this->buf1_, this->buf1_ + this->buf1Size_};
        DrawRgbBounds(this->boundsResult_, this->width_, this->pixDepth_, this->pixels_.data());
    }
}

void RgbMaskBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, this->pixels_.data(), this->buf1Size_);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs, env.Null()});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsWorker::RgbRegionsBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()) {
}

void RgbRegionsBoundsWorker::Execute() {
    this->flaggedCount_ = RgbRegionsBounds(this->pixDepth_, this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_, this->boundsResultVec_);
    if (this->flaggedCount_ > 0 && this->draw_) {
        this->pixels_ = std::vector<uint_fast8_t>{this->buf1_, this->buf1_ + this->buf1Size_};
        DrawRgbBounds(this->regionsLen_, this->boundsResultVec_, this->width_, this->pixDepth_, this->pixels_.data());
    }
}

void RgbRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->regionsLen_, this->boundsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::Copy(env, this->pixels_.data(), this->buf1Size_);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs, env.Null()});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////