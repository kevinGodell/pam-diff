#include "object.h"
#include "engine.h"
#include "napi.h"

#ifdef NAPI_DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

Napi::Object CreateObject(const Napi::CallbackInfo &info) {
    const Napi::Env env = info.Env();
    const Napi::Object configObj = info[0].As<Napi::Object>();
#ifdef NAPI_DEBUG
    // show system size values for types being used
    cout << "size of bool : " << sizeof(bool) << endl;
    cout << "size of std::string : " << sizeof(std::string) << endl;
    cout << "size of int32_t : " << sizeof(int32_t) << endl;
    cout << "size of uint32_t : " << sizeof(uint32_t) << endl;
    cout << "size of std::vector<bool> : " << sizeof(std::vector<bool>) << endl;
    cout << "size of std::vector<uint8_t> : " << sizeof(std::vector<uint8_t>) << endl;
    cout << "size of std::vector<Result> : " << sizeof(std::vector<Result>) << endl;
    cout << "size of Config struct : " << sizeof(Config) << endl;
    cout << "size of All struct : " << sizeof(All) << endl;
    cout << "size of Bounds struct : " << sizeof(Bounds) << endl;
    cout << "size of Region struct : " << sizeof(Region) << endl;
    cout << "size of Blob struct : " << sizeof(Blob) << endl;
    cout << "size of Result struct : " << sizeof(Result) << endl;
    cout << "size of Pixels struct : " << sizeof(Pixels) << endl;
    cout << "size of CallbackData struct : " << sizeof(CallbackData) << endl;

    if (configObj.HasOwnProperty("depth")) cout << "depth : " << configObj.Get("depth").As<Napi::Number>().Uint32Value() << endl;
    if (configObj.HasOwnProperty("width")) cout << "width : " << configObj.Get("width").As<Napi::Number>().Uint32Value() << std::endl;
    if (configObj.HasOwnProperty("height")) std::cout << "height : " << configObj.Get("height").As<Napi::Number>().Uint32Value() << std::endl;
    if (configObj.HasOwnProperty("response")) std::cout << "response : " << configObj.Get("response").As<Napi::String>().Utf8Value() << std::endl;
    if (configObj.HasOwnProperty("draw")) std::cout << "draw : " << configObj.Get("draw").As<Napi::Boolean>().Value() << std::endl;
    if (configObj.HasOwnProperty("difference")) std::cout << "difference : " << configObj.Get("difference").As<Napi::Number>().Uint32Value() << std::endl;
    if (configObj.HasOwnProperty("percent")) std::cout << "percent : " << configObj.Get("percent").As<Napi::Number>().Uint32Value() << std::endl;
    if (configObj.HasOwnProperty("regions")) {
        const Napi::Array regionsJs = configObj.Get("regions").As<Napi::Array>();
        //uint32_t regionsLength = regionsJs.Length();
        cout << "regions length : " << regionsJs.Length() << endl;
        for (uint32_t r = 0; r < regionsJs.Length(); ++r) {
            Napi::Object obj = regionsJs.Get(r).As<Napi::Object>();
            const std::string name = obj.HasOwnProperty("name") ? obj.Get("name").As<Napi::String>() : std::string();
            const uint32_t difference = obj.HasOwnProperty("difference") ? obj.Get("difference").As<Napi::Number>().Uint32Value() : 0;
            const uint32_t percent = obj.HasOwnProperty("percent") ? obj.Get("percent").As<Napi::Number>().Uint32Value() : 0;
            const uint32_t bitsetCount = obj.HasOwnProperty("bitsetCount") ? obj.Get("bitsetCount").As<Napi::Number>().Uint32Value() : 0;
            const uint32_t bitsetLength = obj.HasOwnProperty("bitset") ? static_cast<uint32_t>(obj.Get("bitset").As<Napi::Buffer<bool>>().Length()) : 0;
            const uint32_t minX = obj.HasOwnProperty("minX") ? obj.Get("minX").As<Napi::Number>().Uint32Value() : 0;
            const uint32_t maxX = obj.HasOwnProperty("maxX") ? obj.Get("maxX").As<Napi::Number>().Uint32Value() : 0;
            const uint32_t minY = obj.HasOwnProperty("minY") ? obj.Get("minY").As<Napi::Number>().Uint32Value() : 0;
            const uint32_t maxY = obj.HasOwnProperty("maxY") ? obj.Get("maxY").As<Napi::Number>().Uint32Value() : 0;
            cout << name << " - " << difference << " - " << minX << " - " << maxX << " - " << minY << " - " << maxY << " - " << percent << " - " << bitsetCount << " - " << bitsetLength << endl;
        }
    }
#endif
    return PixelChange::NewInstance(env, configObj);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports = Napi::Function::New(env, CreateObject, "CreateObject");
    PixelChange::Init(env);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)