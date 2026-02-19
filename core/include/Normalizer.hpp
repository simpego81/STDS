#ifndef NORMALIZER_HPP
#define NORMALIZER_HPP

#include <vector>
#include <cmath>
#include <algorithm>

namespace stds {

/**
 * @brief OHLCV data structure
 */
struct OHLCV {
    double open;
    double high;
    double low;
    double close;
    double volume;
};

/**
 * @brief Normalizer for log-return quantization
 */
class Normalizer {
private:
    int num_bins_;
    std::vector<double> bin_edges_;
    
public:
    /**
     * @brief Constructor
     * @param num_bins Number of discrete bins for quantization
     */
    explicit Normalizer(int num_bins = 10);
    
    /**
     * @brief Calculate log-return from two consecutive close prices
     * @param prev_close Previous close price
     * @param curr_close Current close price
     * @return Log-return value
     */
    static double calculateLogReturn(double prev_close, double curr_close);
    
    /**
     * @brief Fit the normalizer to historical data
     * @param data Vector of OHLCV data
     */
    void fit(const std::vector<OHLCV>& data);
    
    /**
     * @brief Transform a log-return into a discrete symbol
     * @param log_return The log-return value
     * @return Integer symbol representing the bin
     */
    int transform(double log_return) const;
    
    /**
     * @brief Get the number of bins
     */
    int getNumBins() const { return num_bins_; }
    
    /**
     * @brief Get bin edges
     */
    const std::vector<double>& getBinEdges() const { return bin_edges_; }
};

}  // namespace stds

#endif  // NORMALIZER_HPP
