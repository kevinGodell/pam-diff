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
GrayAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{all.name.data()});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) continue;
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > result.percent) return;

    //needed for callback
    result.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//gray single region percent
void
GrayRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{region.name.data()});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking percent
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > result.percent) return;

    //needed for callback
    result.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray multi regions percent
void
GrayRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create Result in vector
        results.emplace_back(Result{region.name.data()});

        // get reference to last Result in vector
        Result &result = results.back();

        // loop pixels while tracking percent
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
                ++result.percent;
            }
        }

        // calculate percent size of changed pixels
        result.percent = 100 * result.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > result.percent) continue;

        // set flagged to true
        result.flagged = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all bounds
void
GrayAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{all.name.data(), Bounds{config.width - 1, 0, config.height - 1, 0}});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking bounds and percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) continue;
            result.bounds.expandTo(x, y);
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > result.percent) return;

    //needed for callback
    result.flagged = true;

    // draw bounds into pixels
    if (config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // write bounds into pixels
        SetGrayPixels(result.bounds, config, pixels.ptr.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//gray single region bounds
void
GrayRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking percent and bounds
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
            result.bounds.expandTo(x, y);
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > result.percent) return;

    //needed for callback
    result.flagged = true;

    // draw bounds into pixels
    if (config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // write bounds into pixels
        SetGrayPixels(result.bounds, config, pixels.ptr.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray multi regions bounds
void
GrayRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create Result in vector
        results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to last Result in vector
        Result &result = results.back();

        // loop pixels while tracking pixels and bounds
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) continue;
                result.bounds.expandTo(x, y);
                ++result.percent;
            }
        }

        // calculate percent size of changed pixels
        result.percent = 100 * result.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > result.percent) continue;

        // set flagged to true
        flagged = result.flagged = true;
    }

    //must be outside loop since all bounds will be draw to same pixels
    if (flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate blobsResults vector
        for (auto &result : results) {

            // skip if not flagged
            if (!result.flagged) continue;

            // write bounds into pixels
            SetGrayPixels(result.bounds, config, pixels.ptr.get());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray all blobs
void
GrayAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{all.name.data(), Bounds{config.width - 1, 0, config.height - 1, 0}});

    // get reference to last Result in vector
    Result &result = results.back();

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > GrayDiff(buf0, buf1, p)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                result.bounds.expandTo(x, y);
                ++result.percent;
            }
        }
    }

    // calculate percent size of blobbed pixels
    result.percent = 100 * result.percent / config.pixelCount;

    // if percent threshold is not met, skip blobbing the pixels
    if (all.percent > result.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, result.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = result.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{result.bounds.maxX, result.bounds.minX, result.bounds.maxY, result.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = result.bounds.minY; y <= result.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = result.bounds.minX, p = y * config.width + x; x <= result.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            blob.bounds.expandTo(x, y);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / config.pixelCount;
        if (all.percent > blob.percent) continue;
        blob.label = b;
        result.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (result.flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate blobs and draw bounds in pixels
        for (auto &blob : result.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetGrayPixels(blob.bounds, config, pixels.ptr.get());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray single region blobs
void
GrayRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

    // get reference to last Result in vector
    Result &result = results.back();

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                result.bounds.expandTo(x, y);
                ++result.percent;
            }
        }
    }

    // calculate percent size of blobbed pixels
    result.percent = 100 * result.percent / region.bitsetCount;

    // if percent threshold is not met, skip blobbing the pixels
    if (region.percent > result.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, result.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = result.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{result.bounds.maxX, result.bounds.minX, result.bounds.maxY, result.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = result.bounds.minY; y <= result.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = result.bounds.minX, p = y * config.width + x; x <= result.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            blob.bounds.expandTo(x, y);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / region.bitsetCount;
        if (region.percent > blob.percent) continue;
        blob.label = b;
        result.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (result.flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate blobs and draw bounds in pixels
        for (auto &blob : result.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetGrayPixels(blob.bounds, config, pixels.ptr.get());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gray multi regions blobs
void
GrayRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create Result in vector
        results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to last Result in vector
        Result &result = results.back();

        // all pixels set to -1 will those set to -2 will be ignored
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > GrayDiff(buf0, buf1, p)) {
                    labels[p] = -2;// set to -2 to mark as pixel to ignore
                } else {
                    labels[p] = -1;// set to -1 to mark as pixel of interest
                    result.bounds.expandTo(x, y);
                    ++result.percent;
                }
            }
        }

        // calculate percent size of blobbed pixels
        result.percent = 100 * result.percent / region.bitsetCount;

        // if percent threshold is not met, skip blobbing the pixels
        if (region.percent > result.percent) continue;

        // assign label to each pixel and return count of unique labels
        uint32_t blobCount = LabelImage(config, result.bounds, labels);

        // get reference to blobs vector
        std::vector<Blob> &blobs = result.blobs;

        // fill vector with blobs using the previous bounds results
        blobs.assign(blobCount, Blob{Bounds{result.bounds.maxX, result.bounds.minX, result.bounds.maxY, result.bounds.minY}});

        // loop rows within upper and lower bounds
        for (uint32_t y = result.bounds.minY; y <= result.bounds.maxY; ++y) {

            // loop columns within left and right bounds
            for (uint32_t x = result.bounds.minX, p = y * config.width + x; x <= result.bounds.maxX; ++x, ++p) {

                // get pixel label as blob index
                int32_t b = labels[p];

                // skip unlabeled pixel
                if (b < 0) continue;

                // get blob and update its data
                Blob &blob = blobs[b];
                blob.bounds.expandTo(x, y);
                ++blob.percent;
            }
        }

        // convert blob size to percent and check against threshold and flag
        for (uint32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobs[b];
            blob.percent = 100 * blob.percent / region.bitsetCount;
            if (region.percent > blob.percent) continue;
            blob.label = b;
            flagged = result.flagged = blob.flagged = true;
        }
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate Result vector
        for (auto &result : results) {

            // skip if not flagged
            if (!result.flagged) continue;

            // iterate blobs and draw bounds in pixels
            for (auto &blob : result.blobs) {

                // skip if not flagged
                if (!blob.flagged) continue;

                // write bounds into pixels
                SetGrayPixels(blob.bounds, config, pixels.ptr.get());
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all percent
void
RgbAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result inside vector
    results.emplace_back(Result{all.name.data()});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > result.percent) return;

    //needed for callback
    result.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//rgb single region percent
void
RgbRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{region.name.data()});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking percent
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > result.percent) return;

    //needed for callback
    result.flagged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb multi regions percent
void
RgbRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create Result in vector
        results.emplace_back(Result{region.name.data()});

        // get reference to last Result in vector
        Result &result = results.back();

        // loop pixels while tracking percent
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
                ++result.percent;
            }
        }

        // calculate percent size of changed pixels
        result.percent = 100 * result.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > result.percent) continue;

        // set flagged to true
        result.flagged = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all bounds
void
RgbAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{all.name.data(), Bounds{config.width - 1, 0, config.height - 1, 0}});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking bounds and percent
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            result.bounds.expandTo(x, y);
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / config.pixelCount;

    // if percent threshold is not met, return
    if (all.percent > result.percent) return;

    //needed for callback
    result.flagged = true;

    // draw bounds into pixels
    if (config.draw) {


        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // write bounds into pixels
        SetRgbPixels(result.bounds, config, pixels.ptr.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//rgb single region bounds
void
RgbRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

    // get reference to last Result in vector
    Result &result = results.back();

    // loop pixels while tracking percent and bounds
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
            result.bounds.expandTo(x, y);
            ++result.percent;
        }
    }

    // calculate percent size of changed pixels
    result.percent = 100 * result.percent / region.bitsetCount;

    // if percent threshold is not met, return
    if (region.percent > result.percent) return;

    //needed for callback
    result.flagged = true;

    // draw bounds into pixels
    if (config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // write bounds into pixels
        SetRgbPixels(result.bounds, config, pixels.ptr.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb multi regions bounds
void
RgbRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create Result in vector
        results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to last Result in vector
        Result &result = results.back();

        // loop pixels while tracking pixels and bounds
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) continue;
                result.bounds.expandTo(x, y);
                ++result.percent;
            }
        }

        // calculate percent size of changed pixels
        result.percent = 100 * result.percent / region.bitsetCount;

        // if percent threshold is not met, continue
        if (region.percent > result.percent) continue;

        // set flagged to true
        flagged = result.flagged = true;
    }

    //must be outside loop since all bounds will be draw to same pixels
    if (flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate blobsResults vector
        for (auto &result : results) {

            // skip if not flagged
            if (!result.flagged) continue;

            // write bounds into pixels
            SetRgbPixels(result.bounds, config, pixels.ptr.get());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb all blobs
void
RgbAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{all.name.data(), Bounds{config.width - 1, 0, config.height - 1, 0}});

    // get reference to last Result in vector
    Result &result = results.back();

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = 0, p = 0; y < config.height; ++y) {
        for (uint32_t x = 0; x < config.width; ++x, ++p) {
            if (all.difference > RgbDiff(buf0, buf1, p * config.depth)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                result.bounds.expandTo(x, y);
                ++result.percent;
            }
        }
    }

    // calculate percent size of blobbed pixels
    result.percent = 100 * result.percent / config.pixelCount;

    // if percent threshold is not met, skip blobbing the pixels
    if (all.percent > result.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, result.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = result.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{result.bounds.maxX, result.bounds.minX, result.bounds.maxY, result.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = result.bounds.minY; y <= result.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = result.bounds.minX, p = y * config.width + x; x <= result.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            blob.bounds.expandTo(x, y);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / config.pixelCount;
        if (all.percent > blob.percent) continue;
        blob.label = b;
        result.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (result.flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate blobs and draw bounds in pixels
        for (auto &blob : result.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetRgbPixels(blob.bounds, config, pixels.ptr.get());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb single region blobs
void
RgbRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(1);

    // create Result in vector
    results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

    // get reference to last Result in vector
    Result &result = results.back();

    // all pixels set to -1 will those set to -2 will be ignored
    for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
        for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
            if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) {
                labels[p] = -2;// set to -2 to mark as pixel to ignore
            } else {
                labels[p] = -1;// set to -1 to mark as pixel of interest
                result.bounds.expandTo(x, y);
                ++result.percent;
            }
        }
    }

    // calculate percent size of blobbed pixels
    result.percent = 100 * result.percent / region.bitsetCount;

    // if percent threshold is not met, skip blobbing the pixels
    if (region.percent > result.percent) return;

    // assign label to each pixel and return count of unique labels
    uint32_t blobCount = LabelImage(config, result.bounds, labels);

    // get reference to blobs vector
    std::vector<Blob> &blobs = result.blobs;

    // fill vector with blobs using the previous bounds results
    blobs.assign(blobCount, Blob{Bounds{result.bounds.maxX, result.bounds.minX, result.bounds.maxY, result.bounds.minY}});

    // loop rows within upper and lower bounds
    for (uint32_t y = result.bounds.minY; y <= result.bounds.maxY; ++y) {

        // loop columns within left and right bounds
        for (uint32_t x = result.bounds.minX, p = y * config.width + x; x <= result.bounds.maxX; ++x, ++p) {

            // get pixel label as blob index
            int32_t b = labels[p];

            // skip unlabeled pixel
            if (b < 0) continue;

            // get blob and update its data
            Blob &blob = blobs[b];
            blob.bounds.expandTo(x, y);
            ++blob.percent;
        }
    }

    // convert blob size to percent and check against threshold and flag
    for (uint32_t b = 0; b < blobCount; ++b) {
        Blob &blob = blobs[b];
        blob.percent = 100 * blob.percent / region.bitsetCount;
        if (region.percent > blob.percent) continue;
        blob.label = b;
        result.flagged = blob.flagged = true;
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (result.flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate blobs and draw bounds in pixels
        for (auto &blob : result.blobs) {

            // skip if not flagged
            if (!blob.flagged) continue;

            // write bounds into pixels
            SetRgbPixels(blob.bounds, config, pixels.ptr.get());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rgb multi regions blobs
void
RgbRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) {

    // will be used after main loop if config.draw == true
    bool flagged = false;

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<int32_t[]> up(new int32_t[config.pixelCount]);

    // get pointer
    int32_t *labels = up.get();

    // use size of regions to determine loop
    const size_t regionsLength = regions.size();

    // get reference to results
    std::vector<Result> &results = callbackData.results;

    // reserve memory
    results.reserve(regionsLength);

    // start looping regions and filling results
    for (uint32_t r = 0; r < regionsLength; ++r) {

        // get reference to region
        const Region &region = regions[r];

        // create Result in vector
        results.emplace_back(Result{region.name.data(), Bounds{region.bounds.maxX, region.bounds.minX, region.bounds.maxY, region.bounds.minY}});

        // get reference to last Result in vector
        Result &result = results.back();

        // all pixels set to -1 will those set to -2 will be ignored
        for (uint32_t y = region.bounds.minY; y <= region.bounds.maxY; ++y) {
            for (uint32_t x = region.bounds.minX, p = y * config.width + x; x <= region.bounds.maxX; ++x, ++p) {
                if (region.bitset[p] == 0 || region.difference > RgbDiff(buf0, buf1, p * config.depth)) {
                    labels[p] = -2;// set to -2 to mark as pixel to ignore
                } else {
                    labels[p] = -1;// set to -1 to mark as pixel of interest
                    result.bounds.expandTo(x, y);
                    ++result.percent;
                }
            }
        }

        // calculate percent size of blobbed pixels
        result.percent = 100 * result.percent / region.bitsetCount;

        // if percent threshold is not met, skip blobbing the pixels
        if (region.percent > result.percent) continue;

        // assign label to each pixel and return count of unique labels
        uint32_t blobCount = LabelImage(config, result.bounds, labels);

        // get reference to blobs vector
        std::vector<Blob> &blobs = result.blobs;

        // fill vector with blobs using the previous bounds results
        blobs.assign(blobCount, Blob{Bounds{result.bounds.maxX, result.bounds.minX, result.bounds.maxY, result.bounds.minY}});

        // loop rows within upper and lower bounds
        for (uint32_t y = result.bounds.minY; y <= result.bounds.maxY; ++y) {

            // loop columns within left and right bounds
            for (uint32_t x = result.bounds.minX, p = y * config.width + x; x <= result.bounds.maxX; ++x, ++p) {

                // get pixel label as blob index
                int32_t b = labels[p];

                // skip unlabeled pixel
                if (b < 0) continue;

                // get blob and update its data
                Blob &blob = blobs[b];
                blob.bounds.expandTo(x, y);
                ++blob.percent;
            }
        }

        // convert blob size to percent and check against threshold and flag
        for (uint32_t b = 0; b < blobCount; ++b) {
            Blob &blob = blobs[b];
            blob.percent = 100 * blob.percent / region.bitsetCount;
            if (region.percent > blob.percent) continue;
            blob.label = b;
            flagged = result.flagged = blob.flagged = true;
        }
    }

    //must be outside loop since all blobs will be draw to same pixels
    if (flagged && config.draw) {

        // get reference to Pixels
        Pixels &pixels = callbackData.pixels;

        // set pixels data in results
        pixels.ptr = std::unique_ptr<uint8_t[]>(new uint8_t[config.byteLength]);
        pixels.size = config.byteLength;

        // copy buf1 into pixels
        std::copy(buf1, buf1 + config.byteLength, pixels.ptr.get());

        // iterate Result vector
        for (auto &result : results) {

            // skip if not flagged
            if (!result.flagged) continue;

            // iterate blobs and draw bounds in pixels
            for (auto &blob : result.blobs) {

                // skip if not flagged
                if (!blob.flagged) continue;

                // write bounds into pixels
                SetRgbPixels(blob.bounds, config, pixels.ptr.get());
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
            executeFunc = [config, all](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayAllPercentExecute(config, {all}, buf0, buf1, callbackData); };
            callbackFunc = &PercentCallback;
            break;
        }
        case GRAY_ALL_BOUNDS: {//1
            const All all = All{"all", difference, percent};
            executeFunc = [config, all](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayAllBoundsExecute(config, {all}, buf0, buf1, callbackData); };
            callbackFunc = &BoundsCallback;
            break;
        }
        case GRAY_ALL_BLOBS: {//2
            const All all = All{"all", difference, percent};
            executeFunc = [config, all](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayAllBlobsExecute(config, {all}, buf0, buf1, callbackData); };
            callbackFunc = &BlobsCallback;
            break;
        }
        case GRAY_REGION_PERCENT: {//10
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [config, region](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayRegionPercentExecute(config, region, buf0, buf1, callbackData); };
            callbackFunc = &PercentCallback;
            break;
        }
        case GRAY_REGION_BOUNDS: {//11
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [config, region](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayRegionBoundsExecute(config, region, buf0, buf1, callbackData); };
            callbackFunc = &BoundsCallback;
            break;
        }
        case GRAY_REGION_BLOBS: {//12
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [config, region](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayRegionBlobsExecute(config, region, buf0, buf1, callbackData); };
            callbackFunc = &BlobsCallback;
            break;
        }
        case GRAY_REGIONS_PERCENT: {//20
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [config, regions](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayRegionsPercentExecute(config, regions, buf0, buf1, callbackData); };
            callbackFunc = &PercentCallback;
            break;
        }
        case GRAY_REGIONS_BOUNDS: {//21
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [config, regions](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayRegionsBoundsExecute(config, regions, buf0, buf1, callbackData); };
            callbackFunc = &BoundsCallback;
            break;
        }
        case GRAY_REGIONS_BLOBS: {//22
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [config, regions](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { GrayRegionsBlobsExecute(config, regions, buf0, buf1, callbackData); };
            callbackFunc = &BlobsCallback;
            break;
        }
        case RGB_ALL_PERCENT: {//100
            const All all = All{"all", difference, percent};
            executeFunc = [config, all](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbAllPercentExecute(config, {all}, buf0, buf1, callbackData); };
            callbackFunc = &PercentCallback;
            break;
        }
        case RGB_ALL_BOUNDS: {//101
            const All all = All{"all", difference, percent};
            executeFunc = [config, all](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbAllBoundsExecute(config, {all}, buf0, buf1, callbackData); };
            callbackFunc = &BoundsCallback;
            break;
        }
        case RGB_ALL_BLOBS: {//102
            const All all = All{"all", difference, percent};
            executeFunc = [config, all](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbAllBlobsExecute(config, {all}, buf0, buf1, callbackData); };
            callbackFunc = &BlobsCallback;
            break;
        }
        case RGB_REGION_PERCENT: {//110
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [config, region](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbRegionPercentExecute(config, region, buf0, buf1, callbackData); };
            callbackFunc = &PercentCallback;
            break;
        }
        case RGB_REGION_BOUNDS: {//111
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [config, region](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbRegionBoundsExecute(config, region, buf0, buf1, callbackData); };
            callbackFunc = &BoundsCallback;
            break;
        }
        case RGB_REGION_BLOBS: {//112
            const Region region = RegionJsToCpp(configObj.Get("regions").As<Napi::Array>().Get(0u).As<Napi::Object>());
            executeFunc = [config, region](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbRegionBlobsExecute(config, region, buf0, buf1, callbackData); };
            callbackFunc = &BlobsCallback;
            break;
        }
        case RGB_REGIONS_PERCENT: {//120
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [config, regions](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbRegionsPercentExecute(config, regions, buf0, buf1, callbackData); };
            callbackFunc = &PercentCallback;
            break;
        }
        case RGB_REGIONS_BOUNDS: {//121
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [config, regions](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbRegionsBoundsExecute(config, regions, buf0, buf1, callbackData); };
            callbackFunc = &BoundsCallback;
            break;
        }
        case RGB_REGIONS_BLOBS: {//122
            const std::vector<Region> regions = RegionsJsToCpp(configObj.Get("regions").As<Napi::Array>());
            executeFunc = [config, regions](const uint8_t *buf0, const uint8_t *buf1, CallbackData &callbackData) -> void { RgbRegionsBlobsExecute(config, regions, buf0, buf1, callbackData); };
            callbackFunc = &BlobsCallback;
            break;
        }
        default:
            throw Napi::Error::New(configObj.Env(), "Engine not found for type " + std::to_string(engineType));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////