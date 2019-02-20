#include "engine.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <iostream>

//#include "ccl.c"
#include "labelmethod.c"

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
RegionsJsToCpp(const uint_fast32_t pixCount, const uint_fast32_t regionsLen, const Napi::Array &regionsJs) {
    std::vector<Region> regionVec;
    regionVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        const std::string name = regionsJs.Get(r).As<Napi::Object>().Get("name").As<Napi::String>();
        const int_fast32_t diff = regionsJs.Get(r).As<Napi::Object>().Get("diff").As<Napi::Number>().Int32Value();
        const uint_fast32_t percent = regionsJs.Get(r).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsJs.Get(r).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const bool *bitset = regionsJs.Get(r).As<Napi::Object>().Get("bitset").As<Napi::Buffer<bool>>().Data();
        std::vector<bool> bitsetVec;
        bitsetVec.assign(bitset, bitset + pixCount);
        regionVec.push_back(Region{name, bitsetVec, diff, count, percent});
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
GrayRegionsPercent(const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
    for (uint_fast32_t p = 0, r = 0; p < pixCount; ++p) {
        const int_fast32_t diff = GrayDiff(buf0, buf1, p);
        if (minDiff > diff) continue;
        for (r = 0; r < regionsLen; ++r) {
            if (regionsVec[r].bitset[p] == 0 || regionsVec[r].pixDiff > diff) continue;
            ++percentResultVec[r].percent;
        }
    }
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].name = regionsVec[r].name;
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regionsVec[r].bitsetCount;
        if (percentResultVec[r].percent >= regionsVec[r].percent) {
            percentResultVec[r].flagged = true;
            ++flaggedCount;
        }
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
GrayRegionsBounds(const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
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
        if (boundsResultVec[r].percent >= regionsVec[r].percent) {
            boundsResultVec[r].flagged = true;
            ++flaggedCount;
        }
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
RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
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
        if (percentResultVec[r].percent >= regionsVec[r].percent) {
            percentResultVec[r].flagged = true;
            ++flaggedCount;
        }
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
RgbRegionsBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const int_fast32_t minDiff, const uint_fast32_t regionsLen, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
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
        if (boundsResultVec[r].percent >= regionsVec[r].percent) {
            boundsResultVec[r].flagged = true;
            ++flaggedCount;
        }
    }
    return flaggedCount;
}

// gray all blobs
void
GrayAllBlobs(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const int_fast32_t pixDiff, const uint_fast32_t diffsPerc, /*const uint_fast32_t blobSize,*/ const uint_fast8_t *buf0, const uint_fast8_t *buf1, BlobsResult &blobsResult) {

    // fill with -1
    std::vector<int_fast32_t> labelsVec = std::vector<int_fast32_t>(pixCount, -1);

    // all elements changed to 0 will be labelled while -1 will be ignored
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            if (pixDiff > GrayDiff(buf0, buf1, p)) continue;
            //change from -1 to 0 to mark as pixel of interest
            labelsVec[p] = 0;
            SetMin(x, blobsResult.minX);
            SetMax(x, blobsResult.maxX);
            SetMin(y, blobsResult.minY);
            SetMax(y, blobsResult.maxY);
            ++blobsResult.percent;
        }
    }

    blobsResult.percent = 100 * blobsResult.percent / pixCount;

    //todo dont flag yet until checking blobs sizes meeting threshold
    //blobsResult.flagged = blobsResult.percent >= diffsPerc;

    // percent level has been met, now check the sizes of blobs
    if (blobsResult.percent > diffsPerc) {
        //todo carry on to label and blob

        // assign label to each indexed pixel that has a 0 instead of -1, returns the highest label value
        uint_fast32_t highLabel = LabelImage(width, height, blobsResult.minX, blobsResult.maxX, blobsResult.minY, blobsResult.maxY, labelsVec);

        uint_fast32_t blobCount = highLabel + 1;
        //std::cout << "high label " << highLabel << std::endl;

        //std::vector<Blob> blobVec = std::vector<Blob>(highLabel + 1, Blob{std::string(), width - 1, 0, height - 1, 0, 0, false});

        blobsResult.blobs = std::vector<Blob>(blobCount, Blob{0, width - 1, 0, height - 1, 0, 0, false});

        for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
            for (uint_fast32_t x = 0; x < width; ++x, ++p) {
        //for (uint_fast32_t y = blobsResult.minY, p = 0; y <= blobsResult.maxY; ++y) {
            //for (uint_fast32_t x = blobsResult.minX; x <= blobsResult.maxX; ++x, p = width * y + x) {
                if (labelsVec[p] < 1) continue;// ignored(-1) or unlabelled(0)
                Blob &blob = blobsResult.blobs[labelsVec[p]];
                SetMin(x, blob.minX);
                SetMax(x, blob.maxX);
                SetMin(y, blob.minY);
                SetMax(y, blob.maxY);
                ++blob.percent;
            }
        }

        for (uint_fast32_t b = 1; b < blobCount; ++b) {
            Blob &blob = blobsResult.blobs[b];
            //std::cout << "count before percenting it " << blob.percent << std::endl;
            blob.percent = 100 * blob.percent / pixCount;
            //std::cout << "count after percenting it " << blob.percent << std::endl;
            if (blob.percent >= diffsPerc) {
                blob.label = b;
                blobsResult.flagged = blob.flagged = true;
            }
        }

        //std::cout << "total percent that triggered the blob search " << blobsResult.percent << std::endl;

        //for (uint_fast32_t i = 1; i < highLabel + 1; ++i) {

        //  std::cout << "label " << i << " has percent " << blobsResult.blobs[i].percent << " minX " << blobsResult.blobs[i].minX << " maxX " << blobsResult.blobs[i].maxX << " minY " << blobsResult.blobs[i].minY << " maxY " << blobsResult.blobs[i].maxY << " flagged " << blobsResult.blobs[i].flagged << std::endl;
        //}

    }

}

// assign label value to each pixel
uint_fast32_t
LabelImage(const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t minX, const uint_fast32_t maxX, const uint_fast32_t minY, const uint_fast32_t maxY, std::vector<int_fast32_t> &labelsVec) {

    std::unique_ptr<uint_fast32_t[]> stack(new uint_fast32_t[3 * (width * height + 1)]);

    // label number
    int_fast32_t labelNumber = 0;

    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {

        for (uint_fast32_t x = 0; x < width; ++x, ++p) {

    //for (uint_fast32_t y = minY, p = 0; y <= maxY; ++y) {

        //for (uint_fast32_t x = minX; x <= maxX; ++x, p = width * y + x) {

            // ignored == -1, unlabeled == 0, labeled > 0
            if (labelsVec[p] != 0) continue;   /* This pixel has already been labelled  */

            /* New component found, increment label number */
            ++labelNumber;

            // send to C function for recursive labelling
            // todo may send min and max x y for bounds
            LabelComponent(stack.get(), width, /*height,*/ minX, maxX, minY, maxY, labelNumber, x, y, labelsVec.data());

        }
    }

    return static_cast<uint_fast32_t>(labelNumber);
}