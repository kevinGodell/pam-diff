#include "worker.h"
#include "engine.h"
#include "results.h"
#include "napi.h"
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PixelChangeWorker::PixelChangeWorker(const ExecuteFunc &execute, const CallbackFunc &callback, const Napi::Buffer<uint8_t> &napiBuf0, const Napi::Buffer<uint8_t> &napiBuf1, const Napi::Function &cb)
        : Napi::AsyncWorker(cb),
          execute_(execute),
          callback_(callback),
          buf0_(napiBuf0.Data()),
          buf1_(napiBuf1.Data()),
          buf0ref_(Napi::Reference<Napi::Buffer<uint8_t>>::New(napiBuf0, 1)),
          buf1ref_(Napi::Reference<Napi::Buffer<uint8_t>>::New(napiBuf1, 1)) {
}

void PixelChangeWorker::Execute() {
    this->execute_(this->buf0_, this->buf1_, this->callbackData_);
}

void PixelChangeWorker::OnOK() {
    this->callback_(Env(), Callback().Value(), this->callbackData_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////