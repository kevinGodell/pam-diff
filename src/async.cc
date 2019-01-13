#include "async.h"
#include "engine.h"
#include "results.h"
#include <stdint.h>
#include <vector>
#include <napi.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllPercent::GrayAllPercent(const std::string target, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), target_(target), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void GrayAllPercent::Execute() {
    this->percentResult_ = Engine::GrayAllPercent(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void GrayAllPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercent::GrayMaskPercent(const std::string target, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), target_(target), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
}

void GrayMaskPercent::Execute() {
    this->percentResult_ = Engine::GrayMaskPercent(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void GrayMaskPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercent::RgbAllPercent(const std::string target, const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), target_(target), pixCount_(pixCount), pixDepth_(pixDepth), bufLen_(bufLen), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void RgbAllPercent::Execute() {
    this->percentResult_ = Engine::RgbAllPercent(this->pixCount_, this->pixDepth_, this->bufLen_, this->pixDiff_, this->buf0_, this->buf1_);
}

void RgbAllPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//does not need pixCount
RgbMaskPercent::RgbMaskPercent(const std::string target, const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), target_(target), pixCount_(pixCount), pixDepth_(pixDepth), bufLen_(bufLen), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
}

void RgbMaskPercent::Execute() {
    this->percentResult_ = Engine::RgbMaskPercent(this->pixCount_, this->pixDepth_, this->bufLen_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void RgbMaskPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercent::GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
}

void GrayRegionsPercent::Execute() {
    this->percentResultVec_ = Engine::GrayRegionsPercent(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

void GrayRegionsPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->regionsLen_, this->regionsVec_, this->percentResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//does not need pixCount
RgbRegionsPercent::RgbRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast32_t bufLen, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), bufLen_(bufLen), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
}

void RgbRegionsPercent::Execute() {
    this->percentResultVec_ = Engine::RgbRegionsPercent(this->pixCount_, this->pixDepth_, this->bufLen_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

void RgbRegionsPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->regionsLen_, this->regionsVec_, this->percentResultVec_);
    Callback().Call({env.Null(), resultsJs});
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBounds::GrayAllBounds(const std::string target, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), target_(target), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void GrayAllBounds::Execute() {
    this->boundsResult_ = Engine::GrayAllBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void GrayAllBounds::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->target_, this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//does not need pixCount
GrayMaskBounds::GrayMaskBounds(const std::string target, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), target_(target), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
}

void GrayMaskBounds::Execute() {
    this->boundsResult_ = Engine::GrayMaskBounds(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void GrayMaskBounds::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->target_, this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//does not need pixCount
GrayRegionsBounds::GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
}

void GrayRegionsBounds::Execute() {
    this->boundsResultVec_ = Engine::GrayRegionsBounds(this->width_, this->height_, this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

void GrayRegionsBounds::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ToJs(env, this->regionsLen_, this->regionsVec_, this->boundsResultVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////