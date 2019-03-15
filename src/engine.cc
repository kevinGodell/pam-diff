#include "engine.h"
#include "ccl.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

//#include <chrono>
//#include <iostream>

// determine engine type
uint_fast32_t
EngineType(const uint_fast32_t pixDepth, const std::string &target, const std::string &response, const bool async) {
    uint_fast32_t value = 0;
    if (pixDepth == 3 || pixDepth == 4) {//dont add for pixDepth == 1
        value += 1;
    }
    if (target == "mask") {//dont add for target == "all"
        value += 10;
    } else if (target == "regions") {
        value += 20;
    }
    if (response == "bounds") {//dont add for target == "percent"
        value += 100;
    } else if (response == "blobs") {
        value += 200;
    }
    if (async) {
        value += 1000;
    }
    return value;
}

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(const uint_fast32_t pixCount, const Napi::Array &regionsJs) {
    std::vector<Region> regionVec;
    auto regionsLen = regionsJs.Length();
    regionVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        const std::string name = regionsJs.Get(r).As<Napi::Object>().Get("name").As<Napi::String>();
        const int_fast32_t diff = regionsJs.Get(r).As<Napi::Object>().Get("diff").As<Napi::Number>().Int32Value();
        const uint_fast32_t percent = regionsJs.Get(r).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsJs.Get(r).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const bool *bitset = regionsJs.Get(r).As<Napi::Object>().Get("bitset").As<Napi::Buffer<bool>>().Data();
        std::vector<bool> bitsetVec;
        bitsetVec.assign(bitset, bitset + pixCount);
        const uint_fast32_t minX = regionsJs.Get(r).As<Napi::Object>().Get("minX").As<Napi::Number>().Uint32Value();
        const uint_fast32_t maxX = regionsJs.Get(r).As<Napi::Object>().Get("maxX").As<Napi::Number>().Uint32Value();
        const uint_fast32_t minY = regionsJs.Get(r).As<Napi::Object>().Get("minY").As<Napi::Number>().Uint32Value();
        const uint_fast32_t maxY = regionsJs.Get(r).As<Napi::Object>().Get("maxY").As<Napi::Number>().Uint32Value();
        regionVec.push_back(Region{name, bitsetVec, diff, count, percent, minX, maxX, minY, maxY});
    }
    return regionVec;
}

// gray all percent
void
GrayAllPercent(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / pixCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// gray mask percent
void
GrayMaskPercent(const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (bitsetVec[p] == 0 || pixDiff > GrayDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / bitsetCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// gray regions percent
uint_fast32_t
GrayRegionsPercent(const uint_fast32_t pixCount, const int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regionsVec.size();
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        const int_fast32_t diff = GrayDiff(buf0, buf1, p);
        if (minDiff > diff) continue;
        for (uint_fast32_t r = 0; r < regionsLen; ++r) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            ++percentResultVec[r].percent;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].name = regionsVec[r].name;
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (regionsVec[r].percent > percentResultVec[r].percent) continue;
        percentResultVec[r].flagged = true;
        ++flaggedCount;
    }
    return flaggedCount;
}

// gray all bounds
void
GrayAllBounds(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / pixCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// gray mask bounds
void
GrayMaskBounds(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (bitsetVec[p] == 0 || pixDiff > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / bitsetCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// gray regions bounds
uint_fast32_t
GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regionsVec.size();
    for (uint_fast32_t y = 0, p = 0, r = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            const int_fast32_t diff = GrayDiff(buf0, buf1, p);
            if (minDiff > diff) continue;
            for (r = 0; r < regionsLen; ++r) {
                if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
                SetMin(x, boundsResultVec[r].minX);
                SetMax(x, boundsResultVec[r].maxX);
                SetMin(y, boundsResultVec[r].minY);
                SetMax(y, boundsResultVec[r].maxY);
                ++boundsResultVec[r].percent;
            }
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        boundsResultVec[r].name = regionsVec[r].name;
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (regionsVec[r].percent > boundsResultVec[r].percent) continue;
        boundsResultVec[r].flagged = true;
        ++flaggedCount;
    }
    return flaggedCount;
}

// rgb all percent
void
RgbAllPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    /*uint_fast32_t limit = pixCount * pixDepth;
    for (uint_fast32_t p = 0; p < limit; p += pixDepth) {
        if (pixDiff > RgbDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }*/
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / pixCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// rgb mask percent
void
RgbMaskPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / bitsetCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// rgb regions percent
uint_fast32_t
RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regionsVec.size();
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        const int_fast32_t diff = RgbDiff(buf0, buf1, p * pixDepth);
        if (minDiff > diff) continue;
        for (uint_fast32_t r = 0; r < regionsLen; ++r) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            ++percentResultVec[r].percent;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].name = regionsVec[r].name;
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (regionsVec[r].percent > percentResultVec[r].percent) continue;
        percentResultVec[r].flagged = true;
        ++flaggedCount;
    }
    return flaggedCount;
}

// rgb all bounds
void
RgbAllBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / pixCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// rgb mask bounds
void
RgbMaskBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / bitsetCount;
    boundsResult.flagged = boundsResult.percent >= diffsPerc;
}

// rgb regions bounds
uint_fast32_t
RgbRegionsBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regionsVec.size();
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            const int_fast32_t diff = RgbDiff(buf0, buf1, p * pixDepth);
            if (minDiff > diff) continue;
            for (uint_fast32_t r = 0; r < regionsLen; ++r) {
                if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
                SetMin(x, boundsResultVec[r].minX);
                SetMax(x, boundsResultVec[r].maxX);
                SetMin(y, boundsResultVec[r].minY);
                SetMax(y, boundsResultVec[r].maxY);
                ++boundsResultVec[r].percent;
            }
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        boundsResultVec[r].name = regionsVec[r].name;
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (regionsVec[r].percent > boundsResultVec[r].percent) continue;
        boundsResultVec[r].flagged = true;
        ++flaggedCount;
    }
    return flaggedCount;
}

// gray all blobs
void
GrayAllBlobs(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BlobsResult &blobsResult) {
    // fill with -2
    std::vector<int_fast32_t> labelsVec = std::vector<int_fast32_t>(pixCount, -2);
    // all elements changed to -1 will be labelled while -2 will be ignored
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
            //change from -2 to -1 to mark as pixel of interest
            labelsVec[p] = -1;
            SetMin(x, blobsResult.minX);
            SetMax(x, blobsResult.maxX);
            SetMin(y, blobsResult.minY);
            SetMax(y, blobsResult.maxY);
            ++blobsResult.percent;
        }
    }
    // calculate percent size of blobbed pixels
    blobsResult.percent = 100 * blobsResult.percent / pixCount;
    // percent level has been met, check the sizes of blobs
    if (blobsResult.percent > diffsPerc) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(width, height, blobsResult.minX, blobsResult.maxX, blobsResult.minY, blobsResult.maxY, labelsVec);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, Blob{0, width - 1, 0, height - 1, 0, 0, false});
        // count and group labels
        /*for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
            for (uint_fast32_t x = 0; x < width; ++x, ++p) {*/
        for (uint_fast32_t y = blobsResult.minY; y <= blobsResult.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.minX, p = y * width + x; x <= blobsResult.maxX; ++x, ++p) {
                if (labelsVec[p] == -2) continue;// ignored(-2) or unlabelled(-1)
                Blob &blob = blobsResult.blobs[labelsVec[p]];
                SetMin(x, blob.minX);
                SetMax(x, blob.maxX);
                SetMin(y, blob.minY);
                SetMax(y, blob.maxY);
                ++blob.percent;
            }
        }
        // convert blob size to percent and check against threshold and flag
        for (uint_fast32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobsResult.blobs[b];
            blob.percent = 100 * blob.percent / pixCount;
            if (diffsPerc > blob.percent) continue;
            blob.label = b;
            blobsResult.flagged = blob.flagged = true;
        }
    }
}

// gray all blobs
void
GrayMaskBlobs(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BlobsResult &blobsResult) {
    //auto start = std::chrono::high_resolution_clock::now();
    // fill with -2
    std::vector<int_fast32_t> labelsVec = std::vector<int_fast32_t>(width * height, -2);
    // all elements changed to -1 will be labelled while -2 will be ignored
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (bitsetVec[p] == 0 || pixDiff > GrayDiff(buf0, buf1, p)) continue;
            //change from -2 to -1 to mark as pixel of interest
            labelsVec[p] = -1;
            SetMin(x, blobsResult.minX);
            SetMax(x, blobsResult.maxX);
            SetMin(y, blobsResult.minY);
            SetMax(y, blobsResult.maxY);
            ++blobsResult.percent;
        }
    }
    // calculate percent size of blobbed pixels
    blobsResult.percent = 100 * blobsResult.percent / bitsetCount;
    // percent level has been met, check the sizes of blobs
    if (blobsResult.percent > diffsPerc) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(width, height, blobsResult.minX, blobsResult.maxX, blobsResult.minY, blobsResult.maxY, labelsVec);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, Blob{0, width - 1, 0, height - 1, 0, 0, false});
        // count and group labels
        /*for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
            for (uint_fast32_t x = 0; x < width; ++x, ++p) {*/
        for (uint_fast32_t y = blobsResult.minY; y <= blobsResult.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.minX, p = y * width + x; x <= blobsResult.maxX; ++x, ++p) {
                if (labelsVec[p] == -2) continue;// ignored(-2) or unlabelled(-1)
                Blob &blob = blobsResult.blobs[labelsVec[p]];
                SetMin(x, blob.minX);
                SetMax(x, blob.maxX);
                SetMin(y, blob.minY);
                SetMax(y, blob.maxY);
                ++blob.percent;
            }
        }
        // convert blob size to percent and check against threshold and flag
        for (uint_fast32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobsResult.blobs[b];
            blob.percent = 100 * blob.percent / bitsetCount;
            if (diffsPerc > blob.percent) continue;
            blob.label = b;
            blobsResult.flagged = blob.flagged = true;
        }
    }
    //auto stop = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    //std::cout << duration.count() << std::endl;
}

// gray regions blobs
uint_fast32_t
GrayRegionsBlobs(const uint_fast32_t width, const uint_fast32_t /*height*/, const int_fast32_t minDiff, const uint_fast32_t minX, const uint_fast32_t maxX, const uint_fast32_t minY, const uint_fast32_t maxY, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BlobsResult> &blobsResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regionsVec.size();
    for (uint_fast32_t y = minY; y <= maxY; ++y) {
        for (uint_fast32_t x = minX, p = y * width + x; x <= maxX; ++x, ++p) {
            const int_fast32_t diff = GrayDiff(buf0, buf1, p);
            if (minDiff > diff) continue;
            for (uint_fast32_t r = 0; r < regionsLen; ++r) {
                if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
                SetMin(x, blobsResultVec[r].minX);
                SetMax(x, blobsResultVec[r].maxX);
                SetMin(y, blobsResultVec[r].minY);
                SetMax(y, blobsResultVec[r].maxY);
                ++blobsResultVec[r].percent;
            }
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        blobsResultVec[r].name = regionsVec[r].name;
        blobsResultVec[r].percent = blobsResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (regionsVec[r].percent > blobsResultVec[r].percent) continue;
        blobsResultVec[r].flagged = true;
        ++flaggedCount;
    }
    return flaggedCount;
}