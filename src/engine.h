#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include "napi.h"
#include <cstdint>
#include <string>
#include <vector>

enum Engines {// ordered by depth -> target -> response -> synchronicity
    GRAY_ALL_PERCENT_SYNC = 0,
    GRAY_ALL_PERCENT_ASYNC = 1,
    GRAY_ALL_BOUNDS_SYNC = 10,
    GRAY_ALL_BOUNDS_ASYNC = 11,
    GRAY_ALL_BLOBS_SYNC = 20,
    GRAY_ALL_BLOBS_ASYNC = 21,

    GRAY_REGION_PERCENT_SYNC = 100,
    GRAY_REGION_PERCENT_ASYNC = 101,
    GRAY_REGION_BOUNDS_SYNC = 110,
    GRAY_REGION_BOUNDS_ASYNC = 111,
    GRAY_REGION_BLOBS_SYNC = 120,
    GRAY_REGION_BLOBS_ASYNC = 121,

    GRAY_REGIONS_PERCENT_SYNC = 200,
    GRAY_REGIONS_PERCENT_ASYNC = 201,
    GRAY_REGIONS_BOUNDS_SYNC = 210,
    GRAY_REGIONS_BOUNDS_ASYNC = 211,
    GRAY_REGIONS_BLOBS_SYNC = 220,
    GRAY_REGIONS_BLOBS_ASYNC = 221,

    RGB_ALL_PERCENT_SYNC = 1000,
    RGB_ALL_PERCENT_ASYNC = 1001,
    RGB_ALL_BOUNDS_SYNC = 1010,
    RGB_ALL_BOUNDS_ASYNC = 1011,
    RGB_ALL_BLOBS_SYNC = 1020,
    RGB_ALL_BLOBS_ASYNC = 1021,

    RGB_REGION_PERCENT_SYNC = 1100,
    RGB_REGION_PERCENT_ASYNC = 1101,
    RGB_REGION_BOUNDS_SYNC = 1110,
    RGB_REGION_BOUNDS_ASYNC = 1111,
    RGB_REGION_BLOBS_SYNC = 1120,
    RGB_REGION_BLOBS_ASYNC = 1121,

    RGB_REGIONS_PERCENT_SYNC = 1200,
    RGB_REGIONS_PERCENT_ASYNC = 1201,
    RGB_REGIONS_BOUNDS_SYNC = 1210,
    RGB_REGIONS_BOUNDS_ASYNC = 1211,
    RGB_REGIONS_BLOBS_SYNC = 1220,
    RGB_REGIONS_BLOBS_ASYNC = 1221,
};

/*enum Engines2 {// ordered by depth -> target -> response -> synchronicity
    GRAY_ALL_PERCENT = 0,
    GRAY_ALL_BOUNDS = 1,
    GRAY_ALL_BLOBS = 2,

    GRAY_REGION_PERCENT = 10,
    GRAY_REGION_BOUNDS = 11,
    GRAY_REGION_BLOBS = 12,

    GRAY_REGIONS_PERCENT = 020,
    GRAY_REGIONS_BOUNDS = 021,
    GRAY_REGIONS_BLOBS = 022,

    RGB_ALL_PERCENT = 100,
    RGB_ALL_BOUNDS = 101,
    RGB_ALL_BLOBS = 102,

    RGB_REGION_PERCENT = 110,
    RGB_REGION_BOUNDS = 111,
    RGB_REGION_BLOBS = 112,

    RGB_REGIONS_PERCENT = 120,
    RGB_REGIONS_BOUNDS = 121,
    RGB_REGIONS_BLOBS = 122,
};*/

struct Config {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t pixelCount;
    uint32_t byteLength;
    bool draw;

    Config() = default;

    Config(uint32_t _width, uint32_t _height, uint32_t _depth, bool _draw) : width(_width), height(_height), depth(_depth), pixelCount(_width * _height), byteLength(_width * _height * _depth), draw(_draw) {}
};

struct All {
    std::string name;
    //const char *name;
    uint32_t difference;
    uint32_t percent;
};

struct Bounds {
    uint32_t minX;
    uint32_t maxX;
    uint32_t minY;
    uint32_t maxY;
};

struct Region {
    std::string name;// 24
    std::vector<bool> bitset;// 24
    uint32_t bitsetCount;// 4
    uint32_t difference;// 4
    uint32_t percent;// 4
    Bounds bounds;
};

//struct Regions {
//std::vector<Region> regions;
//};

struct PercentResult {
    const char *name;
    uint32_t percent;// 4
    bool flagged;// 1
    PercentResult() : name(nullptr), percent(0), flagged(false) {}

    explicit PercentResult(const char *_name) : name(_name), percent(0), flagged(false) {}
};

struct BoundsResult {
    const char *name;
    Bounds bounds;
    uint32_t percent;
    bool flagged;

    BoundsResult() : name(nullptr), bounds(Bounds{0, 0, 0, 0}), percent(0), flagged(false) {}

    BoundsResult(const char *_name, Bounds _bounds) : name(_name), bounds(_bounds), percent(0), flagged(false) {}
};

struct Blob {
    uint32_t label;
    Bounds bounds;
    uint32_t percent;
    bool flagged;

    Blob() = default;

    explicit Blob(Bounds _bounds) : label(0), bounds(_bounds), percent(0), flagged(false) {}
};

struct BlobsResult {
    const char *name;
    Bounds bounds;
    uint32_t percent;// 4
    bool flagged;// 1
    std::vector<Blob> blobs;

    BlobsResult() : name(nullptr), bounds(Bounds{0, 0, 0, 0}), percent(0), flagged(false), blobs(std::vector<Blob>()) {}

    BlobsResult(const char *_name, Bounds _bounds) : name(_name), bounds(_bounds), percent(0), flagged(false), blobs(std::vector<Blob>()) {}
};

struct Pixels {
    uint8_t *ptr;
    uint32_t size;

    Pixels() : ptr(nullptr), size(0) {}

    Pixels(uint8_t *_ptr, uint32_t _size) : ptr(_ptr), size(_size) {}
};

struct Results {//192
    PercentResult percentResult;
    BoundsResult boundsResult;
    BlobsResult blobsResult;
    std::vector<PercentResult> percentResults;
    std::vector<BoundsResult> boundsResults;
    std::vector<BlobsResult> blobsResults;
    Pixels pixels;
};

typedef std::function<void(const uint8_t *buf0, const uint8_t *buf1, Results &results)> ExecuteFunc;

//typedef std::function<void(const uint8_t *buf0, const uint8_t *buf1, const Napi::Env &env, const Napi::Function &cb)> ExecuteSyncFunc;

typedef std::function<void(const Napi::Env &env, const Napi::Function &cb, const Results &results)> CallbackFunc;

//void
//ConfigureFunctions(const Napi::Object &configObj, ExecuteFunc &executeFunc, CallbackFunc &callbackFunc);

inline uint32_t
AbsInt(int32_t n) {
    return static_cast<uint32_t>(n < 0 ? -n : n);
}

// measure difference of gray bytes
inline uint32_t
GrayDiff(const uint8_t *buf0, const uint8_t *buf1, const uint32_t i) {
    //return static_cast<uint32_t>(std::abs(buf0[i] - buf1[i]));
    return AbsInt(buf0[i] - buf1[i]);
}

// measure difference of rgb(a) bytes using average
inline uint32_t
RgbDiff(const uint8_t *buf0, const uint8_t *buf1, uint32_t i) {
    //return std::abs(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3u;
    return AbsInt(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3u;
}

// set minimum x or y coord
inline void
SetMin(const uint32_t &val, uint32_t &min) {
    if (val < min) min = val;
}

// set maximum x or y coord
inline void
SetMax(const uint32_t &val, uint32_t &max) {
    if (val > max) max = val;
}

// determine engine type
uint32_t
EngineType(uint32_t depth, uint32_t regionsLength, const std::string &response, bool async);

//uint32_t
//EngineType2(uint32_t depth, uint32_t regionsLength, const std::string &response);

// convert js bitset to cpp
std::vector<bool>
BitsetJsToCpp(const Napi::Buffer<bool> &bitsetJs);

// convert js region to cpp
Region
RegionJsToCpp(const Napi::Object &regionJs);

// convert js regions to cpp
std::vector<Region>
RegionsJsToCpp(const Napi::Array &regionsJs);

// gray all percent
void
GrayAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray single region percent
void
GrayRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray multi regions bounds
void
GrayRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray all bounds
void
GrayAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray single region bounds
void
GrayRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray multi regions bounds
void
GrayRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray all blobs
void
GrayAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray single region blobs
void
GrayRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// gray multi regions blobs
void
GrayRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb all percent
void
RgbAllPercentExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb single region percent
void
RgbRegionPercentExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb multi regions bounds
void
RgbRegionsPercentExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb all bounds
void
RgbAllBoundsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb single region bounds
void
RgbRegionBoundsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb multi regions bounds
void
RgbRegionsBoundsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb all blobs
void
RgbAllBlobsExecute(const Config &config, const All &all, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb single region blobs
void
RgbRegionBlobsExecute(const Config &config, const Region &region, const uint8_t *buf0, const uint8_t *buf1, Results &results);

// rgb multi regions blobs
void
RgbRegionsBlobsExecute(const Config &config, const std::vector<Region> &regions, const uint8_t *buf0, const uint8_t *buf1, Results &results);

#endif