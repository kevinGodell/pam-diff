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
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
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
DiffWorker::DiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDepth, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, std::vector<Engine::Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDepth_(pixDepth), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::RgbRegionsPercentExecute;
    this->OnOkPtr_ = &DiffWorker::RgbRegionsPercentOnOk;
}

//experiment

//gray all bounds
DiffWorker::DiffWorker(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::GrayAllBoundsExecute;
    this->OnOkPtr_ = &DiffWorker::GrayAllBoundsOnOk;
}

//gray mask bounds
DiffWorker::DiffWorker(const uint_fast16_t width, const uint_fast16_t height, const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), width_(width), height_(height), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &DiffWorker::GrayMaskBoundsExecute;
    this->OnOkPtr_ = &DiffWorker::GrayMaskBoundsOnOk;
}

void DiffWorker::Execute() {
    (this->*(this->ExecutePtr_))();
}

void DiffWorker::OnOK() {
    (this->*(this->OnOkPtr_))();
}

void DiffWorker::GrayAllPercentExecute() {
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void DiffWorker::GrayAllPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, "all", this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::GrayMaskPercentExecute() {
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void DiffWorker::GrayMaskPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, "mask", this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::GrayRegionsPercentExecute() {
    this->resultsVec_ = Engine::MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

void DiffWorker::GrayRegionsPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::RgbAllPercentExecute() {
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->buf0_, this->buf1_);
}

void DiffWorker::RgbAllPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, "all", this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::RgbMaskPercentExecute() {
    this->percentResult_ = Engine::MeasureDiffs(this->pixCount_, this->pixDepth_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void DiffWorker::RgbMaskPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, "mask", this->diffsPerc_, this->percentResult_);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::RgbRegionsPercentExecute() {
    this->resultsVec_ = Engine::MeasureDiffs(this->pixCount_, this->pixDepth_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_);
}

void DiffWorker::RgbRegionsPercentOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_);
    Callback().Call({env.Null(), resultsJs});
}

///experiments

void DiffWorker::GrayAllBoundsExecute() {
    this->boundsResult_ = Engine::MeasureDiffs(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void DiffWorker::GrayAllBoundsOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, "all", this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}

void DiffWorker::GrayMaskBoundsExecute() {
    this->boundsResult_ = Engine::MeasureDiffs(this->width_, this->height_, this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void DiffWorker::GrayMaskBoundsOnOk() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Results::ConvertToJs(env, "mask", this->diffsPerc_, this->boundsResult_);
    Callback().Call({env.Null(), resultsJs});
}