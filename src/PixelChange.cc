#include <napi.h>
#include <algorithm>
//#include <iostream>
//extern "C" {
#include "ccl.c"
//}

//region = (name, diff, percent, count, bitset, diffs)
using Region = std::tuple<std::string, uint_fast8_t, uint_fast32_t, uint_fast32_t, Napi::Buffer < uint_fast8_t>, uint_fast32_t>;

//blob = (label number, count of pixels, min X, max X, min Y, max Y)
using Blob = std::tuple<uint_fast32_t, uint_fast32_t, uint_fast32_t, uint_fast32_t, uint_fast32_t, uint_fast32_t>;

//absolute value
inline uint_fast32_t absv(int_fast32_t x) {
    return (x > 0) ? x : -x;
}

//create vector of blobs
inline std::vector<Blob> blobsFromLabels(uint_fast32_t *labels, uint_fast32_t vectorSize, uint_fast32_t width, uint_fast32_t height) {
    //create vector
    std::vector<Blob> blobs(vectorSize);
    //track index of pixel
    uint_fast32_t index = 0;
    //iterate labeled pixels and group into blobs
    for (uint_fast32_t y = 0; y < height; y++) {
         for (uint_fast32_t x = 0; x < width; x++, index++) {
              //use label value as index
              uint_fast32_t label = labels[index];
              //skip label 0
              if (label == 0) continue;
              //get blob
              Blob &blob = blobs[label];
              //count will be 0 if this is first contact
              if (std::get<0>(blob) == 0) {
                   blob = std::make_tuple(label, 1, x, x, y, y);
              } else {
                   std::get<1>(blob)++;
                   std::get<2>(blob) = std::min(std::get<2>(blob), x);
                   std::get<3>(blob) = std::max(std::get<3>(blob), x);
                   std::get<4>(blob) = std::min(std::get<4>(blob), y);
                   std::get<5>(blob) = std::max(std::get<5>(blob), y);
              }
         }
    }
    return blobs;
}

Napi::Array CompareGrayPixels(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i++) {
        if (absv(buf0[i] - buf1[i]) >= diff) diffs++;
    }
    Napi::Array results = Napi::Array::New(env);
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
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += 3) {
        if (absv(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3 >= diff) diffs++;
    }
    Napi::Array results = Napi::Array::New(env);
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
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i += 4) {
        if (absv(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3 >= diff) diffs++;
    }
    Napi::Array results = Napi::Array::New(env);
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

Napi::Array CompareGrayRegions(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const uint_fast8_t minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    std::vector<Region> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }
    for (uint_fast32_t p = 0; p < bufLen; p++) {
        const auto diff = absv(buf0[p] - buf1[p]);
        if (minDiff > diff) continue;
        for (uint_fast32_t i = 0; i < regLen; i++) {
            if (!std::get<4>(regionsVec[i])[p] || diff < std::get<1>(regionsVec[i])) continue;
            std::get<5>(regionsVec[i])++;
        }
    }
    Napi::Array results = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0; i < regLen; i++) {
        uint_fast32_t percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }
    return results;
}

Napi::Array CompareRgbRegions(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const uint_fast8_t minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    std::vector<Region> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += 3, p++) {
        const auto diff = absv(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
        if (minDiff > diff) continue;
        for (uint_fast32_t i = 0; i < regLen; i++) {
            if (!std::get<4>(regionsVec[i])[p] || diff < std::get<1>(regionsVec[i])) continue;
            std::get<5>(regionsVec[i])++;
        }
    }
    Napi::Array results = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0; i < regLen; i++) {
        uint_fast32_t percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }
    return results;
}

Napi::Array CompareRgbaRegions(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const uint_fast8_t minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    std::vector<Region> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += 4, p++) {
        const auto diff = absv(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3;
        if (minDiff > diff) continue;
        for (uint_fast32_t i = 0; i < regLen; i++) {
            if (!std::get<4>(regionsVec[i])[p] || diff < std::get<1>(regionsVec[i])) continue;
            std::get<5>(regionsVec[i])++;
        }
    }
    Napi::Array results = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0; i < regLen; i++) {
        uint_fast32_t percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }
    return results;
}

////////////////////////////////////////////////////////////////////////

Napi::Array CompareGrayMask(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i++) {
        if (bitset[i] && absv(buf0[i] - buf1[i]) >= diff) diffs++;
    }
    Napi::Array results = Napi::Array::New(env);
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
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += 3, p++) {
        if (bitset[p] && absv(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3 >= diff) diffs++;
    }
    Napi::Array results = Napi::Array::New(env);
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
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i += 4, p++) {
        if (bitset[p] && absv(buf0[i] + buf0[i + 1] + buf0[i + 2] - buf1[i] - buf1[i + 1] - buf1[i + 2]) / 3 >= diff) diffs++;
    }
    Napi::Array results = Napi::Array::New(env);
    const uint_fast32_t perc = 100 * diffs / count;
    if (perc >= percent) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "mask");
        obj.Set("percent", perc);
        results["0"] = obj;
    }
    return results;
}

Napi::Array CompareGrayPixelsBlob(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    //get all params
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast32_t width = info[4].As<Napi::Number>().Uint32Value();
    const uint_fast32_t height = info[5].As<Napi::Number>().Uint32Value();
    const uint_fast32_t blobSize = info[6].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[7].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[8].As<Napi::Buffer<uint_fast8_t>>();
    //set diffs counter to 0
    uint_fast32_t diffs = 0;
    //set empty bool array, remember to delete
    bool *pixelBitset = new bool[wxh]();
    //iterate pixels, count diffs, and set true and false of bool array
    for (uint_fast32_t i = 0; i < bufLen; i++) {
        if (diff > absv(buf0[i] - buf1[i])) continue;
        diffs++;
        pixelBitset[i] = true;
    }
    //create new array that can be returned to JS
    Napi::Array results = Napi::Array::New(env);
    //calculate percent of difference
    const uint_fast32_t perc = 100 * diffs / wxh;
    //if percent meets/exceeds setting, check for blobs
    if (perc >= percent) {
        //create unsigned int array to hold labels
        uint_fast32_t *pixelLabels = new uint_fast32_t[wxh]();
        //label pixels and set maxLabel (from C lib ccl.c)
        const uint_fast32_t blobsLength = LabelImage(width, height, pixelBitset, pixelLabels);
        //extract blobs from labels
        const std::vector<Blob> blobs = blobsFromLabels(pixelLabels, blobsLength, width, height);
        //delete output array
        delete[] pixelLabels;
        //create JS array to return blob objects
        Napi::Array blobsArray = Napi::Array::New(env);
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
        //create JS object to hold values and put into JS array
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", perc);
        obj.Set("blobs", blobsArray);
        results["0"] = obj;
    }
    //delete input array
    delete[] pixelBitset;
    //return results array to JS
    return results;
}

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
