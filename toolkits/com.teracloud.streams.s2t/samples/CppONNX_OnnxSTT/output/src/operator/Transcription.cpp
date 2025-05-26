// _13zCRC8DHINSLfEFqDZfxXUijQ8eSn68XMgxjhVIqvViQahDnFcHKHAKWDapMakhnhnJfufYiMK7qTFRoEFYBw
/*
 * OnnxSTT operator implementation
 */


#include "./Transcription.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <string>

#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR Transcription_Base
#define MY_OPERATOR Transcription$OP





// Implementation code starts here
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <iostream>
#include <vector>

using namespace SPL;
using namespace std;

// Using the SPL-generated namespace and class names

MY_OPERATOR_SCOPE::MY_OPERATOR::MY_OPERATOR() 
    : initialized_(false)
    , audio_timestamp_ms_(0)
    , total_samples_processed_(0) {
    
    SPLAPPTRC(L_DEBUG, "OnnxSTT operator constructor", "OnnxSTT");
}

MY_OPERATOR_SCOPE::MY_OPERATOR::~MY_OPERATOR() {
    SPLAPPTRC(L_DEBUG, "OnnxSTT operator destructor", "OnnxSTT");
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::initialize() {
    if (initialized_) return;
    
    try {
        // Configure ONNX implementation
        config_.encoder_onnx_path = SPL::rstring("../../models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/decoder-epoch-99-avg-1.onnx");
        config_.vocab_path = SPL::rstring("../../models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/tokens.txt");
        config_.cmvn_stats_path = SPL::rstring("models/global_cmvn.stats");
        config_.sample_rate = SPL::int32(16000);
        config_.chunk_size_ms = SPL::int32(100);
        config_.num_threads = SPL::int32(4);
        config_.use_gpu = true;
        
        SPLAPPTRC(L_INFO, "Initializing OnnxSTT with model: " + config_.encoder_onnx_path, "OnnxSTT");
        
        // Create implementation
        onnx_impl_ = std::make_unique<onnx_stt::OnnxSTTWrapper>(config_);
        
        // Initialize
        if (!onnx_impl_->initialize()) {
            SPLAPPTRC(L_ERROR, "Failed to initialize OnnxSTT", "OnnxSTT");
            throw std::runtime_error("OnnxSTT initialization failed");
        }
        
        initialized_ = true;
        SPLAPPTRC(L_INFO, "OnnxSTT initialized successfully", "OnnxSTT");
        
    } catch (const std::exception& e) {
        SPLAPPTRC(L_ERROR, "Exception during initialization: " + string(e.what()), "OnnxSTT");
        throw;
    }
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Tuple const & tuple, uint32_t port) {
    // Initialize on first tuple
    if (!initialized_) {
        initialize();
    }
    
    AutoPortMutex apm(_mutex, *this);
    
    const IPort0Type& iport = static_cast<const IPort0Type&>(tuple);
    
    // Get audio data
    processAudioData(iport.get_audioChunk());
    
    // Get timestamp
    audio_timestamp_ms_ = iport.get_audioTimestamp();
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::processAudioData(const SPL::blob& audio_blob) {
    // Get audio samples
    const int16_t* samples = reinterpret_cast<const int16_t*>(audio_blob.getData());
    size_t num_samples = audio_blob.getSize() / sizeof(int16_t);
    
    if (num_samples == 0) return;
    
    // Process with ONNX implementation
    auto result = onnx_impl_->processAudioChunk(samples, num_samples, audio_timestamp_ms_);
    
    // Update stats
    total_samples_processed_ += num_samples;
    audio_timestamp_ms_ += (num_samples * 1000) / config_.sample_rate;
    
    // Submit result if we have text
    if (!result.text.empty()) {
        submitResult(result);
    }
    
    // Log performance periodically
    if (total_samples_processed_ % (config_.sample_rate * 10) == 0) {
        auto stats = onnx_impl_->getStats();
        SPLAPPTRC(L_INFO, 
            "Processed " + to_string(total_samples_processed_ / config_.sample_rate) + 
            " seconds, RTF: " + to_string(stats.real_time_factor), 
            "OnnxSTT");
    }
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::submitResult(const onnx_stt::OnnxSTTWrapper::TranscriptionResult& result) {
    // Create output tuple
    OPort0Type otuple;
    
    // Set attributes based on output schema: text, isFinal, confidence
    otuple.set_text(result.text);
    otuple.set_isFinal(result.is_final);
    otuple.set_confidence(result.confidence);
    
    // Submit the tuple
    submit(otuple, 0);
    
    SPLAPPTRC(L_DEBUG, 
        "Submitted: " + result.text + 
        " (final=" + to_string(result.is_final) + 
        ", latency=" + to_string(result.latency_ms) + "ms)", 
        "OnnxSTT");
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Punctuation const & punct, uint32_t port) {
    if (punct == Punctuation::FinalMarker) {
        // Reset the decoder on final punctuation
        if (onnx_impl_) {
            onnx_impl_->reset();
            SPLAPPTRC(L_DEBUG, "Reset decoder on final punctuation", "OnnxSTT");
        }
    }
    
    // Forward punctuation
    submit(punct, 0);
}

// End of implementation

static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("Transcription",&initer));
    return true;
}

template<class T> static void initRTC (SPL::Operator& o, T& v, const char * n) {
    SPL::ValueHandle vh = v;
    o.getContext().getRuntimeConstantValue(vh, n);
}

MY_BASE_OPERATOR::MY_BASE_OPERATOR()
 : Operator()  {
    uint32_t index = getIndex();
    param$encoderModel$0 = SPL::rstring("../../models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/decoder-epoch-99-avg-1.onnx");
    addParameterValue ("encoderModel", SPL::ConstValueHandle(param$encoderModel$0));
    param$vocabFile$0 = SPL::rstring("../../models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/tokens.txt");
    addParameterValue ("vocabFile", SPL::ConstValueHandle(param$vocabFile$0));
    param$cmvnFile$0 = SPL::rstring("models/global_cmvn.stats");
    addParameterValue ("cmvnFile", SPL::ConstValueHandle(param$cmvnFile$0));
    param$sampleRate$0 = SPL::int32(16000);
    addParameterValue ("sampleRate", SPL::ConstValueHandle(param$sampleRate$0));
    param$chunkSizeMs$0 = SPL::int32(100);
    addParameterValue ("chunkSizeMs", SPL::ConstValueHandle(param$chunkSizeMs$0));
    param$provider$0 = SPL::rstring("CPU");
    addParameterValue ("provider", SPL::ConstValueHandle(param$provider$0));
    param$numThreads$0 = SPL::int32(4);
    addParameterValue ("numThreads", SPL::ConstValueHandle(param$numThreads$0));
    (void) getParameters(); // ensure thread safety by initializing here
    $oportBitset = OPortBitsetType(std::string("01"));
    OperatorMetrics& om = getContext().getMetrics();
    metrics_[0] = &(om.createCustomMetric("nExceptionsCaughtPort0", "Number of exceptions caught on port 0", Metric::Counter));
}
MY_BASE_OPERATOR::~MY_BASE_OPERATOR()
{
    for (ParameterMapType::const_iterator it = paramValues_.begin(); it != paramValues_.end(); it++) {
        const ParameterValueListType& pvl = it->second;
        for (ParameterValueListType::const_iterator it2 = pvl.begin(); it2 != pvl.end(); it2++) {
            delete *it2;
        }
    }
}

void MY_BASE_OPERATOR::tupleLogic(Tuple const & tuple, uint32_t port) {
}


void MY_BASE_OPERATOR::processRaw(Tuple const & tuple, uint32_t port) {
    try {
            tupleLogic (tuple, port);
            static_cast<MY_OPERATOR_SCOPE::MY_OPERATOR*>(this)->MY_OPERATOR::process(tuple, port);
    } catch (const SPL::SPLRuntimeException& e) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown(e.getExplanation());
    } catch (const std::exception& e) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown(e.what());
    } catch (...) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown("Unknown exception");
    }
}


void MY_BASE_OPERATOR::punctLogic(Punctuation const & punct, uint32_t port) {
}

void MY_BASE_OPERATOR::punctPermitProcessRaw(Punctuation const & punct, uint32_t port) {
    {
        punctNoPermitProcessRaw(punct, port);
    }
}

void MY_BASE_OPERATOR::punctNoPermitProcessRaw(Punctuation const & punct, uint32_t port) {
    switch(punct) {
    case Punctuation::WindowMarker:
        try {
            punctLogic(punct, port);
            process(punct, port);
        } catch (const SPL::SPLRuntimeException& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.getExplanation());
        } catch (const std::exception& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.what());
        } catch (...) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown("Unknown exception");
        }
        break;
    case Punctuation::FinalMarker:
        {
            try {
                punctLogic(punct, port);
                process(punct, port);
            } catch (const SPL::SPLRuntimeException& e) {
                metrics_[port]->incrementValue();
                unrecoverableExceptionShutdown(e.getExplanation());
            } catch (const std::exception& e) {
                metrics_[port]->incrementValue();
                unrecoverableExceptionShutdown(e.what());
            } catch (...) {
                metrics_[port]->incrementValue();
                unrecoverableExceptionShutdown("Unknown exception");
            }
            bool forward = false;
            {
                AutoPortMutex $apm($fpMutex, *this);
                $oportBitset.reset(port);
                if ($oportBitset.none()) {
                    $oportBitset.set(1);
                    forward=true;
                }
            }
            if(forward) {
                if (0 < 1) submit(punct, 0);
                else submitException(punct, 0 - 1);
            }
            return;
        }
    case Punctuation::DrainMarker:
    case Punctuation::ResetMarker:
    case Punctuation::ResumeMarker:
        break;
    case Punctuation::SwitchMarker:
        break;
    case Punctuation::WatermarkMarker:
        break;
    default:
        break;
    }
}

void MY_BASE_OPERATOR::processRaw(Punctuation const & punct, uint32_t port) {
    switch(port) {
    case 0:
        punctNoPermitProcessRaw(punct, port);
        break;
    }
}



void MY_BASE_OPERATOR::checkpointStateVariables(NetworkByteBuffer & opstate) const {
}

void MY_BASE_OPERATOR::restoreStateVariables(NetworkByteBuffer & opstate) {
}

void MY_BASE_OPERATOR::checkpointStateVariables(Checkpoint & ckpt) {
}

void MY_BASE_OPERATOR::resetStateVariables(Checkpoint & ckpt) {
}

void MY_BASE_OPERATOR::resetStateVariablesToInitialState() {
}

bool MY_BASE_OPERATOR::hasStateVariables() const {
    return false;
}

void MY_BASE_OPERATOR::resetToInitialStateRaw() {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->resetToInitialState();
    }
    resetStateVariablesToInitialState();
}

void MY_BASE_OPERATOR::checkpointRaw(Checkpoint & ckpt) {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->checkpoint(ckpt);
    }
    checkpointStateVariables(ckpt);
}

void MY_BASE_OPERATOR::resetRaw(Checkpoint & ckpt) {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->reset(ckpt);
    }
    resetStateVariables(ckpt);
}


