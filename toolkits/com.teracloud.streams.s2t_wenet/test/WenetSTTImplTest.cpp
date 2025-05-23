#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../impl/include/WenetSTTImplImproved.hpp"
#include <thread>
#include <chrono>
#include <random>

using namespace wenet_streams;
using namespace testing;

// Mock implementation of TranscriptionCallback
class MockTranscriptionCallback : public TranscriptionCallback {
public:
    MOCK_METHOD(void, onTranscriptionResult, (const TranscriptionResult&), (override));
    MOCK_METHOD(void, onError, (const std::string&), (override));
};

// Test fixture for WenetSTTImpl tests
class WenetSTTImplTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create mock callback
        mockCallback = std::make_unique<MockTranscriptionCallback>();
        
        // Set up default config
        config.modelPath = "/tmp/test_model";
        config.sampleRate = 16000;
        config.partialResultsEnabled = true;
        config.maxLatency = 0.3;
        config.vadEnabled = true;
        config.vadSilenceThreshold = -40.0;
        config.vadSpeechThreshold = -10.0;
        config.maxChunkDuration = 3.0;
        config.bufferCapacity = 100;
    }
    
    void TearDown() override {
        // Clean up
    }
    
    // Helper function to generate test audio
    std::vector<int16_t> generateTestAudio(size_t numSamples, double frequency = 440.0, double amplitude = 0.5) {
        std::vector<int16_t> samples(numSamples);
        const double sampleRate = 16000.0;
        
        for (size_t i = 0; i < numSamples; ++i) {
            double t = i / sampleRate;
            double value = amplitude * std::sin(2.0 * M_PI * frequency * t);
            samples[i] = static_cast<int16_t>(value * 32767.0);
        }
        
        return samples;
    }
    
    // Helper function to generate silence
    std::vector<int16_t> generateSilence(size_t numSamples) {
        return std::vector<int16_t>(numSamples, 0);
    }
    
    WeNetConfig config;
    std::unique_ptr<MockTranscriptionCallback> mockCallback;
};

// Test circular buffer functionality
TEST(CircularBufferTest, BasicOperations) {
    CircularBuffer<int> buffer(5);
    
    // Test empty state
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
    
    // Test push
    EXPECT_TRUE(buffer.push(1));
    EXPECT_TRUE(buffer.push(2));
    EXPECT_TRUE(buffer.push(3));
    EXPECT_EQ(buffer.size(), 3);
    EXPECT_FALSE(buffer.empty());
    
    // Test pop
    auto val = buffer.pop();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1);
    EXPECT_EQ(buffer.size(), 2);
    
    // Fill buffer to capacity
    EXPECT_TRUE(buffer.push(4));
    EXPECT_TRUE(buffer.push(5));
    EXPECT_TRUE(buffer.push(6));
    EXPECT_EQ(buffer.size(), 5);
    
    // Test overflow
    EXPECT_FALSE(buffer.push(7));
    EXPECT_EQ(buffer.size(), 5);
    
    // Test wraparound
    buffer.pop();
    EXPECT_TRUE(buffer.push(7));
    
    // Pop all elements
    std::vector<int> popped;
    while (auto v = buffer.pop()) {
        popped.push_back(v.value());
    }
    EXPECT_THAT(popped, ElementsAre(3, 4, 5, 6, 7));
    EXPECT_TRUE(buffer.empty());
}

// Test RAII wrapper
TEST(WenetDecoderWrapperTest, RAIIBehavior) {
    // Test that constructor throws on invalid path
    EXPECT_THROW({
        WenetDecoderWrapper wrapper("");
    }, std::runtime_error);
    
    // Test move semantics
    {
        WenetDecoderWrapper wrapper1("/tmp/test_model");
        EXPECT_TRUE(wrapper1);
        
        WenetDecoderWrapper wrapper2(std::move(wrapper1));
        EXPECT_FALSE(wrapper1);
        EXPECT_TRUE(wrapper2);
        
        WenetDecoderWrapper wrapper3("/tmp/test_model2");
        wrapper3 = std::move(wrapper2);
        EXPECT_FALSE(wrapper2);
        EXPECT_TRUE(wrapper3);
    }
    // Destructors should be called without issues
}

// Test VAD functionality
TEST_F(WenetSTTImplTest, VoiceActivityDetection) {
    // Create implementation with mock
    WenetSTTImplImproved impl(config, mockCallback.get());
    
    // Test with silence - should detect no speech
    auto silence = generateSilence(1600); // 0.1 seconds
    AudioChunk silentChunk(silence, 1000);
    
    // Test with speech - should detect speech
    auto speech = generateTestAudio(1600, 440.0, 0.8); // Loud tone
    AudioChunk speechChunk(speech, 2000);
    
    // Note: In a real test, we'd need access to the private detectSpeech method
    // or test through the public interface
}

// Test error handling
TEST_F(WenetSTTImplTest, ErrorHandling) {
    // Test null callback
    EXPECT_THROW({
        WenetSTTImplImproved impl(config, nullptr);
    }, std::invalid_argument);
    
    // Test invalid model path
    config.modelPath = "";
    WenetSTTImplImproved impl(config, mockCallback.get());
    
    EXPECT_CALL(*mockCallback, onError(_))
        .Times(AtLeast(1));
    
    EXPECT_FALSE(impl.initialize());
}

// Test audio processing pipeline
TEST_F(WenetSTTImplTest, AudioProcessingPipeline) {
    WenetSTTImplImproved impl(config, mockCallback.get());
    
    // Expect initialization to succeed (with mock)
    EXPECT_TRUE(impl.initialize());
    EXPECT_TRUE(impl.isRunning());
    
    // Set up expectations
    EXPECT_CALL(*mockCallback, onTranscriptionResult(_))
        .Times(AtLeast(1))
        .WillRepeatedly([](const TranscriptionResult& result) {
            EXPECT_FALSE(result.text.empty());
            EXPECT_GE(result.confidence, 0.0);
            EXPECT_LE(result.confidence, 1.0);
        });
    
    // Process some audio chunks
    for (int i = 0; i < 10; ++i) {
        auto audio = generateTestAudio(1600); // 0.1 seconds chunks
        AudioChunk chunk(audio, i * 100);
        impl.processAudioChunk(chunk);
    }
    
    // Flush remaining audio
    impl.flush();
    
    // Give processing thread time to work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check metrics
    auto metrics = impl.getMetrics();
    EXPECT_GT(metrics.totalSamplesProcessed, 0);
    EXPECT_GE(metrics.totalProcessingTimeMs, 0);
}

// Test queue overflow handling
TEST_F(WenetSTTImplTest, QueueOverflow) {
    config.bufferCapacity = 5; // Small buffer
    WenetSTTImplImproved impl(config, mockCallback.get());
    
    EXPECT_TRUE(impl.initialize());
    
    // Expect error when queue overflows
    EXPECT_CALL(*mockCallback, onError(HasSubstr("queue full")))
        .Times(AtLeast(1));
    
    // Flood with audio chunks
    for (int i = 0; i < 100; ++i) {
        auto audio = generateTestAudio(16000); // 1 second chunks
        AudioChunk chunk(audio, i * 1000);
        impl.processAudioChunk(std::move(chunk));
    }
    
    // Check error count in metrics
    auto metrics = impl.getMetrics();
    EXPECT_GT(metrics.errorCount, 0);
}

// Test performance metrics
TEST_F(WenetSTTImplTest, PerformanceMetrics) {
    WenetSTTImplImproved impl(config, mockCallback.get());
    
    EXPECT_TRUE(impl.initialize());
    
    // Process known amount of audio
    const size_t totalSamples = 160000; // 10 seconds
    const size_t chunkSize = 1600; // 0.1 seconds
    
    for (size_t i = 0; i < totalSamples / chunkSize; ++i) {
        auto audio = generateTestAudio(chunkSize);
        AudioChunk chunk(audio, i * 100);
        impl.processAudioChunk(chunk);
    }
    
    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Check metrics
    auto metrics = impl.getMetrics();
    EXPECT_EQ(metrics.totalSamplesProcessed, totalSamples);
    EXPECT_GT(metrics.totalProcessingTimeMs, 0);
    EXPECT_GT(metrics.averageLatencyMs, 0.0);
}

// Test JSON parsing
TEST(JsonParsingTest, ParseTranscriptionResult) {
    std::string jsonResult = R"({
        "text": "hello world",
        "confidence": 0.95,
        "nbest": [
            {"sentence": "hello world", "confidence": 0.95},
            {"sentence": "hello word", "confidence": 0.85}
        ]
    })";
    
    rapidjson::Document doc;
    doc.Parse(jsonResult.c_str());
    
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_TRUE(doc.HasMember("text"));
    EXPECT_STREQ(doc["text"].GetString(), "hello world");
    EXPECT_DOUBLE_EQ(doc["confidence"].GetDouble(), 0.95);
    
    EXPECT_TRUE(doc.HasMember("nbest"));
    EXPECT_TRUE(doc["nbest"].IsArray());
    EXPECT_EQ(doc["nbest"].Size(), 2);
}

// Test thread safety
TEST_F(WenetSTTImplTest, ThreadSafety) {
    WenetSTTImplImproved impl(config, mockCallback.get());
    
    EXPECT_TRUE(impl.initialize());
    
    // Launch multiple threads sending audio
    const int numThreads = 4;
    std::vector<std::thread> threads;
    
    std::atomic<int> successCount(0);
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&impl, &successCount, t]() {
            for (int i = 0; i < 25; ++i) {
                auto audio = generateTestAudio(800);
                AudioChunk chunk(audio, t * 1000 + i);
                impl.processAudioChunk(chunk);
                successCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount, numThreads * 25);
    
    // Verify no crashes and processing completed
    impl.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto metrics = impl.getMetrics();
    EXPECT_GT(metrics.totalSamplesProcessed, 0);
}

// Test destructor behavior
TEST_F(WenetSTTImplTest, DestructorSafety) {
    {
        WenetSTTImplImproved impl(config, mockCallback.get());
        EXPECT_TRUE(impl.initialize());
        
        // Send some audio
        auto audio = generateTestAudio(16000);
        AudioChunk chunk(audio, 1000);
        impl.processAudioChunk(chunk);
        
        // Destructor should handle cleanup gracefully
    }
    
    // No crash should occur
    SUCCEED();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}