#include <napi.h>
#include <algorithm>
#include <iostream>
extern "C" {
#include "ccl.c"
}

inline uint_fast32_t absv(int_fast32_t x) {
    return (x > 0) ? x : -x;
}

Napi::Array CompareGrayPixels(const Napi::CallbackInfo& info) {
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

Napi::Array CompareRgbPixels(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i+=3) {
        if (absv(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) diffs++;
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

Napi::Array CompareRgbaPixels(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0; i < bufLen; i+=4) {
        if (absv(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) diffs++;
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

Napi::Array CompareGrayRegions(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const uint_fast8_t minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    std::vector<std::tuple<std::string, uint_fast8_t, uint_fast32_t, uint_fast32_t, Napi::Buffer<uint_fast8_t>, uint_fast32_t>> regionsVec(regLen);
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
    for(uint_fast32_t i = 0, j = 0; i < regLen; i++) {
        uint_fast32_t percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }
    return results;
}

Napi::Array CompareRgbRegions(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const uint_fast8_t minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    std::vector<std::tuple<std::string, uint_fast8_t, uint_fast32_t, uint_fast32_t, Napi::Buffer<uint_fast8_t>, uint_fast32_t>> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i+=3, p++) {
        const auto diff = absv(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3;
        if (minDiff > diff) continue;
        for (uint_fast32_t i = 0; i < regLen; i++) {
            if (!std::get<4>(regionsVec[i])[p] || diff < std::get<1>(regionsVec[i])) continue;
            std::get<5>(regionsVec[i])++;
        }
    }
    Napi::Array results = Napi::Array::New(env);
    for(uint_fast32_t i = 0, j = 0; i < regLen; i++) {
        uint_fast32_t percent = 100 * std::get<5>(regionsVec[i]) / std::get<3>(regionsVec[i]);
        if (std::get<2>(regionsVec[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regionsVec[i]));
        obj.Set("percent", percent);
        results[j++] = obj;
    }
    return results;
}

Napi::Array CompareRgbaRegions(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const uint_fast8_t minDiff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t regLen = info[1].As<Napi::Number>().Uint32Value();
    const Napi::Array regionsArr = info[2].As<Napi::Array>();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[4].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    std::vector<std::tuple<std::string, uint_fast8_t, uint_fast32_t, uint_fast32_t, Napi::Buffer<uint_fast8_t>, uint_fast32_t>> regionsVec(regLen);
    for (uint_fast32_t i = 0; i < regLen; i++) {
        const std::string name = regionsArr.Get(i).As<Napi::Object>().Get("name").As<Napi::String>();
        const uint_fast8_t diff = regionsArr.Get(i).As<Napi::Object>().Get("diff").As<Napi::Number>().Uint32Value();
        const uint_fast32_t percent = regionsArr.Get(i).As<Napi::Object>().Get("percent").As<Napi::Number>().Uint32Value();
        const uint_fast32_t count = regionsArr.Get(i).As<Napi::Object>().Get("count").As<Napi::Number>().Uint32Value();
        const Napi::Buffer<uint_fast8_t> bitset = regionsArr.Get(i).As<Napi::Object>().Get("bitset").As<Napi::Buffer<uint_fast8_t>>();
        regionsVec[i] = std::make_tuple(name, diff, percent, count, bitset, 0);
    }
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i+=4, p++) {
        const auto diff = absv(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3;
        if (minDiff > diff) continue;
        for (uint_fast32_t i = 0; i < regLen; i++) {
            if (!std::get<4>(regionsVec[i])[p] || diff < std::get<1>(regionsVec[i])) continue;
            std::get<5>(regionsVec[i])++;
        }
    }
    Napi::Array results = Napi::Array::New(env);
    for(uint_fast32_t i = 0, j = 0; i < regLen; i++) {
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

Napi::Array CompareGrayMask(const Napi::CallbackInfo& info) {
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

Napi::Array CompareRgbMask(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i+=3, p++) {
        if (bitset[p] && absv(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) diffs++;
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

Napi::Array CompareRgbaMask(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast32_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t count = info[2].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> bitset = info[3].As<Napi::Buffer<uint_fast8_t>>();
    const uint_fast32_t bufLen = info[4].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[5].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[6].As<Napi::Buffer<uint_fast8_t>>();
    uint_fast32_t diffs = 0;
    for (uint_fast32_t i = 0, p = 0; i < bufLen; i+=4, p++) {
        if (bitset[p] && absv(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) diffs++;
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

//using Blob = std::tuple<uint_fast32_t, uint_fast32_t, uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t>;

struct Blob {
    uint_fast32_t label;
    uint_fast32_t size;
    uint_fast16_t minX;
    uint_fast16_t maxX;
    uint_fast16_t minY;
    uint_fast16_t maxY;
};

Napi::Array CompareGrayPixelsBlob(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    //get all params
    const uint_fast8_t diff = info[0].As<Napi::Number>().Uint32Value();
    const uint_fast16_t percent = info[1].As<Napi::Number>().Uint32Value();
    const uint_fast32_t wxh = info[2].As<Napi::Number>().Uint32Value();
    const uint_fast32_t bufLen = info[3].As<Napi::Number>().Uint32Value();
    const uint_fast16_t width = info[4].As<Napi::Number>().Uint32Value();
    const uint_fast16_t height = info[5].As<Napi::Number>().Uint32Value();
    const uint_fast32_t blobSize = info[6].As<Napi::Number>().Uint32Value();
    const Napi::Buffer<uint_fast8_t> buf0 = info[7].As<Napi::Buffer<uint_fast8_t>>();
    const Napi::Buffer<uint_fast8_t> buf1 = info[8].As<Napi::Buffer<uint_fast8_t>>();

    //set diffs counter to 0
    uint_fast32_t diffs = 0;

    //std::cout<<"received blob "<<blob<<std::endl;

    //todo will switch to bitset if possible, much smaller in size
    //set empty unsigned char array, remember to delete
    uint_fast8_t* input = new uint_fast8_t[wxh]();

    //iterate pixels, count diffs, and set 1's and 0's of bitset (binary) array
    for (uint_fast32_t i = 0; i < bufLen; i++) {
        if (diff > absv(buf0[i] - buf1[i])) continue;
        diffs++;
        input[i] = 1;
    }

    //create new array that can be returned to JS
    Napi::Array results = Napi::Array::New(env);

    //calculate percent of difference
    const uint_fast16_t perc = 100 * diffs / wxh;

    //if percent meets/exceeds setting, check for blobs
    if (perc >= percent) {

        std::cout<<"percent "<<perc<<std::endl;

        //create unsigned int array to hold labels
        uint_fast32_t* output = new uint_fast32_t[wxh]();

        //variable to hold maxLabel, will be used to determine vector length
        uint_fast32_t maxLabel;

        //label pixels and set maxLabel
        LabelImage(width, height, input, output, maxLabel);

        std::cout<<"max label "<<maxLabel<<std::endl;

        //length of Blob vector
        const uint_fast32_t blobsLength = maxLabel + 1;

        //create vector of blobs
        std::vector<Blob> blobs(blobsLength);

        //track index of output labels
        uint_fast32_t index = 0;

        //iterate labels and group into blobs
        for (uint_fast16_t y = 0; y < height; y++) {

            for (uint_fast16_t x = 0; x < width; x++, index++) {

                uint_fast32_t label = output[index];

                if (label == 0) continue;

                Blob &blob = blobs[label];

                //count will be 0 if this is first contact
                if (blob.label == 0) {
                    blob.label = label;
                    blob.size = 1;
                    blob.minX = x;
                    blob.maxX = x;
                    blob.minY = y;
                    blob.maxY = y;
                } else {
                    blob.size++;
                    blob.minX = std::min(blob.minX, x);
                    blob.maxX = std::max(blob.maxX, x);
                    blob.minY = std::min(blob.minY, y);
                    blob.maxY = std::max(blob.maxY, y);
                }
            }
        }

        //delete output array
        delete [] output;

        //filter blobs by size
        for (uint_fast32_t i = 1; i < blobsLength; i++) {
            Blob& tmpBlob = blobs[i];
            if (tmpBlob.size < blobSize) continue;

            //todo create return data here
            std::cout<<"label: "<<tmpBlob.label<<" size: "<<tmpBlob.size<<" minX: "<<tmpBlob.minX<<" maxX: "<<tmpBlob.maxX<<" minY: "<<tmpBlob.minY<<" maxY: "<<tmpBlob.maxY<<std::endl;

        }

        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", "all");
        obj.Set("percent", perc);
        results["0"] = obj;
    }

    //delete input array
    delete [] input;

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
