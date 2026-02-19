#include "Normalizer.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace stds {

Normalizer::Normalizer(int num_bins) : num_bins_(num_bins) {
    // Initialize with default bin edges (will be updated by fit)
    bin_edges_.resize(num_bins_ - 1, 0.0);
}

double Normalizer::calculateLogReturn(double prev_close, double curr_close) {
    if (prev_close <= 0.0 || curr_close <= 0.0) {
        return 0.0;  // Invalid data
    }
    return std::log(curr_close / prev_close);
}

void Normalizer::fit(const std::vector<OHLCV>& data) {
    if (data.size() < 2) {
        return;  // Not enough data to calculate returns
    }
    
    // Calculate all log-returns
    std::vector<double> log_returns;
    log_returns.reserve(data.size() - 1);
    
    for (size_t i = 1; i < data.size(); ++i) {
        double log_return = calculateLogReturn(data[i-1].close, data[i].close);
        if (std::isfinite(log_return)) {
            log_returns.push_back(log_return);
        }
    }
    
    if (log_returns.empty()) {
        return;
    }
    
    // Sort log-returns to compute quantiles
    std::sort(log_returns.begin(), log_returns.end());
    
    // Compute bin edges based on quantiles
    bin_edges_.clear();
    for (int i = 1; i < num_bins_; ++i) {
        double quantile = static_cast<double>(i) / num_bins_;
        size_t index = static_cast<size_t>(quantile * log_returns.size());
        if (index >= log_returns.size()) {
            index = log_returns.size() - 1;
        }
        bin_edges_.push_back(log_returns[index]);
    }
}

int Normalizer::transform(double log_return) const {
    if (!std::isfinite(log_return)) {
        return num_bins_ / 2;  // Return middle bin for invalid data
    }
    
    // Find the appropriate bin
    int bin = 0;
    for (double edge : bin_edges_) {
        if (log_return < edge) {
            break;
        }
        ++bin;
    }
    
    return bin;
}

}  // namespace stds
