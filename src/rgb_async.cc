#include "rgb_async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentWorker::RgbAllPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResult_(PercentResult{"all", 0, false}) {
}

void RgbAllPercentWorker::Execute() {
    //RgbAllPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->percentResult_);
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

RgbRegionPercentWorker::RgbRegionPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResult_(PercentResult{"mask", 0, false}) {
}

void RgbRegionPercentWorker::Execute() {
    //RgbRegionPercent(this->pixDepth_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->percentResult_);
}

void RgbRegionPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->percentResult_.flagged) {
        ToJs(env, this->percentResult_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentWorker::RgbRegionsPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const uint_fast32_t minDiff, const std::vector<Region> &regionVec, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), pixCount_(pixCount), minDiff_(minDiff), regionVec_(regionVec), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), percentResultVec_(std::vector<PercentResult>(regionVec.size())) {
}

void RgbRegionsPercentWorker::Execute() {
    this->flaggedCount_ = RgbRegionsPercent(this->pixDepth_, this->pixCount_, this->minDiff_, this->regionVec_, this->buf0_, this->buf1_, this->percentResultVec_);
}

void RgbRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->percentResultVec_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsWorker::RgbAllBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResult_(BoundsResult{"all", width - 1, 0, height - 1, 0, 0, false}), pixels_(nullptr) {
}

void RgbAllBoundsWorker::Execute() {
    //RgbAllBounds(this->pixDepth_, this->width_, this->height_, this->pixCount_, this->pixDiff_, this->diffsPerc_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawRgb(this->boundsResult_, this->width_, this->pixDepth_, this->pixels_);
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

RgbRegionBoundsWorker::RgbRegionBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResult_(BoundsResult{"mask", width - 1, 0, height - 1, 0, 0, false}), pixels_(nullptr) {
}

void RgbRegionBoundsWorker::Execute() {
    //RgbRegionBounds(this->pixDepth_, this->width_, this->height_, this->pixDiff_, this->diffsPerc_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_, this->boundsResult_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawRgb(this->boundsResult_, this->width_, this->pixDepth_, this->pixels_);
    }
}

void RgbRegionBoundsWorker::OnOK() {
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

RgbRegionsBoundsWorker::RgbRegionsBoundsWorker(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t minDiff, const std::vector<Region> &regionVec, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), width_(width), height_(height), minDiff_(minDiff), regionVec_(regionVec), draw_(draw), buf0_(napiBuf0.Data()), buf1_(napiBuf1.Data()), buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)), buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)), buf1Size_(napiBuf1.Length()), boundsResultVec_(std::vector<BoundsResult>(regionVec.size(), BoundsResult{std::string(), width - 1, 0, height - 1, 0, 0, false})), pixels_(nullptr) {
}

void RgbRegionsBoundsWorker::Execute() {
    //this->flaggedCount_ = RgbRegionsBounds(this->pixDepth_, this->width_, this->height_, this->minDiff_, this->regionVec_, this->buf0_, this->buf1_, this->boundsResultVec_);
    if (this->flaggedCount_ > 0 && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->buf1Size_]();
        std::copy(this->buf1_, this->buf1_ + this->buf1Size_, this->pixels_);
        DrawRgb(this->boundsResultVec_, this->width_, this->pixDepth_, this->pixels_);
    }
}

void RgbRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->flaggedCount_ > 0) {
        ToJs(env, this->boundsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->buf1Size_, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////