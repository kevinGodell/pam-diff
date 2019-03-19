#include "engine.h"
#include "ccl.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    return value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<bool>
BitsetJsToCpp(const Napi::Buffer<bool> &bitsetJs) {
    const size_t length = bitsetJs.Length();
    const bool *bitsetJsPtr = bitsetJs.Data();
    std::vector<bool> bitset;
    bitset.assign(bitsetJsPtr, bitsetJsPtr + length);
    return bitset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all percent
PercentResult
GrayAllPercent(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {"all", 0, false};// initialize results
    for (uint_fast32_t p = 0; p < dimensions.pixelCount; ++p) {
        if (all.difference > GrayDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / dimensions.pixelCount;
    percentResult.flagged = percentResult.percent >= all.percent;
    return percentResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray region percent
PercentResult
GrayRegionPercent(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {region.name, 0, false};// initialize results
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * dimensions.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            ++percentResult.percent;
        }
    }
    percentResult.percent = 100 * percentResult.percent / region.bitsetCount;
    percentResult.flagged = percentResult.percent >= region.percent;
    return percentResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray regions percent
std::vector<PercentResult>
GrayRegionsPercent(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<PercentResult> percentResultVec;
    percentResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        PercentResult percentResult = GrayRegionPercent(dimensions, regions.regions[r], buf0, buf1);
        if (!percentResult.flagged) continue;
        percentResultVec.push_back(percentResult);
    }
    percentResultVec.shrink_to_fit();
    return percentResultVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all bounds
BoundsResult
GrayAllBounds(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {"all", dimensions.width - 1, 0, dimensions.height - 1, 0, 0, false};// initialize results
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
    return boundsResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray region bounds
BoundsResult
GrayRegionBounds(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {region.name, region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY, 0, false};// initialize results
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
    return boundsResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray regions bounds
std::vector<BoundsResult>
GrayRegionsBounds(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BoundsResult> boundsResultVec;
    boundsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BoundsResult boundsResult = GrayRegionBounds(dimensions, regions.regions[r], buf0, buf1);
        if (!boundsResult.flagged) continue;
        boundsResultVec.push_back(boundsResult);
    }
    boundsResultVec.shrink_to_fit();
    return boundsResultVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all blobs
BlobsResult
GrayAllBlobs(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {"all", dimensions.width - 1, 0, dimensions.height - 1, 0, 0, false, std::vector<Blob>()};// initialize results
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
    return blobsResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray region blobs
BlobsResult
GrayRegionBlobs(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {region.name, region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY, 0, false, std::vector<Blob>()};// initialize results
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
    }
    return blobsResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray regions blobs
std::vector<BlobsResult>
GrayRegionsBlobs(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BlobsResult> blobsResultVec;
    blobsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BlobsResult blobsResult = GrayRegionBlobs(dimensions, regions.regions[r], buf0, buf1);
        if (!blobsResult.flagged) continue;
        blobsResultVec.push_back(blobsResult);
    }
    blobsResultVec.shrink_to_fit();
    return blobsResultVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all percent
PercentResult
RgbAllPercent(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {"all", 0, false};// initialize results
    for (uint_fast32_t p = 0; p < dimensions.pixelCount; ++p) {
        if (all.difference > RgbDiff(buf0, buf1, p * dimensions.depth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / dimensions.pixelCount;
    percentResult.flagged = percentResult.percent >= all.percent;
    return percentResult;
}

// rgb region percent
PercentResult
RgbRegionPercent(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {region.name, 0, false};// initialize results
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * dimensions.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * dimensions.depth)) continue;
            ++percentResult.percent;
        }
    }
    percentResult.percent = 100 * percentResult.percent / region.bitsetCount;
    percentResult.flagged = percentResult.percent >= region.percent;
    return percentResult;
}

// rgb regions percent
std::vector<PercentResult>
RgbRegionsPercent(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<PercentResult> percentResultVec;
    percentResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        PercentResult percentResult = RgbRegionPercent(dimensions, regions.regions[r], buf0, buf1);
        if (!percentResult.flagged) continue;
        percentResultVec.push_back(percentResult);
    }
    percentResultVec.shrink_to_fit();
    return percentResultVec;
}

// rgb all bounds
BoundsResult
RgbAllBounds(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {"all", dimensions.width - 1, 0, dimensions.height - 1, 0, 0, false};// initialize results
    for (uint_fast32_t y = 0, p = 0; y < dimensions.height; ++y) {
        for (uint_fast32_t x = 0; x < dimensions.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * dimensions.depth)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / dimensions.pixelCount;
    boundsResult.flagged = boundsResult.percent >= all.percent;
    return boundsResult;
}

// rgb region bounds
BoundsResult
RgbRegionBounds(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {region.name, region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY, 0, false};// initialize results
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * dimensions.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * dimensions.depth)) continue;
            SetMin(x, boundsResult.minX);
            SetMax(x, boundsResult.maxX);
            SetMin(y, boundsResult.minY);
            SetMax(y, boundsResult.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;
    boundsResult.flagged = boundsResult.percent >= region.percent;
    return boundsResult;
}

// rgb regions bounds
std::vector<BoundsResult>
RgbRegionsBounds(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BoundsResult> boundsResultVec;
    boundsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BoundsResult boundsResult = RgbRegionBounds(dimensions, regions.regions[r], buf0, buf1);
        if (!boundsResult.flagged) continue;
        boundsResultVec.push_back(boundsResult);
    }
    boundsResultVec.shrink_to_fit();
    return boundsResultVec;
}

// rgb all blobs
BlobsResult
RgbAllBlobs(const Dimensions &dimensions, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {"all", dimensions.width - 1, 0, dimensions.height - 1, 0, 0, false, std::vector<Blob>()};// initialize results
    // fill with -2
    std::vector<int_fast32_t> labelsVec = std::vector<int_fast32_t>(dimensions.pixelCount, -2);
    // all elements changed to -1 will be labelled while -2 will be ignored
    for (uint_fast32_t y = 0, p = 0; y < dimensions.height; ++y) {
        for (uint_fast32_t x = 0; x < dimensions.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * dimensions.depth)) continue;
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
    return blobsResult;
}

// rgb region blobs
BlobsResult
RgbRegionBlobs(const Dimensions &dimensions, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {region.name, region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY, 0, false, std::vector<Blob>()};// initialize results
    // fill with -2
    std::vector<int_fast32_t> labelsVec = std::vector<int_fast32_t>(dimensions.pixelCount, -2);
    // all elements changed to -1 will be labelled while -2 will be ignored
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * dimensions.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * dimensions.depth)) continue;
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
    }
    return blobsResult;
}

// rgb regions blobs
std::vector<BlobsResult>
RgbRegionsBlobs(const Dimensions &dimensions, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BlobsResult> blobsResultVec;
    blobsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BlobsResult blobsResult = RgbRegionBlobs(dimensions, regions.regions[r], buf0, buf1);
        if (!blobsResult.flagged) continue;
        blobsResultVec.push_back(blobsResult);
    }
    blobsResultVec.shrink_to_fit();
    return blobsResultVec;
}