#ifndef SEQUENCE_TREE_HPP
#define SEQUENCE_TREE_HPP

#include "SequenceNode.hpp"
#include <vector>
#include <functional>

namespace stds {

/**
 * @brief Callback function type for node creation events
 */
using NodeCallback = std::function<void(const SequenceNode*)>;

/**
 * @brief Suffix-like Tree for sequential trading decision system
 */
class SequenceTree {
private:
    SequenceNode* root_;
    uint32_t next_id_;
    double confidence_threshold_;
    NodeCallback node_callback_;
    
    /**
     * @brief Calculate synthesis decision for a node
     * @param node The node to calculate synthesis for
     */
    void calculateSynthesis(SequenceNode* node);
    
public:
    /**
     * @brief Constructor
     * @param confidence_threshold Threshold for decision confidence (default 0.70)
     */
    explicit SequenceTree(double confidence_threshold = 0.70);
    
    /**
     * @brief Destructor
     */
    ~SequenceTree();
    
    /**
     * @brief Insert a sequence into the tree
     * @param sequence Vector of symbols representing market states
     * @param buy_signal Whether a buy signal was profitable
     * @param sell_signal Whether a sell signal was profitable
     */
    void insertSequence(const std::vector<int>& sequence, bool buy_signal, bool sell_signal);
    
    /**
     * @brief Query the tree for a decision given a sequence
     * @param sequence Vector of symbols representing current market state
     * @return Synthesis decision string ("BUY", "SELL", "HOLD", or "NONE")
     */
    std::string query(const std::vector<int>& sequence) const;
    
    /**
     * @brief Get the root node
     */
    const SequenceNode* getRoot() const { return root_; }
    
    /**
     * @brief Set callback for node creation events
     */
    void setNodeCallback(NodeCallback callback) { node_callback_ = callback; }
    
    /**
     * @brief Get total number of nodes in the tree
     */
    uint32_t getNodeCount() const { return next_id_; }
    
    /**
     * @brief Serialize tree to JSON format
     */
    std::string toJSON() const;
    
private:
    /**
     * @brief Helper function to serialize a node recursively
     */
    void serializeNode(const SequenceNode* node, std::string& json, bool is_last) const;
};

}  // namespace stds

#endif  // SEQUENCE_TREE_HPP
