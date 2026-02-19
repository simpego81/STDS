#include <napi.h>
#include "STDSEngine.hpp"
#include <memory>
#include <iostream>

class STDSEngineWrapper : public Napi::ObjectWrap<STDSEngineWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    STDSEngineWrapper(const Napi::CallbackInfo& info);

private:
    static Napi::FunctionReference constructor;
    std::unique_ptr<stds::STDSEngine> engine_;
    Napi::ThreadSafeFunction tsfn_;

    Napi::Value LoadData(const Napi::CallbackInfo& info);
    Napi::Value Train(const Napi::CallbackInfo& info);
    Napi::Value ProcessNewData(const Napi::CallbackInfo& info);
    Napi::Value GetTreeJSON(const Napi::CallbackInfo& info);
    Napi::Value SetNodeCallback(const Napi::CallbackInfo& info);
};

Napi::FunctionReference STDSEngineWrapper::constructor;

Napi::Object STDSEngineWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "STDSEngine", {
        InstanceMethod("loadData", &STDSEngineWrapper::LoadData),
        InstanceMethod("train", &STDSEngineWrapper::Train),
        InstanceMethod("processNewData", &STDSEngineWrapper::ProcessNewData),
        InstanceMethod("getTreeJSON", &STDSEngineWrapper::GetTreeJSON),
        InstanceMethod("setNodeCallback", &STDSEngineWrapper::SetNodeCallback)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("STDSEngine", func);
    return exports;
}

STDSEngineWrapper::STDSEngineWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<STDSEngineWrapper>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Parse configuration from JavaScript object
    stds::STDSConfig config;
    
    if (info.Length() > 0 && info[0].IsObject()) {
        Napi::Object configObj = info[0].As<Napi::Object>();
        
        if (configObj.Has("numBins")) {
            config.num_bins = configObj.Get("numBins").As<Napi::Number>().Int32Value();
        }
        if (configObj.Has("sequenceLength")) {
            config.sequence_length = configObj.Get("sequenceLength").As<Napi::Number>().Int32Value();
        }
        if (configObj.Has("confidenceThreshold")) {
            config.confidence_threshold = configObj.Get("confidenceThreshold").As<Napi::Number>().DoubleValue();
        }
        if (configObj.Has("lookaheadDays")) {
            config.lookahead_days = configObj.Get("lookaheadDays").As<Napi::Number>().Int32Value();
        }
        if (configObj.Has("takeProfitThreshold")) {
            config.take_profit_threshold = configObj.Get("takeProfitThreshold").As<Napi::Number>().DoubleValue();
        }
    }

    engine_.reset(new stds::STDSEngine(config));
}

Napi::Value STDSEngineWrapper::LoadData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string filename = info[0].As<Napi::String>().Utf8Value();
    bool success = engine_->loadData(filename);

    return Napi::Boolean::New(env, success);
}

Napi::Value STDSEngineWrapper::Train(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    engine_->train();
    
    return env.Undefined();
}

Napi::Value STDSEngineWrapper::ProcessNewData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Object expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object dataObj = info[0].As<Napi::Object>();
    stds::OHLCV data;

    data.open = dataObj.Get("open").As<Napi::Number>().DoubleValue();
    data.high = dataObj.Get("high").As<Napi::Number>().DoubleValue();
    data.low = dataObj.Get("low").As<Napi::Number>().DoubleValue();
    data.close = dataObj.Get("close").As<Napi::Number>().DoubleValue();
    data.volume = dataObj.Get("volume").As<Napi::Number>().DoubleValue();

    std::string decision = engine_->processNewData(data);

    return Napi::String::New(env, decision);
}

Napi::Value STDSEngineWrapper::GetTreeJSON(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    std::string json = engine_->getTreeJSON();
    
    return Napi::String::New(env, json);
}

Napi::Value STDSEngineWrapper::SetNodeCallback(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction()) {
        Napi::TypeError::New(env, "Function expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Napi::Function callback = info[0].As<Napi::Function>();

    // Create thread-safe function
    tsfn_ = Napi::ThreadSafeFunction::New(
        env,
        callback,
        "NodeCallback",
        0,
        1
    );

    // Set callback in C++ engine
    engine_->setNodeCallback([this](const stds::SequenceNode* node) {
        auto callback = [node](Napi::Env env, Napi::Function jsCallback) {
            Napi::Object nodeObj = Napi::Object::New(env);
            nodeObj.Set("id", Napi::Number::New(env, node->id));
            nodeObj.Set("symbol", Napi::Number::New(env, node->symbol));
            nodeObj.Set("weight", Napi::Number::New(env, node->weight));
            nodeObj.Set("synthesis", Napi::String::New(env, node->synthesis));
            
            Napi::Object statsObj = Napi::Object::New(env);
            statsObj.Set("buyWins", Napi::Number::New(env, node->stats.buy_wins));
            statsObj.Set("sellWins", Napi::Number::New(env, node->stats.sell_wins));
            statsObj.Set("holdCount", Napi::Number::New(env, node->stats.hold_count));
            nodeObj.Set("stats", statsObj);
            
            jsCallback.Call({nodeObj});
        };
        
        tsfn_.BlockingCall(callback);
    });

    return env.Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return STDSEngineWrapper::Init(env, exports);
}

NODE_API_MODULE(stds_bindings, Init)
