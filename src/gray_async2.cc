#include "gray_async2.h"
#include "diff.h"
#include "helper.h"
#include <stdint.h>
#include <napi.h>
//#include <iostream>

GrayDiffWorker::GrayDiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(0), minDiff_(0), regionsLen_(0), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    this->ExecutePtr_ = &GrayDiffWorker::ExecuteOne;
    this->OnOkPtr_ = &GrayDiffWorker::OnOkOne;
    //std::cout << "one" << std::endl;
}

GrayDiffWorker::GrayDiffWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(pixDiff), diffsPerc_(diffsPerc), bitsetCount_(bitsetCount), bitsetVec_(bitsetVec), minDiff_(0), regionsLen_(0), buf0_(buf0), buf1_(buf1), percentResult_(0) {
    this->ExecutePtr_ = &GrayDiffWorker::ExecuteTwo;
    this->OnOkPtr_ = &GrayDiffWorker::OnOkTwo;
    //std::cout << "two" << std::endl;
}

GrayDiffWorker::GrayDiffWorker(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) :
    Napi::AsyncWorker(cb), pixCount_(pixCount), pixDiff_(0), diffsPerc_(0), bitsetCount_(0), minDiff_(minDiff), regionsLen_(regionsLen), regionsVec_(regionsVec), buf0_(buf0), buf1_(buf1) {
    this->ExecutePtr_ = &GrayDiffWorker::ExecuteThree;
    this->OnOkPtr_ = &GrayDiffWorker::OnOkThree;
    //std::cout << "three" << std::endl;
}

void GrayDiffWorker::Execute() {
    (this->*(this->ExecutePtr_))();
    /*this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
    //////
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
    //////
    this->resultsVec_ = std::vector<uint_fast32_t> (this->regionsLen_, 0);
    MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_, this->resultsVec_);*/
}

void GrayDiffWorker::OnOK() {
    (this->*(this->OnOkPtr_))();
    /*const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    //allResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
    //////
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    maskResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
    //////
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    regionsResultsToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_, resultsJs);
    Callback().Call({env.Null(), resultsJs});*/
}

///execute

void GrayDiffWorker::ExecuteOne() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->buf0_, this->buf1_);
}

void GrayDiffWorker::ExecuteTwo() {
    this->percentResult_ = MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitsetVec_, this->buf0_, this->buf1_);
}

void GrayDiffWorker::ExecuteThree() {
    this->resultsVec_ = std::vector<uint_fast32_t> (this->regionsLen_, 0);
    MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsVec_, this->buf0_, this->buf1_, this->resultsVec_);
}

///on ok

void GrayDiffWorker::OnOkOne() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    allResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

void GrayDiffWorker::OnOkTwo() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    maskResultsToJs(env, this->diffsPerc_, this->percentResult_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}

void GrayDiffWorker::OnOkThree() {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    regionsResultsToJs(env, this->regionsLen_, this->regionsVec_, this->resultsVec_, resultsJs);
    Callback().Call({env.Null(), resultsJs});
}