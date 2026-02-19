#ifndef SEQUENCE_NODE_HPP
#define SEQUENCE_NODE_HPP

#include <cstdint>
#include <map>
#include <string>

namespace stds {

/**
 * @brief Statistics for trading decisions at a node
 */
struct Stats {
    uint32_t buy_wins = 0;
    uint32_t sell_wins = 0;
    uint32_t hold_count = 0;
};

/**
 * @brief Node in the Suffix-like Tree representing a market state sequence
 */
struct SequenceNode {
    uint32_t id;
    int symbol;
    uint64_t weight;  // Frequency of the subsequence
    std::map<int, SequenceNode*> children;
    Stats stats;
    std::string synthesis;  // Decision: "BUY", "SELL", "HOLD", or "NONE"
    
    SequenceNode(uint32_t node_id, int sym)
        : id(node_id), symbol(sym), weight(0), synthesis("NONE") {}
    
    ~SequenceNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

}  // namespace stds

#endif  // SEQUENCE_NODE_HPP
