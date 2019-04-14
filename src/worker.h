#ifndef SRC_WORKER_H_
#define SRC_WORKER_H_

#include "engine.h"
#include "napi.h"
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PixelChangeWorker : public Napi::AsyncWorker {
public:
    PixelChangeWorker(const ExecuteFunc &execute, const CallbackFunc &callback, const Napi::Buffer<uint8_t> &napiBuf0, const Napi::Buffer<uint8_t> &napiBuf1, const Napi::Function &cb);

    void Execute() override;

    void OnOK() override;

private:
    // in
    const ExecuteFunc execute_;
    const CallbackFunc callback_;
    const uint8_t *buf0_;
    const uint8_t *buf1_;
    const Napi::Reference<Napi::Buffer<uint8_t>> buf0ref_;
    const Napi::Reference<Napi::Buffer<uint8_t>> buf1ref_;

    // out
    CallbackData callbackData_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif