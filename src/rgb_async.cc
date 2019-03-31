#include "rgb_async.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllPercentWorker::RgbAllPercentWorker(const Config &config, const All &all, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          all_(all),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbAllPercentWorker::Execute() {
    this->percentResult_ = RgbAllPercent(this->config_, this->all_, this->buf0_, this->buf1_);
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

RgbRegionPercentWorker::RgbRegionPercentWorker(const Config &config, const Region &region, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          region_(region),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbRegionPercentWorker::Execute() {
    this->percentResult_ = RgbRegionPercent(this->config_, this->region_, this->buf0_, this->buf1_);
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

RgbRegionsPercentWorker::RgbRegionsPercentWorker(const Config &config, const Regions &regions, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          regions_(regions),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)) {
}

void RgbRegionsPercentWorker::Execute() {
    this->percentResultVec_ = RgbRegionsPercent(this->config_, this->regions_, this->buf0_, this->buf1_);
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

RgbAllBoundsWorker::RgbAllBoundsWorker(const Config &config, const All &all, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          all_(all),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbAllBoundsWorker::Execute() {
    this->boundsResult_ = RgbAllBounds(this->config_, this->all_, this->buf0_, this->buf1_);
    if (this->boundsResult_.flagged && this->config_.draw) {
        this->pixels_ = new uint_fast8_t[this->config_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->config_.byteLength, this->pixels_);
        DrawRgb(this->boundsResult_, this->config_, this->pixels_);
    }
}

void RgbAllBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->config_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBoundsWorker::RgbRegionBoundsWorker(const Config &config, const Region &region, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          region_(region),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionBoundsWorker::Execute() {
    this->boundsResult_ = RgbRegionBounds(this->config_, this->region_, this->buf0_, this->buf1_);
    if (this->boundsResult_.flagged && this->config_.draw) {
        this->pixels_ = new uint_fast8_t[this->config_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->config_.byteLength, this->pixels_);
        DrawRgb(this->boundsResult_, this->config_, this->pixels_);
    }
}

void RgbRegionBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->boundsResult_.flagged) {
        ToJs(env, this->boundsResult_, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->config_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBoundsWorker::RgbRegionsBoundsWorker(const Config &config, const Regions &regions, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          regions_(regions),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionsBoundsWorker::Execute() {
    this->boundsResultVec_ = RgbRegionsBounds(this->config_, this->regions_, this->buf0_, this->buf1_);
    if (!this->boundsResultVec_.empty() && this->config_.draw) {
        this->pixels_ = new uint_fast8_t[this->config_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->config_.byteLength, this->pixels_);
        DrawRgb(this->boundsResultVec_, this->config_, this->pixels_);
    }
}

void RgbRegionsBoundsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!this->boundsResultVec_.empty()) {
        ToJs(env, this->boundsResultVec_, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->config_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbAllBlobsWorker::RgbAllBlobsWorker(const Config &config, const All &all, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          all_(all),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbAllBlobsWorker::Execute() {
    this->blobsResult_ = RgbAllBlobs(this->config_, this->all_, this->buf0_, this->buf1_);
    if (this->blobsResult_.flagged && this->config_.draw) {
        this->pixels_ = new uint_fast8_t[this->config_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->config_.byteLength, this->pixels_);
        DrawRgb(this->blobsResult_, this->config_, this->pixels_);
    }
}

void RgbAllBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->blobsResult_.flagged) {
        ToJs(env, this->blobsResult_, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->config_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionBlobsWorker::RgbRegionBlobsWorker(const Config &config, const Region &region, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          region_(region),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionBlobsWorker::Execute() {
    this->blobsResult_ = RgbRegionBlobs(this->config_, this->region_, this->buf0_, this->buf1_);
    if (this->blobsResult_.flagged && this->config_.draw) {
        this->pixels_ = new uint_fast8_t[this->config_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->config_.byteLength, this->pixels_);
        DrawRgb(this->blobsResult_, this->config_, this->pixels_);
    }
}

void RgbRegionBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (this->blobsResult_.flagged) {
        ToJs(env, this->blobsResult_, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->config_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RgbRegionsBlobsWorker::RgbRegionsBlobsWorker(const Config &config, const Regions &regions, const Napi::Buffer<uint_fast8_t> &napiBuf0, const Napi::Buffer<uint_fast8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          config_(config),
          regions_(regions),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf0, 1)),
          buf1Ref_(Napi::Reference<Napi::Buffer<uint_fast8_t>>::New(napiBuf1, 1)),
          pixels_(nullptr) {
}

void RgbRegionsBlobsWorker::Execute() {
    this->blobsResultVec_ = RgbRegionsBlobs(this->config_, this->regions_, this->buf0_, this->buf1_);
    if (!this->blobsResultVec_.empty() && this->config_.draw) {
        this->pixels_ = new uint_fast8_t[this->config_.byteLength]();
        std::copy(this->buf1_, this->buf1_ + this->config_.byteLength, this->pixels_);
        DrawRgb(this->blobsResultVec_, this->config_, this->pixels_);
    }
}

void RgbRegionsBlobsWorker::OnOK() {
    const Napi::Env env = Env();
    const Napi::HandleScope scope(env);
    Napi::Array resultsJs = Napi::Array::New(env);
    if (!this->blobsResultVec_.empty()) {
        ToJs(env, this->blobsResultVec_, resultsJs);
        if (this->config_.draw) {
            const Napi::Buffer<uint_fast8_t> pixels = Napi::Buffer<uint_fast8_t>::New(env, this->pixels_, this->config_.byteLength, DeleteExternalData);
            Callback().Call({env.Null(), resultsJs, pixels});
            return;
        }
    }
    Callback().Call({env.Null(), resultsJs});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////