#include "SequenceTree.hpp"
#include <sstream>
#include <iomanip>

namespace stds {

SequenceTree::SequenceTree(double confidence_threshold)
    : root_(new SequenceNode(0, -1)), next_id_(1), confidence_threshold_(confidence_threshold) {
}

SequenceTree::~SequenceTree() {
    delete root_;
}

void SequenceTree::calculateSynthesis(SequenceNode* node) {
    uint64_t total_visits = node->weight;
    
    if (total_visits == 0) {
        node->synthesis = "NONE";
        return;
    }
    
    double buy_ratio = static_cast<double>(node->stats.buy_wins) / total_visits;
    double sell_ratio = static_cast<double>(node->stats.sell_wins) / total_visits;
    
    if (buy_ratio > confidence_threshold_) {
        node->synthesis = "BUY";
    } else if (sell_ratio > confidence_threshold_) {
        node->synthesis = "SELL";
    } else if (buy_ratio > 0.4 || sell_ratio > 0.4) {
        node->synthesis = "HOLD";
    } else {
        node->synthesis = "NONE";
    }
}

void SequenceTree::insertSequence(const std::vector<int>& sequence, bool buy_signal, bool sell_signal) {
    if (sequence.empty()) {
        return;
    }
    
    SequenceNode* current = root_;
    
    // Traverse or create path for the sequence
    for (int symbol : sequence) {
        auto it = current->children.find(symbol);
        
        if (it == current->children.end()) {
            // Create new node
            SequenceNode* new_node = new SequenceNode(next_id_++, symbol);
            current->children[symbol] = new_node;
            current = new_node;
            
            // Notify callback if set
            if (node_callback_) {
                node_callback_(new_node);
            }
        } else {
            current = it->second;
        }
        
        // Update weight (frequency)
        current->weight++;
    }
    
    // Update statistics at the final node
    if (buy_signal) {
        current->stats.buy_wins++;
    }
    if (sell_signal) {
        current->stats.sell_wins++;
    }
    if (!buy_signal && !sell_signal) {
        current->stats.hold_count++;
    }
    
    // Recalculate synthesis
    calculateSynthesis(current);
}

std::string SequenceTree::query(const std::vector<int>& sequence) const {
    if (sequence.empty()) {
        return "NONE";
    }
    
    const SequenceNode* current = root_;
    
    // Traverse the tree following the sequence
    for (int symbol : sequence) {
        auto it = current->children.find(symbol);
        if (it == current->children.end()) {
            return "NONE";  // Sequence not found
        }
        current = it->second;
    }
    
    return current->synthesis;
}

void SequenceTree::serializeNode(const SequenceNode* node, std::string& json, bool is_last) const {
    json += "{";
    json += "\"id\":" + std::to_string(node->id) + ",";
    json += "\"symbol\":" + std::to_string(node->symbol) + ",";
    json += "\"weight\":" + std::to_string(node->weight) + ",";
    json += "\"synthesis\":\"" + node->synthesis + "\",";
    json += "\"stats\":{";
    json += "\"buy_wins\":" + std::to_string(node->stats.buy_wins) + ",";
    json += "\"sell_wins\":" + std::to_string(node->stats.sell_wins) + ",";
    json += "\"hold_count\":" + std::to_string(node->stats.hold_count);
    json += "},";
    json += "\"children\":[";
    
    size_t child_count = node->children.size();
    size_t idx = 0;
    for (const auto& pair : node->children) {
        serializeNode(pair.second, json, idx == child_count - 1);
        ++idx;
    }
    
    json += "]";
    json += "}";
    
    if (!is_last) {
        json += ",";
    }
}

std::string SequenceTree::toJSON() const {
    std::string json = "{\"root\":";
    serializeNode(root_, json, true);
    json += "}";
    return json;
}

}  // namespace stds
