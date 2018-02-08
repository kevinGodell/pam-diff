#include <napi.h>
#include <algorithm>

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
        obj.Set("name", "percent");
        obj.Set("percent", perc);
        results[(uint_fast32_t)0] = obj;
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
        obj.Set("name", "percent");
        obj.Set("percent", perc);
        results[(uint_fast32_t)0] = obj;
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
        obj.Set("name", "percent");
        obj.Set("percent", perc);
        results[(uint_fast32_t)0] = obj;
    }
    return results;
}

////////////////////////////////////////////////////////////////////////

//minDiff, regLen, regions, bufLen, buf0, buf1
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

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "compareGrayPixels"), Napi::Function::New(env, CompareGrayPixels));
    exports.Set(Napi::String::New(env, "compareRgbPixels"), Napi::Function::New(env, CompareRgbPixels));
    exports.Set(Napi::String::New(env, "compareRgbaPixels"), Napi::Function::New(env, CompareRgbaPixels));
    exports.Set(Napi::String::New(env, "compareGrayRegions"), Napi::Function::New(env, CompareGrayRegions));
    exports.Set(Napi::String::New(env, "compareRgbRegions"), Napi::Function::New(env, CompareRgbRegions));
    exports.Set(Napi::String::New(env, "compareRgbaRegions"), Napi::Function::New(env, CompareRgbaRegions));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)