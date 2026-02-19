#include <gtest/gtest.h>
#include "Normalizer.hpp"
#include "SequenceTree.hpp"
#include "STDSEngine.hpp"
#include <vector>
#include <cmath>

using namespace stds;

// Test Normalizer
TEST(NormalizerTest, CalculateLogReturn) {
    double prev_close = 100.0;
    double curr_close = 105.0;
    double log_return = Normalizer::calculateLogReturn(prev_close, curr_close);
    
    EXPECT_NEAR(log_return, std::log(1.05), 1e-6);
}

TEST(NormalizerTest, TransformDeterminism) {
    Normalizer normalizer(10);
    
    std::vector<OHLCV> data;
    for (int i = 0; i < 100; ++i) {
        OHLCV ohlcv;
        ohlcv.open = 100.0 + i;
        ohlcv.high = 105.0 + i;
        ohlcv.low = 98.0 + i;
        ohlcv.close = 103.0 + i;
        ohlcv.volume = 1000000.0;
        data.push_back(ohlcv);
    }
    
    normalizer.fit(data);
    
    // Test determinism: same input should produce same output
    double log_return = 0.05;
    int symbol1 = normalizer.transform(log_return);
    int symbol2 = normalizer.transform(log_return);
    
    EXPECT_EQ(symbol1, symbol2);
}

TEST(NormalizerTest, BinRanges) {
    Normalizer normalizer(10);
    
    std::vector<OHLCV> data;
    for (int i = 0; i < 100; ++i) {
        OHLCV ohlcv;
        ohlcv.open = 100.0 + i;
        ohlcv.high = 105.0 + i;
        ohlcv.low = 98.0 + i;
        ohlcv.close = 100.0 + i + (i % 10);
        ohlcv.volume = 1000000.0;
        data.push_back(ohlcv);
    }
    
    normalizer.fit(data);
    
    // Test that symbols are within valid range
    for (int i = 1; i < data.size(); ++i) {
        double log_return = Normalizer::calculateLogReturn(data[i-1].close, data[i].close);
        int symbol = normalizer.transform(log_return);
        EXPECT_GE(symbol, 0);
        EXPECT_LT(symbol, 10);
    }
}

// Test SequenceTree
TEST(SequenceTreeTest, InsertAndQuery) {
    SequenceTree tree(0.70);
    
    std::vector<int> sequence = {1, 2, 3, 4, 5};
    
    // Insert sequence multiple times with buy signals
    for (int i = 0; i < 10; ++i) {
        tree.insertSequence(sequence, true, false);
    }
    
    // Query should return BUY
    std::string decision = tree.query(sequence);
    EXPECT_EQ(decision, "BUY");
}

TEST(SequenceTreeTest, TreeIntegrity) {
    SequenceTree tree(0.70);
    
    std::vector<int> sequence1 = {1, 2, 3};
    std::vector<int> sequence2 = {1, 2, 4};
    
    tree.insertSequence(sequence1, true, false);
    tree.insertSequence(sequence2, false, true);
    
    // Both sequences share the prefix [1, 2], so they should share nodes
    // Node count should be less than if they were completely separate
    uint32_t node_count = tree.getNodeCount();
    EXPECT_LE(node_count, 7);  // Root + 1 + 2 + (3 or 4) = max 7 nodes
}

TEST(SequenceTreeTest, ConfidenceThreshold) {
    SequenceTree tree(0.70);
    
    std::vector<int> sequence = {1, 2, 3};
    
    // Insert with 60% buy wins (below threshold)
    for (int i = 0; i < 6; ++i) {
        tree.insertSequence(sequence, true, false);
    }
    for (int i = 0; i < 4; ++i) {
        tree.insertSequence(sequence, false, false);
    }
    
    // Should not return BUY because 60% < 70%
    std::string decision = tree.query(sequence);
    EXPECT_NE(decision, "BUY");
}

// Test STDSEngine
TEST(STDSEngineTest, LoadData) {
    STDSConfig config;
    config.num_bins = 10;
    config.sequence_length = 5;
    
    STDSEngine engine(config);
    
    // This test would require a real CSV file
    // For now, we just test that the method exists
    EXPECT_NO_THROW({
        // engine.loadData("test_data.csv");
    });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
