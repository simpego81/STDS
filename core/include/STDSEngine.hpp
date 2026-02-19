#ifndef STDS_ENGINE_HPP
#define STDS_ENGINE_HPP

#include "Normalizer.hpp"
#include "SequenceTree.hpp"
#include <string>
#include <vector>

namespace stds {

/**
 * @brief Configuration for the STDS engine
 */
struct STDSConfig {
    int num_bins = 10;
    int sequence_length = 5;
    double confidence_threshold = 0.70;
    int lookahead_days = 5;
    double take_profit_threshold = 0.02;  // 2% profit target
};

/**
 * @brief Main engine for Sequential Trading Decision System
 */
class STDSEngine {
private:
    STDSConfig config_;
    Normalizer normalizer_;
    SequenceTree tree_;
    std::vector<OHLCV> historical_data_;
    std::vector<int> symbol_sequence_;
    
    /**
     * @brief Check if a buy/sell signal would be profitable
     * @param start_index Starting index in historical data
     * @param is_buy True for buy signal, false for sell signal
     * @return True if profitable, false otherwise
     */
    bool checkProfitability(size_t start_index, bool is_buy) const;
    
public:
    /**
     * @brief Constructor
     * @param config Configuration parameters
     */
    explicit STDSEngine(const STDSConfig& config = STDSConfig());
    
    /**
     * @brief Load historical data from CSV file
     * @param filename Path to CSV file with OHLCV data
     * @return True if successful, false otherwise
     */
    bool loadData(const std::string& filename);
    
    /**
     * @brief Train the model on historical data
     */
    void train();
    
    /**
     * @brief Process a new OHLCV data point and get decision
     * @param data New OHLCV data
     * @return Trading decision ("BUY", "SELL", "HOLD", or "NONE")
     */
    std::string processNewData(const OHLCV& data);
    
    /**
     * @brief Get the current sequence tree
     */
    const SequenceTree& getTree() const { return tree_; }
    
    /**
     * @brief Get the normalizer
     */
    const Normalizer& getNormalizer() const { return normalizer_; }
    
    /**
     * @brief Set node callback for real-time updates
     */
    void setNodeCallback(NodeCallback callback) { tree_.setNodeCallback(callback); }
    
    /**
     * @brief Get tree as JSON
     */
    std::string getTreeJSON() const { return tree_.toJSON(); }
};

}  // namespace stds

#endif  // STDS_ENGINE_HPP
