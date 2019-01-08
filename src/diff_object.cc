#include "diff_object.h"
#include "diff.h"
#include "helper.h"

#include <napi.h>

//#include <string>
//#include <iostream>

void Example::Init(const Napi::Env &env, Napi::Object &exports) {
    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "Example", {
        InstanceMethod("compare", &Example::Compare),
        InstanceAccessor("myValue", &Example::GetMyValue, &Example::SetMyValue)
    });

    // Create a peristent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    constructor = Napi::Persistent(func);
    // Call the SuppressDestruct() method on the static data prevent the calling
    // to this destructor to reset the reference when the environment is no longer
    // available.
    constructor.SuppressDestruct();
    exports.Set("Example", func);
}

// constructor implementation
Example::Example(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Example>(info) {
    Napi::Env env = info.Env();//todo not sure if line needed
    Napi::HandleScope scope(env);//todo not sure if line needed

    const Napi::Object obj = info[0].As<Napi::Object>();

    //all config objects must have these values
    this->width_ = obj.Get("width").As<Napi::Number>().Uint32Value();
    this->height_ = obj.Get("height").As<Napi::Number>().Uint32Value();
    this->depth_ = obj.Get("depth").As<Napi::Number>().Uint32Value();
    this->target_ = obj.Get("target").As<Napi::String>().Utf8Value();
    this->filter_ = obj.Get("filter").As<Napi::String>().Utf8Value();
    this->async_ =  obj.Get("async").As<Napi::Boolean>().Value();

    this->pixCount_ = this->width_ * this->height_ * this->depth_;

    /*std::cout << "width: " << this->width_ << std::endl;
    std::cout << "height: " << this->height_ << std::endl;
    std::cout << "depth: " << this->depth_ << std::endl;
    std::cout << "target: " << this->target_ << std::endl;
    std::cout << "filter: " << this->filter_ << std::endl;
    std::cout << "async: " << this->async_ << std::endl;*/

    //std::cout << "engine number: " << engineType(this->depth_, this->target_, this->filter_, this->async_) << std::endl;

    switch (engineType(this->depth_, this->target_, this->filter_, this->async_)) {
        case GRAY_ALL_PERCENT_SYNC :
            this->pixDiff_ = obj.Get("difference").As<Napi::Number>().Uint32Value();
            this->diffsPerc_ = obj.Get("percent").As<Napi::Number>().Uint32Value();
            this->comparePtr_ = &Example::GrayAllPercentSync;
        break;
        case GRAY_MASK_PERCENT_SYNC :
            //std::cout << "using gray mask percent sync" << std::endl;
            this->pixDiff_ = obj.Get("difference").As<Napi::Number>().Uint32Value();
            this->diffsPerc_ = obj.Get("percent").As<Napi::Number>().Uint32Value();
            this->bitsetCount_ = obj.Get("bitsetCount").As<Napi::Number>().Uint32Value();
            this->bitset_ = obj.Get("bitset").As<Napi::Buffer<uint_fast8_t>>().Data();
            this->comparePtr_ = &Example::GrayMaskPercentSync;
        break;
        case GRAY_REGIONS_PERCENT_SYNC :
            //std::cout << "using gray regions percent sync" << std::endl;
            this->minDiff_ = obj.Get("minDiff").As<Napi::Number>().Uint32Value();
            this->regionsLen_ = obj.Get("regionsLen").As<Napi::Number>().Uint32Value();
            const Napi::Array regionsJs = obj.Get("regions").As<Napi::Array>();
            this->regionsCpp_ = new Region[this->regionsLen_]();// create array of type Region on heap
            regionsJsToCpp(this->regionsLen_, regionsJs, this->regionsCpp_);// convert js array to cpp array
            this->comparePtr_ = &Example::GrayRegionsPercentSync;
        break;
    }

}

Example::~Example() {
    if (this->regionsCpp_) {
        delete[] this->regionsCpp_;
    }
};

Napi::FunctionReference Example::constructor;

void Example::Compare(const Napi::CallbackInfo &info) {
    const uint_fast8_t *buf0 = info[0].As<Napi::Buffer<uint_fast8_t>>().Data();
    const uint_fast8_t *buf1 = info[1].As<Napi::Buffer<uint_fast8_t>>().Data();
    const Napi::Function cb = info[2].As<Napi::Function>();

    (this->*(this->comparePtr_))(buf0, buf1, cb);
}

void Example::SetMyValue(const Napi::CallbackInfo &info, const Napi::Value& value) {
    //std::cout << "set myValue" << std::endl;
    this->myValue_ = value.As<Napi::String>();
}

Napi::Value Example::GetMyValue(const Napi::CallbackInfo &info) {
    //std::cout << "get myValue" << std::endl;
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->myValue_);
}

Napi::Value Example::GrayAllPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    uint_fast8_t percentResult = MeasureDiffs(this->pixCount_, this->pixDiff_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback
    allResultsToJs(env, this->diffsPerc_, percentResult, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::GrayMaskPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();
    uint_fast8_t percentResult = MeasureDiffs(this->pixCount_, this->pixDiff_, this->bitsetCount_, this->bitset_, buf0, buf1);
    Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback
    maskResultsToJs(env, this->diffsPerc_, percentResult, resultsJs);
    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

Napi::Value Example::GrayRegionsPercentSync(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const Napi::Function &cb) {
    const Napi::Env env = cb.Env();

    //Region *regionsCpp = new Region[this->regionsLen_]();// create array of type Region on heap

    //regionsJsToCpp(this->regionsLen_, this->regionsJs_, regionsCpp);// convert js array to cpp array

    MeasureDiffs(this->pixCount_, this->minDiff_, this->regionsLen_, this->regionsCpp_, buf0, buf1);

    Napi::Array resultsJs = Napi::Array::New(env);// results placeholder, will be passed to callback

    regionsResultsToJs(env, this->regionsLen_, this->regionsCpp_, resultsJs);//  convert cpp array to js results array

    //delete[] regionsCpp;//should reset results value to 0 for next usage

    cb.Call({env.Null(), resultsJs});
    return env.Undefined();
}

// Napi::Object obj = constructor.New({ Napi::Number::New(info.Env(), this->value_ * multiple.DoubleValue()) });
// https://nodejs.github.io/node-addon-api/class_napi_1_1_object_wrap.html