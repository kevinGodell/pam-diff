#include "rgb_async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentWorker::RgbAllPercentWorker(const Dimensions &dimensions, const All &all, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          all_(all),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbAllPercentWorker::Execute() {
    this->percentResult_ = RgbAllPercent(this->dimensions_, this->all_, this->buf0_, this->buf1_);
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

RgbRegionPercentWorker::RgbRegionPercentWorker(const Dimensions &dimensions, const Region &region, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          region_(region),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbRegionPercentWorker::Execute() {
    this->percentResult_ = RgbRegionPercent(this->dimensions_, this->region_, this->buf0_, this->buf1_);
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

RgbRegionsPercentWorker::RgbRegionsPercentWorker(const Dimensions &dimensions, const Regions &regions, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          regions_(regions),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbRegionsPercentWorker::Execute() {
    this->percentResultVec_ = RgbRegionsPercent(this->dimensions_, this->regions_, this->buf0_, this->buf1_);
}

void RgbRegionsPercentWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!this->percentResultVec_.empty()) {
        ToJs(env, this->percentResultVec_, resultsJs);
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBoundsWorker::RgbAllBoundsWorker(const Dimensions &dimensions, const All &all, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          all_(all),
          draw_(draw),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbAllBoundsWorker::Execute() {
    this->boundsResult_ = RgbAllBounds(this->dimensions_, this->all_, this->buf0_, this->buf1_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawRgb(this->boundsResult_, this->dimensions_.width, this->dimensions_.depth, this->pixels_);
    }
}

void RgbAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBoundsWorker::RgbRegionBoundsWorker(const Dimensions &dimensions, const Region &region, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          region_(region),
          draw_(draw),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionBoundsWorker::Execute() {
    this->boundsResult_ = RgbRegionBounds(this->dimensions_, this->region_, this->buf0_, this->buf1_);
    if (this->boundsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawRgb(this->boundsResult_, this->dimensions_.width, this->dimensions_.depth, this->pixels_);
    }
}

void RgbRegionBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsWorker::RgbRegionsBoundsWorker(const Dimensions &dimensions, const Regions &regions, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          regions_(regions),
          draw_(draw),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionsBoundsWorker::Execute() {
    this->boundsResultVec_ = RgbRegionsBounds(this->dimensions_, this->regions_, this->buf0_, this->buf1_);
    if (!this->boundsResultVec_.empty() && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawRgb(this->boundsResultVec_, this->dimensions_.width, this->dimensions_.depth, this->pixels_);
    }
}

void RgbRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!this->boundsResultVec_.empty()) {
        ToJs(env, this->boundsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBlobsWorker::RgbAllBlobsWorker(const Dimensions &dimensions, const All &all, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          all_(all),
          draw_(draw),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbAllBlobsWorker::Execute() {
    this->blobsResult_ = RgbAllBlobs(this->dimensions_, this->all_, this->buf0_, this->buf1_);
    if (this->blobsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawRgb(this->blobsResult_, this->dimensions_.width, this->dimensions_.depth, this->pixels_);
    }
}

void RgbAllBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->blobsResult_.flagged) {
        ToJs(env, this->blobsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBlobsWorker::RgbRegionBlobsWorker(const Dimensions &dimensions, const Region &region, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          region_(region),
          draw_(draw),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionBlobsWorker::Execute() {
    this->blobsResult_ = RgbRegionBlobs(this->dimensions_, this->region_, this->buf0_, this->buf1_);
    if (this->blobsResult_.flagged && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawRgb(this->blobsResult_, this->dimensions_.width, this->dimensions_.depth, this->pixels_);
    }
}

void RgbRegionBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->blobsResult_.flagged) {
        ToJs(env, this->blobsResult_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBlobsWorker::RgbRegionsBlobsWorker(const Dimensions &dimensions, const Regions &regions, const bool draw, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          dimensions_(dimensions),
          regions_(regions),
          draw_(draw),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionsBlobsWorker::Execute() {
    this->blobsResultVec_ = RgbRegionsBlobs(this->dimensions_, this->regions_, this->buf0_, this->buf1_);
    if (!this->blobsResultVec_.empty() && this->draw_) {
        this->pixels_ = new uint_fast8_t[this->dimensions_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->dimensions_.byteLength, this->pixels_);
        DrawRgb(this->blobsResultVec_, this->dimensions_.width, this->dimensions_.depth, this->pixels_);
    }
}

void RgbRegionsBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!this->blobsResultVec_.empty()) {
        ToJs(env, this->blobsResultVec_, resultsJs);
        if (this->draw_) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->dimensions_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////