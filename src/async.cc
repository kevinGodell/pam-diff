#include "async.h"
#include "engine.h"
#include "results.h"
#include <stdint.h>
#include <vector>
#include <napi.h>

//gray all percent
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::GrayAllPercentExecute;
    this->OnOkPtr_ = &DiffWorker::GrayAllPercentOnOk;
}

//gray mask percent
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::GrayMaskPercentExecute;
    this->OnOkPtr_ = &DiffWorker::GrayMaskPercentOnOk;
}

//gray regions percent
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::GrayRegionsPercentExecute;
    this->OnOkPtr_ = &DiffWorker::GrayRegionsPercentOnOk;
}

//rgb all percent
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::RgbAllPercentExecute;
    this->OnOkPtr_ = &DiffWorker::RgbAllPercentOnOk;
}

//rgb mask percent
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::RgbMaskPercentExecute;
    this->OnOkPtr_ = &DiffWorker::RgbMaskPercentOnOk;
}

//rgb regions percent
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::RgbRegionsPercentExecute;
    this->OnOkPtr_ = &DiffWorker::RgbRegionsPercentOnOk;
}

void DiffWorker::Execute() {
    (this->*(this->ExecutePtr_))();
}

void DiffWorker::OnOK() {
    (this->*(this->OnOkPtr_))();
}

void DiffWorker::GrayAllPercentExecute() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void DiffWorker::GrayAllPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    Results::ConvertToJs(env, "all", this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::GrayMaskPercentExecute() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void DiffWorker::GrayMaskPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    Results::ConvertToJs(env, "mask", this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::GrayRegionsPercentExecute() {
    this->resultsVec_ = std::vector<uint_fast32_t> (this->regionsLen_, 0);
    MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_, this->resultsVec_);
}

void DiffWorker::GrayRegionsPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    Results::ConvertToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::RgbAllPercentExecute() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->buf0_, this->buf1_);
}

void DiffWorker::RgbAllPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    Results::ConvertToJs(env, "all", this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::RgbMaskPercentExecute() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void DiffWorker::RgbMaskPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    Results::ConvertToJs(env, "mask", this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::RgbRegionsPercentExecute() {
    this->resultsVec_ = std::vector<uint_fast32_t> (this->regionsLen_, 0);
    MeasureDiffs(this->pixCount_, this->pixDepth_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_, this->resultsVec_);
}

void DiffWorker::RgbRegionsPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    Results::ConvertToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}