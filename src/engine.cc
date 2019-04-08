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
uint32_t
EngineType(const uint32_t depth, const uint32_t regionsLength, const std::string &response, const bool sync) {
    uint32_t value = 0;
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
    const uint32_t bitsetCount = regionJs.Get("bitsetCount").As<Napi::Number>().Uint32Value();
    const uint32_t difference = regionJs.Get("difference").As<Napi::Number>().Uint32Value();
    const uint32_t percent = regionJs.Get("percent").As<Napi::Number>().Uint32Value();
    const uint32_t minX = regionJs.Get("minX").As<Napi::Number>().Uint32Value();
    const uint32_t maxX = regionJs.Get("maxX").As<Napi::Number>().Uint32Value();
    const uint32_t minY = regionJs.Get("minY").As<Napi::Number>().Uint32Value();
    const uint32_t maxY = regionJs.Get("maxY").As<Napi::Number>().Uint32Value();
    return Region{name, bitset, bitsetCount, difference, percent, Bounds{minX, maxX, minY, maxY}};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(const Napi::Array &regionsJs) {
    std::vector<Region> regionVec;
    const size_t regionsLen = regionsJs.Length();
    regionVec.reserve(regionsLen);
    for (uint32_t r = 0; r < regionsLen; ++r) {
        regionVec.emplace_back(RegionJsToCpp(regionsJs.Get(r).As<Napi::Object>()));
    }
    return regionVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all percent
void
GrayAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to percentResult
    PercentResult &percentResult = results.percentResult;

    // configure percentResult
    percentResult.name = all.name.data();
    percentResult.percent = 0;
    percentResult.flagged = false;

    // loop pixels while tracking percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) continue;
            ++percentResult.percent;
        }
    }

    // calculate percent size of changed pixels
    percentResult.percent = 100 * percentResult.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > percentResult.percent) return;

    //needed for callback
    percentResult.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//gray single region percent
void
GrayRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to percentResult
    PercentResult &percentResult = results.percentResult;

    // configure percentResult
    percentResult.name = region.name.data();
    percentResult.percent = 0;
    percentResult.flagged = false;

    // loop pixels while tracking percent
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            ++percentResult.percent;
        }
    }

    // calculate percent size of changed pixels
    percentResult.percent = 100 * percentResult.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > percentResult.percent) return;

    //needed for callback
    percentResult.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray multi regions percent
void
GrayRegionsPercentExecute(const Config &config, const Regions &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to regions vector
    const std::vector<Region> &regionsVec = regions.regions;

    // use size of regions to determine loop
    const size_t regionsLength = regionsVec.size();

    // get reference to percentResult vector
    std::vector<PercentResult> &percentResultVec = results.percentResults;

    // reserve memory for percentResult vector
    percentResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regionsVec[r];

        // create percentResults in vector
        percentResultVec.emplace_back(PercentResult{region.name.data(), 0, false});

        // get reference to percentResult that was just pushed into vector
        PercentResult &percentResult = percentResultVec[r];

        // loop pixels while tracking percent
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
                ++percentResult.percent;
            }
        }

        // calculate percent size of changed pixels
        percentResult.percent = 100 * percentResult.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > percentResult.percent) continue;

        // set flagged to true
        percentResult.flagged = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all bounds
void
GrayAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to boundsResult
    BoundsResult &boundsResult = results.boundsResult;

    // configure boundsResult
    boundsResult.name = all.name.data();
    boundsResult.bounds = Bounds{config.width - 1, 0, config.height - 1, 0};
    boundsResult.percent = 0;
    boundsResult.flagged = false;

    // loop pixels while tracking bounds and percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }

    // calculate percent size of changed pixels
    boundsResult.percent = 100 * boundsResult.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > boundsResult.percent) return;

    //needed for callback
    boundsResult.flagged = true;

    // draw bounds into pixels
    if (config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // write bounds into pixels
        SetGrayPixels(boundsResult.bounds, config, results.pixels.ptr);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//gray single region bounds
void
GrayRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to boundsResult
    BoundsResult &boundsResult = results.boundsResult;

    // configure boundsResult
    boundsResult.name = region.name.data();
    boundsResult.bounds = Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY};
    boundsResult.percent = 0;
    boundsResult.flagged = false;

    // loop pixels while tracking percent and bounds
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }

    // calculate percent size of changed pixels
    boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > boundsResult.percent) return;

    //needed for callback
    boundsResult.flagged = true;

    // draw bounds into pixels
    if (config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // write bounds into pixels
        SetGrayPixels(boundsResult.bounds, config, results.pixels.ptr);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray multi regions bounds
void
GrayRegionsBoundsExecute(const Config &config, const Regions &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // get reference to regions vector
    const std::vector<Region> &regionsVec = regions.regions;

    // use size of regions to determine loop
    const size_t regionsLength = regionsVec.size();

    // get reference to boundsResult vector
    std::vector<BoundsResult> &boundsResultVec = results.boundsResults;

    // reserve memory for boundsResult vector
    boundsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regionsVec[r];

        // create boundsResults in vector
        boundsResultVec.emplace_back(BoundsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false});

        // get reference to boundsResult that was just pushed into vector
        BoundsResult &boundsResult = boundsResultVec[r];

        // loop pixels while tracking pixels and bounds
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
                SetMin(x, boundsResult.bounds.minX);
                SetMax(x, boundsResult.bounds.maxX);
                SetMin(y, boundsResult.bounds.minY);
                SetMax(y, boundsResult.bounds.maxY);
                ++boundsResult.percent;
            }
        }

        // calculate percent size of changed pixels
        boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > boundsResult.percent) continue;

        // set flagged to true
        flagged = boundsResult.flagged = true;
    }

    //must be outside loop since all bounds will be draw to same pixels
    if (flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobsResults vector
        for (auto &boundsResult : boundsResultVec) {

            // skip if not flagged
            if (!boundsResult.flagged) continue;

            // write bounds into pixels
            SetGrayPixels(boundsResult.bounds, config, results.pixels.ptr);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all blobs
void
GrayAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to blobsResult
    BlobsResult &blobsResult = results.blobsResult;

    // configure blobsResult
    blobsResult.name = all.name.data();
    blobsResult.bounds = Bounds{config.width - 1, 0, config.height - 1, 0};
    blobsResult.percent = 0;
    blobsResult.flagged = false;

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
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

    // if percent threshold is not met, skip blobbing the pixels
    if (all.percent > blobsResult.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, blobsResult.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = blobsResult.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            SetMin(x, blob.bounds.minX);
            SetMax(x, blob.bounds.maxX);
            SetMin(y, blob.bounds.minY);
            SetMax(y, blob.bounds.maxY);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / config.pixelCount;
        if (all.percent > blob.percent) continue;
        blob.label = b;
        blobsResult.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (blobsResult.flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobs and draw bounds in pixels
        for (auto &blob : blobsResult.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetGrayPixels(blob.bounds, config, results.pixels.ptr);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray single region blobs
void
GrayRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to blobsResult
    BlobsResult &blobsResult = results.blobsResult;

    // configure blobsResult
    blobsResult.name = region.name.data();
    blobsResult.bounds = Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY};
    blobsResult.percent = 0;
    blobsResult.flagged = false;

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
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

    // if percent threshold is not met, skip blobbing the pixels
    if (region.percent > blobsResult.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, blobsResult.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = blobsResult.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            SetMin(x, blob.bounds.minX);
            SetMax(x, blob.bounds.maxX);
            SetMin(y, blob.bounds.minY);
            SetMax(y, blob.bounds.maxY);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / region.bitsetCount;
        if (region.percent > blob.percent) continue;
        blob.label = b;
        blobsResult.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (blobsResult.flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobs and draw bounds in pixels
        for (auto &blob : blobsResult.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetGrayPixels(blob.bounds, config, results.pixels.ptr);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray multi regions blobs
void
GrayRegionsBlobsExecute(const Config &config, const Regions &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to regions vector
    const std::vector<Region> &regionsVec = regions.regions;

    // use size of regions to determine loop
    const size_t regionsLength = regionsVec.size();

    // get reference to blobsResult vector
    std::vector<BlobsResult> &blobsResultVec = results.blobsResults;

    // reserve memory for blobsResult vector
    blobsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regionsVec[r];

        // create blobsResultsEx in vector
        blobsResultVec.emplace_back(BlobsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false, std::vector<Blob>()});

        // get reference to blobsResult that was just pushed into vector
        BlobsResult &blobsResult = blobsResultVec[r];

        // all pixels set to -1 will those set to -2 will be ignored
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
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

        // if percent threshold is not met, skip blobbing the pixels
        if (region.percent > blobsResult.percent) continue;

        // assign label to each pixel and return count of unique labels
        uint32_t blobCount = LabelImage(config, blobsResult.bounds, labels);

        // get reference to blobs vector
        std::vector<Blob> &blobs = blobsResult.blobs;

        // fill vector with blobs using the previous bounds results
        blobs.assign(blobCount, Blob{Bounds{blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}});

        // loop rows within upper and lower bounds
        for (uint32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {

            // loop columns within left and right bounds
            for (uint32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {

                // get pixel label as blob index
                int32_t b = labels[p];

                // skip unlabeled pixel
                if (b < 0) continue;

                // get blob and update its data
                Blob &blob = blobs[b];
                SetMin(x, blob.bounds.minX);
                SetMax(x, blob.bounds.maxX);
                SetMin(y, blob.bounds.minY);
                SetMax(y, blob.bounds.maxY);
                ++blob.percent;
            }
        }

        // convert blob size to percent and check against threshold and flag
        for (uint32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobs[b];
            blob.percent = 100 * blob.percent / region.bitsetCount;
            if (region.percent > blob.percent) continue;
            blob.label = b;
            flagged = blobsResult.flagged = blob.flagged = true;
        }
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobsResults vector
        for (auto &blobsResult : blobsResultVec) {

            // skip if not flagged
            if (!blobsResult.flagged) continue;

            // iterate blobs and draw bounds in pixels
            for (auto &blob : blobsResult.blobs) {

                // skip if not flagged
                if (!blob.flagged) continue;

                // write bounds into pixels
                SetGrayPixels(blob.bounds, config, results.pixels.ptr);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all percent
void
RgbAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to percentResult
    PercentResult &percentResult = results.percentResult;

    // configure percentResult
    percentResult.name = all.name.data();
    percentResult.percent = 0;
    percentResult.flagged = false;

    // loop pixels while tracking percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            ++percentResult.percent;
        }
    }

    // calculate percent size of changed pixels
    percentResult.percent = 100 * percentResult.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > percentResult.percent) return;

    //needed for callback
    percentResult.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//rgb single region percent
void
RgbRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to percentResult
    PercentResult &percentResult = results.percentResult;

    // configure percentResult
    percentResult.name = region.name.data();
    percentResult.percent = 0;
    percentResult.flagged = false;

    // loop pixels while tracking percent
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            ++percentResult.percent;
        }
    }

    // calculate percent size of changed pixels
    percentResult.percent = 100 * percentResult.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > percentResult.percent) return;

    //needed for callback
    percentResult.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb multi regions percent
void
RgbRegionsPercentExecute(const Config &config, const Regions &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to regions vector
    const std::vector<Region> &regionsVec = regions.regions;

    // use size of regions to determine loop
    const size_t regionsLength = regionsVec.size();

    // get reference to percentResult vector
    std::vector<PercentResult> &percentResultVec = results.percentResults;

    // reserve memory for percentResult vector
    percentResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regionsVec[r];

        // create percentResults in vector
        percentResultVec.emplace_back(PercentResult{region.name.data(), 0, false});

        // get reference to percentResult that was just pushed into vector
        PercentResult &percentResult = percentResultVec[r];

        // loop pixels while tracking percent
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
                ++percentResult.percent;
            }
        }

        // calculate percent size of changed pixels
        percentResult.percent = 100 * percentResult.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > percentResult.percent) continue;

        // set flagged to true
        percentResult.flagged = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all bounds
void
RgbAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to boundsResult
    BoundsResult &boundsResult = results.boundsResult;

    // configure boundsResult
    boundsResult.name = all.name.data();
    boundsResult.bounds = Bounds{config.width - 1, 0, config.height - 1, 0};
    boundsResult.percent = 0;
    boundsResult.flagged = false;

    // loop pixels while tracking bounds and percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }

    // calculate percent size of changed pixels
    boundsResult.percent = 100 * boundsResult.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > boundsResult.percent) return;

    //needed for callback
    boundsResult.flagged = true;

    // draw bounds into pixels
    if (config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // write bounds into pixels
        SetRgbPixels(boundsResult.bounds, config, results.pixels.ptr);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//rgb single region bounds
void
RgbRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to boundsResult
    BoundsResult &boundsResult = results.boundsResult;

    // configure boundsResult
    boundsResult.name = region.name.data();
    boundsResult.bounds = Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY};
    boundsResult.percent = 0;
    boundsResult.flagged = false;

    // loop pixels while tracking percent and bounds
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            SetMin(x, boundsResult.bounds.minX);
            SetMax(x, boundsResult.bounds.maxX);
            SetMin(y, boundsResult.bounds.minY);
            SetMax(y, boundsResult.bounds.maxY);
            ++boundsResult.percent;
        }
    }

    // calculate percent size of changed pixels
    boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > boundsResult.percent) return;

    //needed for callback
    boundsResult.flagged = true;

    // draw bounds into pixels
    if (config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // write bounds into pixels
        SetRgbPixels(boundsResult.bounds, config, results.pixels.ptr);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb multi regions bounds
void
RgbRegionsBoundsExecute(const Config &config, const Regions &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // get reference to regions vector
    const std::vector<Region> &regionsVec = regions.regions;

    // use size of regions to determine loop
    const size_t regionsLength = regionsVec.size();

    // get reference to boundsResult vector
    std::vector<BoundsResult> &boundsResultVec = results.boundsResults;

    // reserve memory for boundsResult vector
    boundsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regionsVec[r];

        // create boundsResults in vector
        boundsResultVec.emplace_back(BoundsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false});

        // get reference to boundsResult that was just pushed into vector
        BoundsResult &boundsResult = boundsResultVec[r];

        // loop pixels while tracking pixels and bounds
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
                SetMin(x, boundsResult.bounds.minX);
                SetMax(x, boundsResult.bounds.maxX);
                SetMin(y, boundsResult.bounds.minY);
                SetMax(y, boundsResult.bounds.maxY);
                ++boundsResult.percent;
            }
        }

        // calculate percent size of changed pixels
        boundsResult.percent = 100 * boundsResult.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > boundsResult.percent) continue;

        // set flagged to true
        flagged = boundsResult.flagged = true;
    }

    //must be outside loop since all bounds will be draw to same pixels
    if (flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobsResults vector
        for (auto &boundsResult : boundsResultVec) {

            // skip if not flagged
            if (!boundsResult.flagged) continue;

            // write bounds into pixels
            SetRgbPixels(boundsResult.bounds, config, results.pixels.ptr);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all blobs
void
RgbAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to blobsResult
    BlobsResult &blobsResult = results.blobsResult;

    // configure blobsResult
    blobsResult.name = all.name.data();
    blobsResult.bounds = Bounds{config.width - 1, 0, config.height - 1, 0};
    blobsResult.percent = 0;
    blobsResult.flagged = false;

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
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

    // if percent threshold is not met, skip blobbing the pixels
    if (all.percent > blobsResult.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, blobsResult.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = blobsResult.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            SetMin(x, blob.bounds.minX);
            SetMax(x, blob.bounds.maxX);
            SetMin(y, blob.bounds.minY);
            SetMax(y, blob.bounds.maxY);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / config.pixelCount;
        if (all.percent > blob.percent) continue;
        blob.label = b;
        blobsResult.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (blobsResult.flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobs and draw bounds in pixels
        for (auto &blob : blobsResult.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetRgbPixels(blob.bounds, config, results.pixels.ptr);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb single region blobs
void
RgbRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to blobsResult
    BlobsResult &blobsResult = results.blobsResult;

    // configure blobsResult
    blobsResult.name = region.name.data();
    blobsResult.bounds = Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY};
    blobsResult.percent = 0;
    blobsResult.flagged = false;

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
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

    // if percent threshold is not met, skip blobbing the pixels
    if (region.percent > blobsResult.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, blobsResult.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = blobsResult.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            SetMin(x, blob.bounds.minX);
            SetMax(x, blob.bounds.maxX);
            SetMin(y, blob.bounds.minY);
            SetMax(y, blob.bounds.maxY);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / region.bitsetCount;
        if (region.percent > blob.percent) continue;
        blob.label = b;
        blobsResult.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (blobsResult.flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobs and draw bounds in pixels
        for (auto &blob : blobsResult.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetRgbPixels(blob.bounds, config, results.pixels.ptr);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb multi regions blobs
void
RgbRegionsBlobsExecute(const Config &config, const Regions &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to regions vector
    const std::vector<Region> &regionsVec = regions.regions;

    // use size of regions to determine loop
    const size_t regionsLength = regionsVec.size();

    // get reference to blobsResult vector
    std::vector<BlobsResult> &blobsResultVec = results.blobsResults;

    // reserve memory for blobsResult vector
    blobsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regionsVec[r];

        // create blobsResultsEx in vector
        blobsResultVec.emplace_back(BlobsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}, 0, false, std::vector<Blob>()});

        // get reference to blobsResult that was just pushed into vector
        BlobsResult &blobsResult = blobsResultVec[r];

        // all pixels set to -1 will those set to -2 will be ignored
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
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

        // if percent threshold is not met, skip blobbing the pixels
        if (region.percent > blobsResult.percent) continue;

        // assign label to each pixel and return count of unique labels
        uint32_t blobCount = LabelImage(config, blobsResult.bounds, labels);

        // get reference to blobs vector
        std::vector<Blob> &blobs = blobsResult.blobs;

        // fill vector with blobs using the previous bounds results
        blobs.assign(blobCount, Blob{Bounds{blobsResult.bounds.maxX, blobsResult.bounds.minX, blobsResult.bounds.maxY, blobsResult.bounds.minY}});

        // loop rows within upper and lower bounds
        for (uint32_t y = blobsResult.bounds.minY; y <= blobsResult.bounds.maxY; ++y) {

            // loop columns within left and right bounds
            for (uint32_t x = blobsResult.bounds.minX, p = y * config.width + x; x <= blobsResult.bounds.maxX; ++x, ++p) {

                // get pixel label as blob index
                int32_t b = labels[p];

                // skip unlabeled pixel
                if (b < 0) continue;

                // get blob and update its data
                Blob &blob = blobs[b];
                SetMin(x, blob.bounds.minX);
                SetMax(x, blob.bounds.maxX);
                SetMin(y, blob.bounds.minY);
                SetMax(y, blob.bounds.maxY);
                ++blob.percent;
            }
        }

        // convert blob size to percent and check against threshold and flag
        for (uint32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobs[b];
            blob.percent = 100 * blob.percent / region.bitsetCount;
            if (region.percent > blob.percent) continue;
            blob.label = b;
            flagged = blobsResult.flagged = blob.flagged = true;
        }
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (flagged && config.draw) {

        // todo use shared pointer instead of relying on napi buffer to clean up(untested idea)

        // set pixels data in results
        results.pixels.ptr = new uint8_t[config.byteLength]();
        results.pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, results.pixels.ptr);

        // iterate blobsResults vector
        for (auto &blobsResult : blobsResultVec) {

            // skip if not flagged
            if (!blobsResult.flagged) continue;

            // iterate blobs and draw bounds in pixels
            for (auto &blob : blobsResult.blobs) {

                // skip if not flagged
                if (!blob.flagged) continue;

                // write bounds into pixels
                SetRgbPixels(blob.bounds, config, results.pixels.ptr);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////