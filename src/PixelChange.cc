#include <napi.h>
//#include <algorithm>
//#include <iostream> /* needed for std::cout */
#include "ccl.c"

//region     =            name,        diff,          percent,       count,         bitset,                       diffs
using Region = std::tuple<std::string, uint_fast32_t, uint_fast32_t, uint_fast32_t, Napi::Buffer < uint_fast8_t>, uint_fast32_t>;

//blob     =            label,         count,         minX,          maxX,          minY,          maxY
using Blob = std::tuple<uint_fast32_t, uint_fast32_t, uint_fast32_t, uint_fast32_t, uint_fast32_t, uint_fast32_t>;

//absolute value
inline uint_fast32_t AbsUint(int_fast32_t n) {
    return (n > 0) ? n : -n;
}

//math min
inline uint_fast32_t MinUint(uint_fast32_t a, uint_fast32_t b) {
    return (a < b) ? a : b;
}

//math max
inline uint_fast32_t MaxUint(uint_fast32_t a, uint_fast32_t b) {
    return (a > b) ? a : b;
}

//gray pixel diff
inline uint_fast32_t GrayDiff(Napi::Buffer<uint_fast8_t> buf0, Napi::Buffer<uint_fast8_t> buf1, uint_fast32_t i) {
    return AbsUint(buf0[i] - buf1[i]);
}

//rgb pixel diff
inline uint_fast32_t RgbDiff(Napi::Buffer<uint_fast8_t> buf0, Napi::Buffer<uint_fast8_t> buf1, uint_fast32_t i) {
    return AbsUint(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
}

//create vector of blobs
inline std::vector<Blob> BlobsFromLabels(const uint_fast32_t *labels, uint_fast32_t vectorSize, uint_fast32_t width, uint_fast32_t height) {
    //create vector
    std::vector<Blob> blobs(vectorSize);
    //iterate labeled pixels and group into blobs
    for (uint_fast32_t y = 0, x = 0, i = 0, label = 0; y < height; y++) {
         for (x = 0; x < width; x++, i++) {
              //use label value as index
              label = labels[i];
              //skip label 0
              if (label == 0) continue;
              //get blob
              Blob &blob = blobs[label];
              //count will be 0 if this is first contact
              if (std::get<0>(blob) == 0) {
                   blob = std::make_tuple(label, 1, x, x, y, y);
              } else {
                   std::get<1>(blob)++;
                   std::get<2>(blob) = MinUint(std::get<2>(blob), x);
                   std::get<3>(blob) = MaxUint(std::get<3>(blob), x);
                   std::get<4>(blob) = MinUint(std::get<4>(blob), y);
                   std::get<5>(blob) = MaxUint(std::get<5>(blob), y);
              }
         }
    }
    return blobs;
}

////////////////////////////////////////////////////////////////////////

Napi::Array CompareGrayPixels(const Napi::CallbackInfo &info) {
    const Napi::Env                   env = info.Env();
    const uint_fast32_t              diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t           percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t               wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                   results = Napi::Array::New(env);

    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i++) {
        if (GrayDiff(buf0, buf1, i) >= diff) diffs++;
    }

    const uint_fast32_t perc = 100 * diffs / wxh;
    if (perc >= percent) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", perc);
        results["0"] = obj;
    }

    return results;
}

Napi::Array CompareRgbPixels(const Napi::CallbackInfo &info) {
    const Napi::Env                   env = info.Env();
    const uint_fast32_t              diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t           percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t               wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                   results = Napi::Array::New(env);

    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += 3) {
        if (RgbDiff(buf0, buf1, i) >= diff) diffs++;
    }

    const uint_fast32_t perc = 100 * diffs / wxh;
    if (perc >= percent) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", perc);
        results["0"] = obj;
    }

    return results;
}

Napi::Array CompareRgbaPixels(const Napi::CallbackInfo &info) {
    const Napi::Env                   env = info.Env();
    const uint_fast32_t              diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t           percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t               wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                   results = Napi::Array::New(env);

    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += 4) {
        if (RgbDiff(buf0, buf1, i) >= diff) diffs++;
    }

    const uint_fast32_t perc = 100 * diffs / wxh;
    if (perc >= percent) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", perc);
        results["0"] = obj;
    }

    return results;
}

////////////////////////////////////////////////////////////////////////

Napi::Array CompareGrayMask(const Napi::CallbackInfo &info) {
    const Napi::Env                     env = info.Env();
    const uint_fast32_t                diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t             percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t               count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t              bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t>   buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t>   buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                     results = Napi::Array::New(env);

    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i++) {
        if (bitset[i] && GrayDiff(buf0, buf1, i) >= diff) diffs++;
    }

    const uint_fast32_t perc = 100 * diffs / count;
    if (perc >= percent) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", perc);
        results["0"] = obj;
    }

    return results;
}

Napi::Array CompareRgbMask(const Napi::CallbackInfo &info) {
    const Napi::Env                     env = info.Env();
    const uint_fast32_t                diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t             percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t               count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t              bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t>   buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t>   buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                     results = Napi::Array::New(env);

    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += 3, p++) {
        if (bitset[p] && RgbDiff(buf0, buf1, i) >= diff) diffs++;
    }

    const uint_fast32_t perc = 100 * diffs / count;
    if (perc >= percent) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", perc);
        results["0"] = obj;
    }

    return results;
}

Napi::Array CompareRgbaMask(const Napi::CallbackInfo &info) {
    const Napi::Env                     env = info.Env();
    const uint_fast32_t                diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t             percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t               count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t              bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t>   buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t>   buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                     results = Napi::Array::New(env);

    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += 4, p++) {
        if (bitset[p] && RgbDiff(buf0, buf1, i) >= diff) diffs++;
    }

    const uint_fast32_t perc = 100 * diffs / count;
    if (perc >= percent) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", perc);
        results["0"] = obj;
    }

    return results;
}

////////////////////////////////////////////////////////////////////////

Napi::Array CompareGrayRegions(const Napi::CallbackInfo &info) {
    const Napi::Env                   env = info.Env();
    const uint_fast32_t           minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array          regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t            bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                   results = Napi::Array::New(env);

    std::vector<Region> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string                  name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast32_t                diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t             percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t               count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }

    for (uint_fast32_t i = 0, j = 0, diff = 0; i < bufLen; i++) {
        diff = GrayDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (j = 0; j < regLen; j++) {
            if (std::get<4>(regionsVec[j])[i] && diff >= std::get<1>(regionsVec[j])) std::get<5>(regionsVec[j])++;
        }
    }

    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }

    return results;
}

Napi::Array CompareRgbRegions(const Napi::CallbackInfo &info) {
    const Napi::Env                   env = info.Env();
    const uint_fast8_t            minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array          regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t            bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                   results = Napi::Array::New(env);

    std::vector<Region> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string                  name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t                 diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t             percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t               count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }

    for (uint_fast32_t i = 0, j = 0, diff = 0; i < bufLen; i += 3) {
        diff = RgbDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (j = 0; j < regLen; j++) {
            if (std::get<4>(regionsVec[j])[i / 3] && diff >= std::get<1>(regionsVec[j])) std::get<5>(regionsVec[j])++;
        }
    }

    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }

    return results;
}

Napi::Array CompareRgbaRegions(const Napi::CallbackInfo &info) {
    const Napi::Env                   env = info.Env();
    const uint_fast8_t            minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array          regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t            bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                   results = Napi::Array::New(env);

    std::vector<Region> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string                  name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t                 diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t             percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t               count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }

    for (uint_fast32_t i = 0, j = 0, diff = 0; i < bufLen; i += 4) {
        diff = RgbDiff(buf0, buf1, i);
        if (minDiff > diff) continue;
        for (j = 0; j < regLen; j++) {
            if (std::get<4>(regionsVec[j])[i / 4] && diff >= std::get<1>(regionsVec[j])) std::get<5>(regionsVec[j])++;
        }
    }

    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }

    return results;
}

////////////////////////////////////////////////////////////////////////

Napi::Array CompareGrayPixelsBlob(const Napi::CallbackInfo &info) {
    const Napi::Env                   env = info.Env();
    const uint_fast32_t              diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t           percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t               wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            bufLen = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast32_t             width = info[4].As<Napi::Number>().Uint32Value();
    const uint_fast32_t            height = info[5].As<Napi::Number>().Uint32Value();
    const uint_fast32_t          blobSize = info[6].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[7].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[8].As<Napi::Buffer<uint_fast8_t>>();

    Napi::Array                  results = Napi::Array::New(env);// create new array that can be returned to JS

    uint_fast32_t diffs = 0;// set diffs counter to 0
    bool *pixelBitset = new bool[wxh]();// set empty bool array, remember to delete
    //iterate pixels, count diffs, and set true and false of bool array
    for (uint_fast32_t i = 0; i < bufLen; i++) {
        if (diff > GrayDiff(buf0, buf1, i)) continue;
        diffs++;
        pixelBitset[i] = true;
    }

    const uint_fast32_t perc = 100 * diffs / wxh;// calculate percent of difference
    //if percent meets/exceeds setting, check for blobs
    if (perc >= percent) {
        uint_fast32_t *pixelLabels = new uint_fast32_t[wxh]();// create unsigned int array to hold labels

        const uint_fast32_t blobsLength = LabelImage(width, height, pixelBitset, pixelLabels);// label pixels and set maxLabel (from C lib ccl.c)

        const std::vector<Blob> blobs = BlobsFromLabels(pixelLabels, blobsLength, width, height);// extract blobs from labels

        delete[] pixelLabels;// delete output array

        Napi::Array blobsArray = Napi::Array::New(env);// create JS array to return blob objects

        //filter blobs by size
        for (uint_fast32_t i = 1, j = 0; i < blobsLength; i++) {
            const Blob blob = blobs[i];
            if (std::get<1>(blob) < blobSize) continue;
            Napi::Object obj = Napi::Object::New(env);
            obj.Set("label", std::get<0>(blob));
            obj.Set("size", std::get<1>(blob));
            obj.Set("minX", std::get<2>(blob));
            obj.Set("maxX", std::get<3>(blob));
            obj.Set("minY", std::get<4>(blob));
            obj.Set("maxY", std::get<5>(blob));
            blobsArray[j++] = obj;
        }

        Napi::Object obj = Napi::Object::New(env);// create JS object to hold values and put into JS array
        obj.Set("name", "all");
        obj.Set("percent", perc);
        obj.Set("blobs", blobsArray);
        results["0"] = obj;
    }

    delete[] pixelBitset;// delete input array

    return results;// return JS array results
}

////////////////////////////////////////////////////////////////////////

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "compareGrayPixels"), Napi::Function::New(env, CompareGrayPixels));
    exports.Set(Napi::String::New(env, "compareRgbPixels"), Napi::Function::New(env, CompareRgbPixels));
    exports.Set(Napi::String::New(env, "compareRgbaPixels"), Napi::Function::New(env, CompareRgbaPixels));
    exports.Set(Napi::String::New(env, "compareGrayRegions"), Napi::Function::New(env, CompareGrayRegions));
    exports.Set(Napi::String::New(env, "compareRgbRegions"), Napi::Function::New(env, CompareRgbRegions));
    exports.Set(Napi::String::New(env, "compareRgbaRegions"), Napi::Function::New(env, CompareRgbaRegions));
    exports.Set(Napi::String::New(env, "compareGrayMask"), Napi::Function::New(env, CompareGrayMask));
    exports.Set(Napi::String::New(env, "compareRgbMask"), Napi::Function::New(env, CompareRgbMask));
    exports.Set(Napi::String::New(env, "compareRgbaMask"), Napi::Function::New(env, CompareRgbaMask));
    exports.Set(Napi::String::New(env, "compareGrayPixelsBlob"), Napi::Function::New(env, CompareGrayPixelsBlob));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)

//todo should mark low index and high index of pixel when regions/masks used so that we can iterate less unnecessary pixels
//todo maybe use min/max X and min/max Y for low point and high points of pixel iteration, but then will have to calculate index manually
