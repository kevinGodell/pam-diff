#include "helper.h"
#include "diff.h"
#include <napi.h>

/*inline Napi::Array convertResultsToJS(const Napi::Env env, Region *regions, uint_fast8_t regLen) {
    Napi::Array jsArray = Napi::Array::New(env);
    for (uint_fast32_t i = 0, j = 0, percent = 0; i < regLen; i++) {
        percent = 100 * std::get<5>(regions[i]) / std::get<3>(regions[i]);
        if (std::get<2>(regions[i]) > percent) continue;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", std::get<0>(regions[i]));
        obj.Set("percent", percent);
        jsArray[j++] = obj;
    }
    return jsArray;
}*/