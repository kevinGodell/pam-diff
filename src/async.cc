#include "async.h"
#include "engine.h"
#include "results.h"
#include <cstdint>
#include <vector>
#include <napi.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercentWorker::GrayAllPercentWorker(const std::string &target, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void GrayAllPercentWorker::Execute() {
    this->percentResult_ = GrayAllPercent(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void GrayAllPercentWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercentWorker::GrayMaskPercentWorker(const std::string &target, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
}

void GrayMaskPercentWorker::Execute() {
    this->percentResult_ = GrayMaskPercent(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void GrayMaskPercentWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercentWorker::GrayRegionsPercentWorker(const uint_fast32_t pixCount, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(buf0), buf1_(buf1) {
}

void GrayRegionsPercentWorker::Execute() {
    this->percentResultVec_ = GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_);
}

void GrayRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, this->percentResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentWorker::RgbAllPercentWorker(const std::string &target, const uint_fast32_t pixCount, const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), pixCount_(pixCount), pixDepth_(pixDepth), bufLen_(bufLen), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void RgbAllPercentWorker::Execute() {
    this->percentResult_ = RgbAllPercent(this->pixCount_, this->pixDepth_, this->bufLen_, this->pixDiff_, this->buf0_, this->buf1_);
}

void RgbAllPercentWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskPercentWorker::RgbMaskPercentWorker(const std::string &target, const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), pixDepth_(pixDepth), bufLen_(bufLen), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
}

void RgbMaskPercentWorker::Execute() {
    this->percentResult_ = RgbMaskPercent(this->pixDepth_, this->bufLen_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void RgbMaskPercentWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercentWorker::RgbRegionsPercentWorker(const uint_fast32_t pixDepth, const uint_fast32_t bufLen, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), pixDepth_(pixDepth), bufLen_(bufLen), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(buf0), buf1_(buf1) {
}

void RgbRegionsPercentWorker::Execute() {
    this->percentResultVec_ = RgbRegionsPercent(this->pixDepth_, this->bufLen_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_);
}

void RgbRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, this->percentResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBoundsWorker::GrayAllBoundsWorker(const std::string &target, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void GrayAllBoundsWorker::Execute() {
    this->boundsResult_ = GrayAllBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void GrayAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBoundsWorker::GrayMaskBoundsWorker(const std::string &target, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), width_(width), height_(height), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
}

void GrayMaskBoundsWorker::Execute() {
    this->boundsResult_ = GrayMaskBounds(this->width_, this->height_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void GrayMaskBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsBoundsWorker::GrayRegionsBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(buf0), buf1_(buf1) {
}

void GrayRegionsBoundsWorker::Execute() {
    this->boundsResultVec_ = GrayRegionsBounds(this->width_, this->height_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_);
}

void GrayRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, this->boundsResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsWorker::RgbAllBoundsWorker(const std::string &target, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDepth, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), width_(width), height_(height), pixCount_(pixCount), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void RgbAllBoundsWorker::Execute() {
    this->boundsResult_ = RgbAllBounds(this->width_, this->height_, this->pixCount_, this->pixDepth_, this->pixDiff_, this->buf0_, this->buf1_);
}

void RgbAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskBoundsWorker::RgbMaskBoundsWorker(const std::string &target, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), target_(target), width_(width), height_(height), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
}

void RgbMaskBoundsWorker::Execute() {
    this->boundsResult_ = RgbMaskBounds(this->width_, this->height_, this->pixDepth_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void RgbMaskBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->target_, this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsWorker::RgbRegionsBoundsWorker(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDepth, const uint_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb), width_(width), height_(height), pixDepth_(pixDepth), minDiff_(minDiff), regionsLen_(regionsLen), regionVec_(regionVec), buf0_(buf0), buf1_(buf1) {
}

void RgbRegionsBoundsWorker::Execute() {
    this->boundsResultVec_ = RgbRegionsBounds(this->width_, this->height_, this->pixDepth_, this->minDiff_, this->regionsLen_, this->regionVec_, this->buf0_, this->buf1_);
}

void RgbRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = ToJs(env, this->regionsLen_, this->regionVec_, this->boundsResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////