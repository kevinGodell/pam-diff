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
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void GrayAllPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, this->target_, this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercent::RgbAllPercent(const std::string target, const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : GrayAllPercent(target, pixCount, pixDiff, diffsPerc, buf0, buf1, cb), pixDepth_(pixDepth), bufLen_(bufLen) {
}

void RgbAllPercent::Execute() {
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDepth_, this->bufLen_, this->pixDiff_, this->buf0_, this->buf1_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskPercent::GrayMaskPercent(const std::string target, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : GrayAllPercent(target, pixCount, pixDiff, diffsPerc, buf0, buf1, cb), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec) {
}

void GrayMaskPercent::Execute() {
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbMaskPercent::RgbMaskPercent(const std::string target, const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast32_t bufLen, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : GrayMaskPercent(target, pixCount, pixDiff, diffsPerc, bitsetCount, bitsetVec, buf0, buf1, cb), pixDepth_(pixDepth), bufLen_(bufLen) {
}

void RgbMaskPercent::Execute() {
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDepth_, this->bufLen_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayRegionsPercent::GrayRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
}

void GrayRegionsPercent::Execute() {
    this->resultsVec_ = Engine::MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

void GrayRegionsPercent::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsPercent::RgbRegionsPercent(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast32_t bufLen, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : GrayRegionsPercent(pixCount, minDiff, regionsLen, regionsVec, buf0, buf1, cb), pixDepth_(pixDepth), bufLen_(bufLen) {
}

void RgbRegionsPercent::Execute() {
    this->resultsVec_ = Engine::MeasureDiffs(this->pixCount_, this->pixDepth_, this->bufLen_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayAllBounds::GrayAllBounds(const std::string target, const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : Napi::AsyncWorker(cb), target_(target), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
}

void GrayAllBounds::Execute() {
    this->boundsResult_ = Engine::MeasureDiffs(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void GrayAllBounds::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, this->target_, this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrayMaskBounds::GrayMaskBounds(const std::string target, const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb)
    : GrayAllBounds(target, width, height, pixCount, pixDiff, diffsPerc, buf0, buf1, cb), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec) {
}

void GrayMaskBounds::Execute() {
    this->boundsResult_ = Engine::MeasureDiffs(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*GrayRegionsBounds::GrayRegionsBounds(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, const std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
}

void GrayRegionsBounds::Execute() {
    //this->resultsVec_ = Engine::MeasureDiffs(this->width_, this->height_, this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

void GrayRegionsBounds::OnOK() {
    //const Napi::Env env = Env();
    //Napi::HandleScope scope(env);
    //Napi::Array resultsJs = Results::ConvertToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_);
    //Callback().Call({env.Null(), resultsJs});
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////