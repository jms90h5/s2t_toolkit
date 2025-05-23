#include <benchmark/benchmark.h>
#include "../impl/include/WenetSTTImplImproved.hpp"
#include <random>
#include <vector>

using namespace wenet_streams;

// Mock callback for benchmarking
class BenchmarkCallback : public TranscriptionCallback {
public:
    void onTranscriptionResult(const TranscriptionResult&) override {}
    void onError(const std::string&) override {}
};

// Generate random audio data
std::vector<int16_t> generateRandomAudio(size_t samples) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int16_t> dist(-32768, 32767);
    
    std::vector<int16_t> audio(samples);
    for (auto& sample : audio) {
        sample = dist(gen);
    }
    return audio;
}

// Benchmark circular buffer operations
static void BM_CircularBuffer_PushPop(benchmark::State& state) {
    CircularBuffer<int> buffer(state.range(0));
    
    for (auto _ : state) {
        // Push until full
        for (int i = 0; i < state.range(0); ++i) {
            buffer.push(i);
        }
        
        // Pop all
        while (buffer.pop().has_value()) {
            benchmark::DoNotOptimize(buffer.size());
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
}
BENCHMARK(BM_CircularBuffer_PushPop)->Range(10, 1000);

// Benchmark audio chunk processing
static void BM_ProcessAudioChunk(benchmark::State& state) {
    WeNetConfig config;
    config.modelPath = "/tmp/mock_model";
    config.bufferCapacity = 100;
    config.vadEnabled = false;  // Disable VAD for pure throughput test
    
    BenchmarkCallback callback;
    WenetSTTImplImproved engine(config, &callback);
    engine.initialize();
    
    const size_t chunkSize = state.range(0);
    auto audioData = generateRandomAudio(chunkSize);
    
    for (auto _ : state) {
        AudioChunk chunk(audioData, 0);
        engine.processAudioChunk(chunk);
    }
    
    state.SetItemsProcessed(state.iterations() * chunkSize);
    state.SetBytesProcessed(state.iterations() * chunkSize * sizeof(int16_t));
}
BENCHMARK(BM_ProcessAudioChunk)->Range(160, 16000);

// Benchmark VAD performance
static void BM_VoiceActivityDetection(benchmark::State& state) {
    const size_t samples = state.range(0);
    auto audioData = generateRandomAudio(samples);
    
    // Simple energy-based VAD
    auto detectSpeech = [](const std::vector<int16_t>& samples) -> bool {
        if (samples.empty()) return false;
        
        double sumSquares = 0.0;
        for (const auto& sample : samples) {
            double normalized = sample / 32768.0;
            sumSquares += normalized * normalized;
        }
        double rms = std::sqrt(sumSquares / samples.size());
        double energyDb = 20.0 * std::log10(rms + 1e-10);
        
        return energyDb > -30.0;  // Threshold
    };
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(detectSpeech(audioData));
    }
    
    state.SetItemsProcessed(state.iterations() * samples);
}
BENCHMARK(BM_VoiceActivityDetection)->Range(160, 16000);

// Benchmark JSON parsing
static void BM_JsonParsing(benchmark::State& state) {
    std::string jsonResult = R"({
        "text": "this is a test transcription result with multiple words",
        "confidence": 0.95,
        "timestamp": 1234567890,
        "nbest": [
            {"sentence": "this is a test transcription result with multiple words", "confidence": 0.95},
            {"sentence": "this is a test transcription results with multiple words", "confidence": 0.85},
            {"sentence": "this is a text transcription result with multiple words", "confidence": 0.75}
        ]
    })";
    
    for (auto _ : state) {
        rapidjson::Document doc;
        doc.Parse(jsonResult.c_str());
        
        if (!doc.HasParseError()) {
            benchmark::DoNotOptimize(doc["text"].GetString());
            benchmark::DoNotOptimize(doc["confidence"].GetDouble());
            
            if (doc.HasMember("nbest") && doc["nbest"].IsArray()) {
                for (const auto& alt : doc["nbest"].GetArray()) {
                    benchmark::DoNotOptimize(alt["sentence"].GetString());
                    benchmark::DoNotOptimize(alt["confidence"].GetDouble());
                }
            }
        }
    }
    
    state.SetBytesProcessed(state.iterations() * jsonResult.size());
}
BENCHMARK(BM_JsonParsing);

// Benchmark batch processing
static void BM_BatchProcessing(benchmark::State& state) {
    WeNetConfig config;
    config.modelPath = "/tmp/mock_model";
    config.bufferCapacity = 1000;
    config.chunkSize = state.range(0);  // Batch size
    
    BenchmarkCallback callback;
    WenetSTTImplImproved engine(config, &callback);
    engine.initialize();
    
    const size_t audioChunkSize = 1600;  // 0.1 seconds at 16kHz
    std::vector<AudioChunk> chunks;
    for (int i = 0; i < state.range(0); ++i) {
        chunks.emplace_back(generateRandomAudio(audioChunkSize), i * 100);
    }
    
    for (auto _ : state) {
        for (const auto& chunk : chunks) {
            engine.processAudioChunk(chunk);
        }
        
        // Wait a bit for processing
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0) * audioChunkSize);
}
BENCHMARK(BM_BatchProcessing)->Range(1, 32);

// Benchmark end-to-end latency
static void BM_EndToEndLatency(benchmark::State& state) {
    WeNetConfig config;
    config.modelPath = "/tmp/mock_model";
    config.bufferCapacity = 100;
    config.maxLatency = 0.1;  // Low latency mode
    
    class LatencyCallback : public TranscriptionCallback {
    public:
        std::chrono::steady_clock::time_point lastResultTime;
        std::atomic<bool> resultReceived{false};
        
        void onTranscriptionResult(const TranscriptionResult&) override {
            lastResultTime = std::chrono::steady_clock::now();
            resultReceived = true;
        }
        void onError(const std::string&) override {}
    };
    
    LatencyCallback callback;
    WenetSTTImplImproved engine(config, &callback);
    engine.initialize();
    
    const size_t audioChunkSize = 16000;  // 1 second
    auto audioData = generateRandomAudio(audioChunkSize);
    
    std::vector<double> latencies;
    
    for (auto _ : state) {
        callback.resultReceived = false;
        auto startTime = std::chrono::steady_clock::now();
        
        AudioChunk chunk(audioData, 0);
        engine.processAudioChunk(chunk);
        engine.flush();
        
        // Wait for result
        while (!callback.resultReceived && 
               std::chrono::steady_clock::now() - startTime < std::chrono::seconds(1)) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        
        if (callback.resultReceived) {
            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
                callback.lastResultTime - startTime).count();
            latencies.push_back(latency);
        }
    }
    
    // Calculate statistics
    if (!latencies.empty()) {
        std::sort(latencies.begin(), latencies.end());
        double median = latencies[latencies.size() / 2];
        double p95 = latencies[latencies.size() * 95 / 100];
        double p99 = latencies[latencies.size() * 99 / 100];
        
        state.SetLabel("median=" + std::to_string(median) + "us, " +
                      "p95=" + std::to_string(p95) + "us, " +
                      "p99=" + std::to_string(p99) + "us");
    }
}
BENCHMARK(BM_EndToEndLatency)->Iterations(100);

// Memory allocation benchmark
static void BM_MemoryAllocation(benchmark::State& state) {
    const size_t chunkSize = state.range(0);
    
    for (auto _ : state) {
        std::vector<int16_t> samples(chunkSize);
        AudioChunk chunk(std::move(samples), 0);
        benchmark::DoNotOptimize(chunk);
    }
    
    state.SetBytesProcessed(state.iterations() * chunkSize * sizeof(int16_t));
}
BENCHMARK(BM_MemoryAllocation)->Range(160, 48000);

BENCHMARK_MAIN();