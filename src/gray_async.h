#ifndef SRC_GRAY_ASYNC_H_
#define SRC_GRAY_ASYNC_H_

#include "helper.h"
#include "diff.h"
#include <stdint.h>
#include <napi.h>

class GrayDiffAllWorker : public Napi::AsyncWorker {

public:
    GrayDiffAllWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
    void Execute();
    void OnOK();

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

////////////////////////////////////////////////////////////////////////////

class GrayDiffMaskWorker : public Napi::AsyncWorker {

public:
    GrayDiffMaskWorker(const uint_fast32_t pixCount, const uint_fast8_t pixDiff, const uint_fast8_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
    void Execute();
    void OnOK();

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t pixDiff_;// can only be 0 - 255
    const uint_fast8_t diffsPerc_;// minimum percent 0 - 100
    const uint_fast32_t bitsetCount_;// number of 1's in bitset, others are 0
    const std::vector<bool> bitsetVec_;// mask, 1's and 0's based on pixel index
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    uint_fast8_t percentResult_;// percent placeholder
};

////////////////////////////////////////////////////////////////////////////

class GrayDiffRegionsWorker : public Napi::AsyncWorker {

public:
    GrayDiffRegionsWorker(const uint_fast32_t pixCount, const uint_fast8_t minDiff, const uint_fast8_t regionsLen, std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb);
    void Execute();
    void OnOK();

private:
    const uint_fast32_t pixCount_;// number of pixels
    const uint_fast8_t minDiff_;// can only be 0 - 255
    const uint_fast8_t regionsLen_;// number of 1's in bitset, others are 0
    const std::vector<Region> regionsVec_;//
    const uint_fast8_t *buf0_;// pixel buffer
    const uint_fast8_t *buf1_;// pixel buffer
    std::vector<uint_fast32_t> resultsVec_;//
};

#endif