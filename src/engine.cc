#include "engine.h"
#include "ccl.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// determine engine type
uint_fast32_t
EngineType(const uint_fast32_t depth, const uint_fast32_t regionsLength, const std::string &response, const bool sync) {
    uint_fast32_t value = 0;
    value += depth == 4 || depth == 3 ? 1000 : 0;
    value += regionsLength > 1 ? 200 : regionsLength == 1 ? 100 : 0;
    value += response == "blobs" ? 20 : response == "bounds" ? 10 : 0;
    value += sync ? 0 : 1;
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
GrayAllPercent(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {all.name.data(), 0, false};// initialize results
    for (uint_fast32_t p = 0; p < config.pixelCount; ++p) {
        if (all.difference > GrayDiff(buf0, buf1, p)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / config.pixelCount;
    percentResult.flagged = percentResult.percent >= all.percent;
    return percentResult;
}

// gray all percent
Results
GrayAllPercentExecute(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    return {GrayAllPercent(config, all, buf0, buf1), {}, {}, {}, {}, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray region percent
PercentResult
GrayRegionPercent(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {region.name.data(), 0, false};// initialize results
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            ++percentResult.percent;
        }
    }
    percentResult.percent = 100 * percentResult.percent / region.bitsetCount;
    percentResult.flagged = percentResult.percent >= region.percent;
    return percentResult;
}

// gray region percent
Results
GrayRegionPercentExecute(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    return {GrayRegionPercent(config, region, buf0, buf1), {}, {}, {}, {}, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray regions percent
std::vector<PercentResult>
GrayRegionsPercent(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<PercentResult> percentResultVec;
    percentResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        PercentResult percentResult = GrayRegionPercent(config, regions.regions[r], buf0, buf1);
        if (!percentResult.flagged) continue;
        percentResultVec.push_back(percentResult);
    }
    percentResultVec.shrink_to_fit();
    return percentResultVec;
}

// gray regions percent
Results
GrayRegionsPercentExecute(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    return {{}, {}, {}, GrayRegionsPercent(config, regions, buf0, buf1), {}, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all bounds
BoundsResult
GrayAllBounds(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {all.name.data(), {config.width - 1, 0, config.height - 1, 0}, 0, false};// initialize results
    for (uint_fast32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint_fast32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / config.pixelCount;
    boundsResult.flagged = boundsResult.percent >= all.percent;
    return boundsResult;
}

// gray all bounds
Results
GrayAllBoundsExecute(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = GrayAllBounds(config, all, buf0, buf1);
    if (boundsResult.flagged && config.draw) {
        auto *pixels = new uint_fast8_t[config.byteLength]();
        std::copy(buf1, buf1 + config.byteLength, pixels);
        DrawGray(boundsResult, config, pixels);
        return {{}, boundsResult, {}, {}, {}, {}, {pixels, config.byteLength}};
    }
    return {{}, boundsResult, {}, {}, {}, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray region bounds
BoundsResult
GrayRegionBounds(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {region.name.data(), {region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false};// initialize results
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;
    boundsResult.flagged = boundsResult.percent >= region.percent;
    return boundsResult;
}

// gray region bounds
Results
GrayRegionBoundsExecute(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = GrayRegionBounds(config, region, buf0, buf1);
    if (boundsResult.flagged && config.draw) {
        auto *pixels = new uint_fast8_t[config.byteLength]();
        std::copy(buf1, buf1 + config.byteLength, pixels);
        DrawGray(boundsResult, config, pixels);
        return {{}, boundsResult, {}, {}, {}, {}, {pixels, config.byteLength}};
    }
    return {{}, boundsResult, {}, {}, {}, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray regions bounds
std::vector<BoundsResult>
GrayRegionsBounds(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BoundsResult> boundsResultVec;
    boundsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BoundsResult boundsResult = GrayRegionBounds(config, regions.regions[r], buf0, buf1);
        if (!boundsResult.flagged) continue;
        boundsResultVec.push_back(boundsResult);
    }
    boundsResultVec.shrink_to_fit();
    return boundsResultVec;
}

// gray regions bounds
Results
GrayRegionsBoundsExecute(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BoundsResult> boundsResultVec = GrayRegionsBounds(config, regions, buf0, buf1);
    if (!boundsResultVec.empty() && config.draw) {
        auto *pixels = new uint_fast8_t[config.byteLength]();
        std::copy(buf1, buf1 + config.byteLength, pixels);
        DrawGray(boundsResultVec, config, pixels);
        return {{}, {}, {}, {}, boundsResultVec, {}, {pixels, config.byteLength}};
    }
    return {{}, {}, {}, {}, boundsResultVec, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all blobs
BlobsResult
GrayAllBlobs(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {all.name.data(), {config.width - 1, 0, config.height - 1, 0}, 0, false, std::vector<Blob>()};// initialize results
    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int_fast32_t[]> up(new int_fast32_t[config.pixelCount]);
    // get pointer
    auto *labels = up.get();
    // all elements set to -1 will be labeled while -2 will be ignored
    for (uint_fast32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint_fast32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                SetMin(x, blobsResult.bounds.minX);
                SetMax(x, blobsResult.bounds.maxX);
                SetMin(y, blobsResult.bounds.minY);
                SetMax(y, blobsResult.bounds.maxY);
                ++blobsResult.percent;
            }
        }
    }
    // calculate percent size of blobbed pixels
    blobsResult.percent = 100 * blobsResult.percent / config.pixelCount;
    // initial percent level has been met, check the sizes of blobs
    if (blobsResult.percent >= all.percent) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(config, blobsResult.bounds, labels);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, {0, {blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}, 0, false});
        // count and group labels
        for (uint_fast32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {
                if (labels[p] == -2) continue;// ignored(-2) or unlabeled(-1)
                Blob &blob = blobsResult.blobs[labels[p]];
                SetMin(x, blob.bounds.minX);
                SetMax(x, blob.bounds.maxX);
                SetMin(y, blob.bounds.minY);
                SetMax(y, blob.bounds.maxY);
                ++blob.percent;
            }
        }
        // convert blob size to percent and check against threshold and flag
        for (uint_fast32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobsResult.blobs[b];
            blob.percent = 100 * blob.percent / config.pixelCount;
            if (all.percent > blob.percent) continue;
            blob.label = b;
            blobsResult.flagged = blob.flagged = true;
        }
    }
    return blobsResult;
}

// gray all blobs
Results
GrayAllBlobsExecute(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = GrayAllBlobs(config, all, buf0, buf1);
    if (blobsResult.flagged && config.draw) {
        auto *pixels = new uint_fast8_t[config.byteLength]();
        std::copy(buf1, buf1 + config.byteLength, pixels);
        DrawGray(blobsResult, config, pixels);
        return {{}, {}, blobsResult, {}, {}, {}, {pixels, config.byteLength}};
    }
    return {{}, {}, blobsResult, {}, {}, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray region blobs
BlobsResult
GrayRegionBlobs(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {region.name.data(), {region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false, std::vector<Blob>()};// initialize results
    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int_fast32_t[]> up(new int_fast32_t[config.pixelCount]);
    // get pointer
    auto *labels = up.get();
    // all elements set to -1 will be labeled while -2 will be ignored
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                SetMin(x, blobsResult.bounds.minX);
                SetMax(x, blobsResult.bounds.maxX);
                SetMin(y, blobsResult.bounds.minY);
                SetMax(y, blobsResult.bounds.maxY);
                ++blobsResult.percent;
            }

        }
    }
    // calculate percent size of blobbed pixels
    blobsResult.percent = 100 * blobsResult.percent / region.bitsetCount;
    // percent level has been met, check the sizes of blobs
    if (blobsResult.percent >= region.percent) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(config, blobsResult.bounds, labels);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, {0, {blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}, 0, false});
        // count and group labels
        for (uint_fast32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {
                if (labels[p] == -2) continue;// ignored(-2) or unlabeled(-1)
                Blob &blob = blobsResult.blobs[labels[p]];
                SetMin(x, blob.bounds.minX);
                SetMax(x, blob.bounds.maxX);
                SetMin(y, blob.bounds.minY);
                SetMax(y, blob.bounds.maxY);
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

// gray region blobs
Results
GrayRegionBlobsExecute(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = GrayRegionBlobs(config, region, buf0, buf1);
    if (blobsResult.flagged && config.draw) {
        auto *pixels = new uint_fast8_t[config.byteLength]();
        std::copy(buf1, buf1 + config.byteLength, pixels);
        DrawGray(blobsResult, config, pixels);
        return {{}, {}, blobsResult, {}, {}, {}, {pixels, config.byteLength}};
    }
    return {{}, {}, blobsResult, {}, {}, {}, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray regions blobs
std::vector<BlobsResult>
GrayRegionsBlobs(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BlobsResult> blobsResultVec;
    blobsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BlobsResult blobsResult = GrayRegionBlobs(config, regions.regions[r], buf0, buf1);
        if (!blobsResult.flagged) continue;
        blobsResultVec.push_back(blobsResult);
    }
    blobsResultVec.shrink_to_fit();
    return blobsResultVec;
}

// gray regions blobs
Results
GrayRegionsBlobsExecute(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    std::vector<BlobsResult> blobsResultVec = GrayRegionsBlobs(config, regions, buf0, buf1);
    if (!blobsResultVec.empty() && config.draw) {
        auto *pixels = new uint_fast8_t[config.byteLength]();
        std::copy(buf1, buf1 + config.byteLength, pixels);
        DrawGray(blobsResultVec, config, pixels);
        return {{}, {}, {}, {}, {}, blobsResultVec, {pixels, config.byteLength}};
    }
    return {{}, {}, {}, {}, {}, blobsResultVec, {}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all percent
PercentResult
RgbAllPercent(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {all.name.data(), 0, false};// initialize results
    for (uint_fast32_t p = 0; p < config.pixelCount; ++p) {
        if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
        ++percentResult.percent;
    }
    percentResult.percent = 100 * percentResult.percent / config.pixelCount;
    percentResult.flagged = percentResult.percent >= all.percent;
    return percentResult;
}

// rgb region percent
PercentResult
RgbRegionPercent(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    PercentResult percentResult = {region.name.data(), 0, false};// initialize results
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            ++percentResult.percent;
        }
    }
    percentResult.percent = 100 * percentResult.percent / region.bitsetCount;
    percentResult.flagged = percentResult.percent >= region.percent;
    return percentResult;
}

// rgb regions percent
std::vector<PercentResult>
RgbRegionsPercent(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<PercentResult> percentResultVec;
    percentResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        PercentResult percentResult = RgbRegionPercent(config, regions.regions[r], buf0, buf1);
        if (!percentResult.flagged) continue;
        percentResultVec.push_back(percentResult);
    }
    percentResultVec.shrink_to_fit();
    return percentResultVec;
}

// rgb all bounds
BoundsResult
RgbAllBounds(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {all.name.data(), {config.width - 1, 0, config.height - 1, 0}, 0, false};// initialize results
    for (uint_fast32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint_fast32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / config.pixelCount;
    boundsResult.flagged = boundsResult.percent >= all.percent;
    return boundsResult;
}

// rgb region bounds
BoundsResult
RgbRegionBounds(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BoundsResult boundsResult = {region.name.data(), {region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false};// initialize results
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }
    boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;
    boundsResult.flagged = boundsResult.percent >= region.percent;
    return boundsResult;
}

// rgb regions bounds
std::vector<BoundsResult>
RgbRegionsBounds(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BoundsResult> boundsResultVec;
    boundsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BoundsResult boundsResult = RgbRegionBounds(config, regions.regions[r], buf0, buf1);
        if (!boundsResult.flagged) continue;
        boundsResultVec.push_back(boundsResult);
    }
    boundsResultVec.shrink_to_fit();
    return boundsResultVec;
}

// rgb all blobs
BlobsResult
RgbAllBlobs(const Config &config, const All &all, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {all.name.data(), {config.width - 1, 0, config.height - 1, 0}, 0, false, std::vector<Blob>()};// initialize results
    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int_fast32_t[]> up(new int_fast32_t[config.pixelCount]);
    // get pointer
    auto *labels = up.get();
    // all elements set to -1 will be labeled while -2 will be ignored
    for (uint_fast32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint_fast32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                SetMin(x, blobsResult.bounds.minX);
                SetMax(x, blobsResult.bounds.maxX);
                SetMin(y, blobsResult.bounds.minY);
                SetMax(y, blobsResult.bounds.maxY);
                ++blobsResult.percent;
            }
        }
    }
    // calculate percent size of blobbed pixels
    blobsResult.percent = 100 * blobsResult.percent / config.pixelCount;
    // initial percent level has been met, check the sizes of blobs
    if (blobsResult.percent >= all.percent) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(config, blobsResult.bounds, labels);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, {0, {blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}, 0, false});
        // count and group labels
        for (uint_fast32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {
                if (labels[p] == -2) continue;// ignored(-2) or unlabeled(-1)
                Blob &blob = blobsResult.blobs[labels[p]];
                SetMin(x, blob.bounds.minX);
                SetMax(x, blob.bounds.maxX);
                SetMin(y, blob.bounds.minY);
                SetMax(y, blob.bounds.maxY);
                ++blob.percent;
            }
        }
        // convert blob size to percent and check against threshold and flag
        for (uint_fast32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobsResult.blobs[b];
            blob.percent = 100 * blob.percent / config.pixelCount;
            if (all.percent > blob.percent) continue;
            blob.label = b;
            blobsResult.flagged = blob.flagged = true;
        }
    }
    return blobsResult;
}



// rgb region blobs
BlobsResult
RgbRegionBlobs(const Config &config, const Region &region, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    BlobsResult blobsResult = {region.name.data(), {region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false, std::vector<Blob>()};// initialize results
    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int_fast32_t[]> up(new int_fast32_t[config.pixelCount]);
    // get pointer
    auto *labels = up.get();
    // all elements set to -1 will be labeled while -2 will be ignored
    for (uint_fast32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint_fast32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                SetMin(x, blobsResult.bounds.minX);
                SetMax(x, blobsResult.bounds.maxX);
                SetMin(y, blobsResult.bounds.minY);
                SetMax(y, blobsResult.bounds.maxY);
                ++blobsResult.percent;
            }

        }
    }
    // calculate percent size of blobbed pixels
    blobsResult.percent = 100 * blobsResult.percent / region.bitsetCount;
    // percent level has been met, check the sizes of blobs
    if (blobsResult.percent >= region.percent) {
        // assign label to each indexed pixel that has a -1 instead of -2, returns the total unique labels count
        uint_fast32_t blobCount = LabelImage(config, blobsResult.bounds, labels);
        // create vector using blobCount size
        blobsResult.blobs = std::vector<Blob>(blobCount, {0, {blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}, 0, false});
        // count and group labels
        for (uint_fast32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {
            for (uint_fast32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {
                if (labels[p] == -2) continue;// ignored(-2) or unlabeled(-1)
                Blob &blob = blobsResult.blobs[labels[p]];
                SetMin(x, blob.bounds.minX);
                SetMax(x, blob.bounds.maxX);
                SetMin(y, blob.bounds.minY);
                SetMax(y, blob.bounds.maxY);
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
RgbRegionsBlobs(const Config &config, const Regions &regions, const uint_fast8_t *buf0, const uint_fast8_t *buf1) {
    auto regionsLen = regions.regions.size();
    std::vector<BlobsResult> blobsResultVec;
    blobsResultVec.reserve(regionsLen);
    for (uint_fast32_t r = 0; r < regionsLen; ++r) {
        BlobsResult blobsResult = RgbRegionBlobs(config, regions.regions[r], buf0, buf1);
        if (!blobsResult.flagged) continue;
        blobsResultVec.push_back(blobsResult);
    }
    blobsResultVec.shrink_to_fit();
    return blobsResultVec;
}