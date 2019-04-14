#include "engine.h"
#include "ccl.h"
#include "results.h"
#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

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
    const std::string name = regionJs.Get("name").As<Napi::String>().Utf8Value();
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
GrayRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to percentResult vector
    std::vector<PercentResult> &percentResultVec = results.percentResults;

    // reserve memory for percentResult vector
    percentResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create percentResults in vector
        percentResultVec.emplace_back(PercentResult{region.name.data()});

        // get reference to percentResult that was just pushed into vector
        PercentResult &percentResult = percentResultVec.back();

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
GrayRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to boundsResult vector
    std::vector<BoundsResult> &boundsResultVec = results.boundsResults;

    // reserve memory for boundsResult vector
    boundsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create boundsResults in vector
        boundsResultVec.emplace_back(BoundsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to boundsResult that was just pushed into vector
        BoundsResult &boundsResult = boundsResultVec.back();

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
GrayRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to blobsResult vector
    std::vector<BlobsResult> &blobsResultVec = results.blobsResults;

    // reserve memory for blobsResult vector
    blobsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create blobsResultsEx in vector
        blobsResultVec.emplace_back(BlobsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to blobsResult that was just pushed into vector
        BlobsResult &blobsResult = blobsResultVec.back();

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

// rgb all percent
void
RgbAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // get reference to percentResult
    PercentResult &percentResult = results.percentResult;

    // configure percentResult
    percentResult.name = all.name.data();

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
RgbRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to percentResult vector
    std::vector<PercentResult> &percentResultVec = results.percentResults;

    // reserve memory for percentResult vector
    percentResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create percentResults in vector
        percentResultVec.emplace_back(PercentResult{region.name.data()});

        // get reference to percentResult that was just pushed into vector
        PercentResult &percentResult = percentResultVec.back();

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
RgbRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to boundsResult vector
    std::vector<BoundsResult> &boundsResultVec = results.boundsResults;

    // reserve memory for boundsResult vector
    boundsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create boundsResults in vector
        boundsResultVec.emplace_back(BoundsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to boundsResult that was just pushed into vector
        BoundsResult &boundsResult = boundsResultVec.back();

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
RgbRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to blobsResult vector
    std::vector<BlobsResult> &blobsResultVec = results.blobsResults;

    // reserve memory for blobsResult vector
    blobsResultVec.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create blobsResultsEx in vector
        blobsResultVec.emplace_back(BlobsResult{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to blobsResult that was just pushed into vector
        BlobsResult &blobsResult = blobsResultVec.back();

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

// determine engine type
uint32_t
EngineType(const uint32_t depth, const uint32_t regionsLength, const std::string &response) {
    uint32_t value = 0;
    value += depth == 4 || depth == 3 ? 100 : 0;
    value += regionsLength > 1 ? 20 : regionsLength == 1 ? 10 : 0;
    value += response == "blobs" ? 2 : response == "bounds" ? 1 : 0;
    return value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// set execute and callback functions
void
SetFunctions(const Napi::Object &configObj, ExecuteFunc &executeFunc, CallbackFunc &callbackFunc) {

    // width, height, depth. required.
    const uint32_t width = configObj.Get("width").As<Napi::Number>().Uint32Value();
    const uint32_t height = configObj.Get("height").As<Napi::Number>().Uint32Value();
    const uint32_t depth = configObj.Get("depth").As<Napi::Number>().Uint32Value();

    // difference 1-255. optional. default 1.
    const uint32_t difference = configObj.HasOwnProperty("difference") ? configObj.Get("difference").As<Napi::Number>().Uint32Value() : 1;

    // percent 1 - 100. optional. default 1.
    const uint32_t percent = configObj.HasOwnProperty("percent") ? configObj.Get("percent").As<Napi::Number>().Uint32Value() : 1;

    // draw pixels. optional. default false.
    const bool draw = configObj.HasOwnProperty("draw") && configObj.Get("draw").As<Napi::Boolean>().Value();

    // figure out target and response. optional. defaults to target "all" and response "percent"
    const uint32_t regionsLength = configObj.HasOwnProperty("regions") && configObj.Get("regions").IsArray() && configObj.Get("regions").As<Napi::Array>().Length() > 0 ? configObj.Get("regions").As<Napi::Array>().Length() : 0;
    const std::string response = configObj.HasOwnProperty("response") ? configObj.Get("response").As<Napi::String>().Utf8Value() : "percent";

    // determine functions to assign
    const uint32_t engineType = EngineType(depth, regionsLength, response);

    // create Config struct
    const Config config = Config{width, height, depth, draw};

    switch (engineType) {
        case GRAY_ALL_PERCENT: {//0
            const All all = All{"all", difference, percent};
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayAllPercentExecute(config, {all}, buf0, buf1, results);};
            callbackFunc = &AllPercentCallback;
            break;
        }
        case GRAY_ALL_BOUNDS: {//1
            const All all = All{"all", difference, percent};
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayAllBoundsExecute(config, {all}, buf0, buf1, results);};
            callbackFunc = &AllBoundsCallback;
            break;
        }
        case GRAY_ALL_BLOBS:{//2
            const All all = All{"all", difference, percent};
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayAllBlobsExecute(config, {all}, buf0, buf1, results);};
            callbackFunc = &AllBlobsCallback;
            break;
        }
        case GRAY_REGION_PERCENT:{//10
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayRegionPercentExecute(config, region, buf0, buf1, results);};
            callbackFunc = &RegionPercentCallback;
            break;
        }
        case GRAY_REGION_BOUNDS:{//11
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayRegionBoundsExecute(config, region, buf0, buf1, results);};
            callbackFunc = &RegionBoundsCallback;
            break;
        }
        case GRAY_REGION_BLOBS:{//12
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayRegionBlobsExecute(config, region, buf0, buf1, results);};
            callbackFunc = &RegionBlobsCallback;
            break;
        }
        case GRAY_REGIONS_PERCENT:{//20
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayRegionsPercentExecute(config, regions, buf0, buf1, results);};
            callbackFunc = &RegionsPercentCallback;
            break;
        }
        case GRAY_REGIONS_BOUNDS:{//21
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayRegionsBoundsExecute(config, regions, buf0, buf1, results);};
            callbackFunc = &RegionsBoundsCallback;
            break;
        }
        case GRAY_REGIONS_BLOBS:{//22
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {GrayRegionsBlobsExecute(config, regions, buf0, buf1, results);};
            callbackFunc = &RegionsBlobsCallback;
            break;
        }
        case RGB_ALL_PERCENT: {//100
            const All all = All{"all", difference, percent};
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbAllPercentExecute(config, {all}, buf0, buf1, results);};
            callbackFunc = &AllPercentCallback;
            break;
        }
        case RGB_ALL_BOUNDS: {//101
            const All all = All{"all", difference, percent};
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbAllBoundsExecute(config, {all}, buf0, buf1, results);};
            callbackFunc = &AllBoundsCallback;
            break;
        }
        case RGB_ALL_BLOBS:{//102
            const All all = All{"all", difference, percent};
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbAllBlobsExecute(config, {all}, buf0, buf1, results);};
            callbackFunc = &AllBlobsCallback;
            break;
        }
        case RGB_REGION_PERCENT:{//110
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbRegionPercentExecute(config, region, buf0, buf1, results);};
            callbackFunc = &RegionPercentCallback;
            break;
        }
        case RGB_REGION_BOUNDS:{//111
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbRegionBoundsExecute(config, region, buf0, buf1, results);};
            callbackFunc = &RegionBoundsCallback;
            break;
        }
        case RGB_REGION_BLOBS:{//112
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbRegionBlobsExecute(config, region, buf0, buf1, results);};
            callbackFunc = &RegionBlobsCallback;
            break;
        }
        case RGB_REGIONS_PERCENT:{//20
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbRegionsPercentExecute(config, regions, buf0, buf1, results);};
            callbackFunc = &RegionsPercentCallback;
            break;
        }
        case RGB_REGIONS_BOUNDS:{//21
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbRegionsBoundsExecute(config, regions, buf0, buf1, results);};
            callbackFunc = &RegionsBoundsCallback;
            break;
        }
        case RGB_REGIONS_BLOBS:{//22
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [=](const uint8_t *buf0, const uint8_t *buf1, Results &results) {RgbRegionsBlobsExecute(config, regions, buf0, buf1, results);};
            callbackFunc = &RegionsBlobsCallback;
            break;
        }
        default:
            throw Napi::Error::New(configObj.Env(), "Engine not found for type " + std::to_string(engineType));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////