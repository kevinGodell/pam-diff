#include "engine.h"
#include "ccl.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

//#include <chrono>
#include <iostream>

using std::cout;
using std::endl;

// determine engine type
uint_fast32_t
EngineType(const uint_fast32_t pixDepth, const std::string &response, const bool async, const uint_fast32_t regionsLength) {
    uint_fast32_t value = 0;
    if (pixDepth == 3 || pixDepth == 4) {//dont add for pixDepth == 1
        value += 1;
    }
    if (regionsLength == 1) {//dont add for regionsLength == 0
        value += 10;
    } else if (regionsLength > 1) {
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

    cout << "engine number " << value << endl;

    return value;
}

std::vector<bool>
BitsetJsToCpp(const Napi::Buffer<bool> &bitsetJs) {
    const size_t length = bitsetJs.Length();
    const bool *bitsetJsPtr = bitsetJs.Data();
    std::vector<bool> bitset;
    bitset.assign(bitsetJsPtr, bitsetJsPtr + length);
    return bitset;
}

Region
RegionJsToCpp(const Napi::Object &regionJs) {
    const std::string name = regionJs.Get("name").As<Napi::String>();
    const std::vector<bool> bitset = BitsetJsToCpp(regionJs.Get("bitset").As<Napi::Buffer<bool>>());
    const uint_fast32_t bitsetCount = regionJs.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const uint_fast32_t difference = regionJs.Get("difference").As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = regionJs.Get("percent").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minX = regionJs.Get("minX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxX = regionJs.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint_fast32_t minY = regionJs.Get("minY").As<Napi::Number>().Uint32Value();
    const uint_fast32_t maxY = regionJs.Get("maxY").As<Napi::Number>().Uint32Value();
    return {name, bitset, bitsetCount, difference, percent, {minX, maxX, minY, maxY}};
}

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(const Napi::Array &regionsJs) {
    std::vector<Region> regionVec;
    auto regionsLen = regionsJs.Length();
    regionVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        regionVec.push_back(RegionJsToCpp(regionsJs.Get(r).As<Napi::Object>()));
    }
    return regionVec;
}

// gray all percent
void
GrayAllPercent(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    for (uint_fast32_t p = 0; p < dimensions.pixelCount; ++p) {
        if (all.difference > GrayDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / dimensions.pixelCount;
    percentResult.flagged = percentResult.percent >= all.percent;
}

// gray region percent
void
GrayRegionPercent(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * dimensions.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            ++percentResult.percent;
        }
    }
    percentResult.percent = 100 * percentResult.percent / region.bitsetCount;
    percentResult.flagged = percentResult.percent >= region.percent;
}

// gray regions percent
uint_fast32_t
GrayRegionsPercent(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regions.regions.size();

    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        for (uint_fast32_t y = regions.regions[r].bounds.minY; y <= regions.regions[r].bounds.maxY; ++y) {
            for (uint_fast32_t x = regions.regions[r].bounds.minX, p = y * dimensions.width + x; x <= regions.regions[r].bounds.maxX; ++x, ++p) {
                if (regions.regions[r].bitset[p] == 0 || regions.regions[r].difference > GrayDiff(buf0, buf1, p)) continue;
                ++percentResultVec[r].percent;
            }
        }
    }

    /*for (uint_fast32_t y = regions.bounds.minY; y <= regions.bounds.maxY; ++y) {
        for (uint_fast32_t x = regions.bounds.minX, p = y * dimensions.width + x; x <= regions.bounds.maxX; ++x, ++p) {
            if (!regions.bitset[p]) continue;
            const uint_fast32_t difference = GrayDiff(buf0, buf1, p);
            if (regions.difference > difference) continue;
            for (uint_fast32_t r = 0; r < regionsLen; ++r) {
                if (regions.regions[r].bitset[p] == 0 || regions.regions[r].difference > difference) continue;
                ++percentResultVec[r].percent;
            }
        }
    }*/

    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        percentResultVec[r].name = regions.regions[r].name;
        percentResultVec[r].percent = percentResultVec[r].percent * 100 / regions.regions[r].bitsetCount;
        if (regions.regions[r].percent > percentResultVec[r].percent) continue;
        percentResultVec[r].flagged = true;
        ++flaggedCount;
    }
    return flaggedCount;
}

// gray all bounds
void
GrayAllBounds(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    for (uint_fast32_t y = 0, p = 0; y < dimensions.height; ++y) {
        for (uint_fast32_t x = 0; x < dimensions.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / dimensions.pixelCount;
    boundsResult.flagged = boundsResult.percent >= all.percent;
}

// gray region bounds
void
GrayRegionBounds(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * dimensions.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;
    boundsResult.flagged = boundsResult.percent >= region.percent;
}

// gray regions bounds
uint_fast32_t
GrayRegionsBounds(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regions.regions.size();
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        for (uint_fast32_t y = regions.regions[r].bounds.minY; y <= regions.regions[r].bounds.maxY; ++y) {
            for (uint_fast32_t x = regions.regions[r].bounds.minX, p = y * dimensions.width + x; x <= regions.regions[r].bounds.maxX; ++x, ++p) {
                if (regions.regions[r].bitset[p] == 0 || regions.regions[r].difference > GrayDiff(buf0, buf1, p)) continue;
                SetMin(x, boundsResultVec[r].minX);
                SetMax(x, boundsResultVec[r].maxX);
                SetMin(y, boundsResultVec[r].minY);
                SetMax(y, boundsResultVec[r].maxY);
                ++boundsResultVec[r].percent;
            }
        }
    }
    /*for (uint_fast32_t y = 0, p = 0, r = 0; y < dimensions.height; ++y) {
        for (uint_fast32_t x = 0; x < dimensions.width; ++x, ++p) {
            const uint_fast32_t difference = GrayDiff(buf0, buf1, p);
            if (regions.difference > difference) continue;
            for (r = 0; r < regionsLen; ++r) {
                if (regions.regions[r].bitset[p] == 0 || regions.regions[r].difference > difference) continue;
                SetMin(x, boundsResultVec[r].minX);
                SetMax(x, boundsResultVec[r].maxX);
                SetMin(y, boundsResultVec[r].minY);
                SetMax(y, boundsResultVec[r].maxY);
                ++boundsResultVec[r].percent;
            }
        }
    }*/
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        boundsResultVec[r].name = regions.regions[r].name;
        boundsResultVec[r].percent = boundsResultVec[r].percent * 100 / regions.regions[r].bitsetCount;
        if (regions.regions[r].percent > boundsResultVec[r].percent) continue;
        boundsResultVec[r].flagged = true;
        ++flaggedCount;
    }
    return flaggedCount;
}

// gray all blobs
void
GrayAllBlobs(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BlobsResult &blobsResult) {
    // fill with -2
    std::vector<int_fast32_t> labelsVec = std::vector<int_fast32_t>(dimensions.pixelCount, -2);
    // all elements changed to -1 will be labelled while -2 will be ignored
    for (uint_fast32_t y = 0, p = 0; y < dimensions.height; ++y) {
        for (uint_fast32_t x = 0; x < dimensions.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) continue;
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
    blobsResult.percent = 100 * blobsResult.percent / dimensions.pixelCount;
    // initial percent level has been met, check the sizes of blobs
    if (blobsResult.percent >= all.percent) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(dimensions.width, dimensions.height, blobsResult.minX, blobsResult.maxX, blobsResult.minY, blobsResult.maxY, labelsVec);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, {0, blobsResult.maxX, blobsResult.minX, blobsResult.maxY, blobsResult.minY, 0, false});
        // count and group labels
        for (uint_fast32_t y = blobsResult.minY; y <= blobsResult.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.minX, p = y * dimensions.width + x; x <= blobsResult.maxX; ++x, ++p) {
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
            blob.percent = 100 * blob.percent / dimensions.pixelCount;
            if (all.percent > blob.percent) continue;
            blob.label = b;
            blobsResult.flagged = blob.flagged = true;
        }
    }
}

// gray region blobs
void
GrayRegionBlobs(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BlobsResult &blobsResult) {
    //auto start = std::chrono::high_resolution_clock::now();

    // fill with -2
    std::vector<int_fast32_t> labelsVec = std::vector<int_fast32_t>(dimensions.pixelCount, -2);
    // all elements changed to -1 will be labelled while -2 will be ignored
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * dimensions.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
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
    blobsResult.percent = 100 * blobsResult.percent / region.bitsetCount;
    // percent level has been met, check the sizes of blobs
    if (blobsResult.percent >= region.percent) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(dimensions.width, dimensions.height, blobsResult.minX, blobsResult.maxX, blobsResult.minY, blobsResult.maxY, labelsVec);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, {0, blobsResult.maxX, blobsResult.minX, blobsResult.maxY, blobsResult.minY, 0, false});
        // count and group labels
        for (uint_fast32_t y = blobsResult.minY; y <= blobsResult.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.minX, p = y * dimensions.width + x; x <= blobsResult.maxX; ++x, ++p) {
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
            blob.percent = 100 * blob.percent / region.bitsetCount;
            if (region.percent > blob.percent) continue;
            blob.label = b;
            blobsResult.flagged = blob.flagged = true;
        }
        /*// assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(dimensions.width, dimensions.height, blobsResult.minX, blobsResult.maxX, blobsResult.minY, blobsResult.maxY, labelsVec);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, Blob{0, blobsResult.maxX, bounds.minX, bounds.maxY, bounds.minY, 0, false});
        // count and group labels
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
        }*/
    }
    //auto stop = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    //std::cout << duration.count() << std::endl;
}

// gray regions blobs
uint_fast32_t
GrayRegionsBlobs(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BlobsResult> &blobsResultVec) {
    uint_fast32_t flaggedCount = 0;

    auto regionsLen = regions.regions.size();

    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        GrayRegionBlobs(dimensions, regions.regions[r], buf0, buf1, blobsResultVec[r]);
        if (blobsResultVec[r].flagged) ++flaggedCount;
    }

    return flaggedCount;
}

/*
// rgb all percent
void
RgbAllPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / pixCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// rgb region percent
void
RgbRegionPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, PercentResult &percentResult) {
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        if (bitsetVec[p] == 0 || pixDiff > RgbDiff(buf0, buf1, p * pixDepth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / bitsetCount;
    percentResult.flagged = percentResult.percent >= diffsPerc;
}

// rgb regions percent
uint_fast32_t
RgbRegionsPercent(const uint_fast32_t pixDepth, const uint_fast32_t pixCount, const uint_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<PercentResult> &percentResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regionsVec.size();
    for (uint_fast32_t p = 0; p < pixCount; ++p) {
        const uint_fast32_t diff = RgbDiff(buf0, buf1, p * pixDepth);
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
RgbAllBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixCount, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
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

// rgb region bounds
void
RgbRegionBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t pixDiff, const uint_fast32_t diffsPerc, const uint_fast32_t bitsetCount, const std::vector<bool> &bitsetVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, BoundsResult &boundsResult) {
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
RgbRegionsBounds(const uint_fast32_t pixDepth, const uint_fast32_t width, const uint_fast32_t height, const uint_fast32_t minDiff, const std::vector<Region> &regionsVec, const uint_fast8_t *buf0, const uint_fast8_t *buf1, std::vector<BoundsResult> &boundsResultVec) {
    uint_fast32_t flaggedCount = 0;
    auto regionsLen = regionsVec.size();
    for (uint_fast32_t y = 0, p = 0; y < height; ++y) {
        for (uint_fast32_t x = 0; x < width; ++x, ++p) {
            const uint_fast32_t diff = RgbDiff(buf0, buf1, p * pixDepth);
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




*/