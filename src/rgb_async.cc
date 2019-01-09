#include "rgb_async.h"
#include "diff.h"
#include "helper.h"
#include <stdint.h>
#include <napi.h>

RgbDiffAllWorker::RgbDiffAllWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1), percentResult_(0) {}

void RgbDiffAllWorker::Execute() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->buf0_, this->buf1_);
}

void RgbDiffAllWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    allResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////

RgbDiffMaskWorker::RgbDiffMaskWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1), percentResult_(0) {}

void RgbDiffMaskWorker::Execute() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void RgbDiffMaskWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    maskResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////

RgbDiffRegionsWorker::RgbDiffRegionsWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {}

void RgbDiffRegionsWorker::Execute() {
    this->resultsVec_ = std::vector<uint_fast32_t> (this->regionsLen_, 0);
    MeasureDiffs(this->pixCount_, this->pixDepth_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_, this->resultsVec_);
}

void RgbDiffRegionsWorker::OnOK() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    regionsResultsToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}