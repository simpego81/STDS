#include "STDSEngine.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace stds {

STDSEngine::STDSEngine(const STDSConfig& config)
    : config_(config),
      normalizer_(config.num_bins),
      tree_(config.confidence_threshold) {
}

bool STDSEngine::loadData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    historical_data_.clear();
    std::string line;
    
    // Skip header line
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        OHLCV data;
        
        // Parse CSV: assuming format is Date,Open,High,Low,Close,Volume
        // Skip date
        std::getline(ss, token, ',');
        
        // Parse OHLCV
        std::getline(ss, token, ',');
        data.open = std::stod(token);
        
        std::getline(ss, token, ',');
        data.high = std::stod(token);
        
        std::getline(ss, token, ',');
        data.low = std::stod(token);
        
        std::getline(ss, token, ',');
        data.close = std::stod(token);
        
        std::getline(ss, token, ',');
        data.volume = std::stod(token);
        
        historical_data_.push_back(data);
    }
    
    file.close();
    
    if (historical_data_.empty()) {
        std::cerr << "No data loaded from file" << std::endl;
        return false;
    }
    
    // Fit the normalizer to the data
    normalizer_.fit(historical_data_);
    
    return true;
}

bool STDSEngine::checkProfitability(size_t start_index, bool is_buy) const {
    if (start_index >= historical_data_.size()) {
        return false;
    }
    
    double entry_price = historical_data_[start_index].close;
    size_t end_index = std::min(start_index + config_.lookahead_days, historical_data_.size());
    
    for (size_t i = start_index + 1; i < end_index; ++i) {
        double current_price = historical_data_[i].close;
        double return_pct = (current_price - entry_price) / entry_price;
        
        if (is_buy) {
            // For buy signal, check if price went up
            if (return_pct >= config_.take_profit_threshold) {
                return true;
            }
        } else {
            // For sell signal, check if price went down
            if (return_pct <= -config_.take_profit_threshold) {
                return true;
            }
        }
    }
    
    return false;
}

void STDSEngine::train() {
    if (historical_data_.size() < config_.sequence_length + config_.lookahead_days) {
        std::cerr << "Not enough data for training" << std::endl;
        return;
    }
    
    // Convert historical data to symbol sequence
    std::vector<int> symbols;
    symbols.reserve(historical_data_.size());
    
    for (size_t i = 1; i < historical_data_.size(); ++i) {
        double log_return = Normalizer::calculateLogReturn(
            historical_data_[i-1].close,
            historical_data_[i].close
        );
        int symbol = normalizer_.transform(log_return);
        symbols.push_back(symbol);
    }
    
    // Build sequences and insert into tree
    for (size_t i = 0; i + config_.sequence_length < symbols.size(); ++i) {
        // Extract sequence
        std::vector<int> sequence(
            symbols.begin() + i,
            symbols.begin() + i + config_.sequence_length
        );
        
        // Check profitability for buy and sell signals
        size_t data_index = i + config_.sequence_length;
        bool buy_profitable = checkProfitability(data_index, true);
        bool sell_profitable = checkProfitability(data_index, false);
        
        // Insert into tree
        tree_.insertSequence(sequence, buy_profitable, sell_profitable);
    }
}

std::string STDSEngine::processNewData(const OHLCV& data) {
    // Add to historical data
    historical_data_.push_back(data);
    
    // Calculate symbol for new data
    if (historical_data_.size() < 2) {
        return "NONE";
    }
    
    double log_return = Normalizer::calculateLogReturn(
        historical_data_[historical_data_.size() - 2].close,
        data.close
    );
    int symbol = normalizer_.transform(log_return);
    
    // Update symbol sequence
    symbol_sequence_.push_back(symbol);
    
    // Keep only the last sequence_length symbols
    if (symbol_sequence_.size() > static_cast<size_t>(config_.sequence_length)) {
        symbol_sequence_.erase(symbol_sequence_.begin());
    }
    
    // Query the tree for decision
    if (symbol_sequence_.size() == static_cast<size_t>(config_.sequence_length)) {
        return tree_.query(symbol_sequence_);
    }
    
    return "NONE";
}

}  // namespace stds
