#pragma once

#include <string>
#include <vector>
#include <memory>
#include <set>

namespace qc {

/**
 * @brief Represents tensor indices in quantum chemistry expressions
 */
class Index {
public:
    enum class Type {
        OCCUPIED,      // occupied orbital indices (i, j, k, ...)
        VIRTUAL,       // virtual orbital indices (a, b, c, ...)
        GENERAL,       // general indices (p, q, r, ...)
        SPIN,          // spin indices (α, β)
        SPATIAL        // spatial orbital indices
    };

    enum class Symmetry {
        NONE,
        SYMMETRIC,
        ANTISYMMETRIC
    };

private:
    std::string label_;
    Type type_;
    int range_start_;
    int range_end_;
    Symmetry symmetry_;

public:
    Index(const std::string& label, Type type = Type::GENERAL, 
          int range_start = 0, int range_end = -1, 
          Symmetry symmetry = Symmetry::NONE);
    
    // Accessors
    const std::string& label() const { return label_; }
    Type type() const { return type_; }
    int range_start() const { return range_start_; }
    int range_end() const { return range_end_; }
    Symmetry symmetry() const { return symmetry_; }
    
    // Setters
    void set_range(int start, int end);
    void set_symmetry(Symmetry sym) { symmetry_ = sym; }
    
    // Type checking
    bool is_occupied() const { return type_ == Type::OCCUPIED; }
    bool is_virtual() const { return type_ == Type::VIRTUAL; }
    bool is_general() const { return type_ == Type::GENERAL; }
    bool is_spin() const { return type_ == Type::SPIN; }
    bool is_spatial() const { return type_ == Type::SPATIAL; }
    
    // Symmetry checking
    bool is_symmetric() const { return symmetry_ == Symmetry::SYMMETRIC; }
    bool is_antisymmetric() const { return symmetry_ == Symmetry::ANTISYMMETRIC; }
    
    // Comparison operators
    bool operator==(const Index& other) const;
    bool operator!=(const Index& other) const;
    bool operator<(const Index& other) const;
    
    // String representation
    std::string to_string() const;
    
    // Hash support
    std::size_t hash() const;
    
    // Clone
    std::unique_ptr<Index> clone() const;
};

/**
 * @brief Container for multiple indices with convenience methods
 */
class IndexSet {
private:
    std::vector<std::unique_ptr<Index>> indices_;

public:
    IndexSet() = default;
    IndexSet(const std::vector<Index>& indices);
    IndexSet(const IndexSet& other);
    IndexSet& operator=(const IndexSet& other);
    IndexSet(IndexSet&&) = default;
    IndexSet& operator=(IndexSet&&) = default;
    
    // Container interface
    void add_index(const Index& idx);
    void add_index(std::unique_ptr<Index> idx);
    size_t size() const { return indices_.size(); }
    bool empty() const { return indices_.empty(); }
    
    // Access
    const Index& operator[](size_t i) const { return *indices_[i]; }
    Index& operator[](size_t i) { return *indices_[i]; }
    
    // Iterators
    auto begin() const { return indices_.begin(); }
    auto end() const { return indices_.end(); }
    auto begin() { return indices_.begin(); }
    auto end() { return indices_.end(); }
    
    // Operations
    IndexSet operator+(const IndexSet& other) const;
    bool contains(const Index& idx) const;
    std::set<std::string> get_labels() const;
    
    // Find common indices between two sets
    IndexSet find_common(const IndexSet& other) const;
    IndexSet find_unique() const;
    
    // Symmetry operations
    bool has_repeated_indices() const;
    std::vector<std::pair<size_t, size_t>> find_symmetric_pairs() const;
    
    // String representation
    std::string to_string() const;
    
    // Clone
    std::unique_ptr<IndexSet> clone() const;
};

/**
 * @brief Factory functions for creating common index types
 */
namespace IndexFactory {
    Index occupied(const std::string& label, int range_end = -1);
    Index virtual_orbital(const std::string& label, int range_end = -1);
    Index general(const std::string& label, int range_end = -1);
    Index spin(const std::string& label);
    Index spatial(const std::string& label, int range_end = -1);
    
    // Create sets of indices
    IndexSet occupied_set(const std::vector<std::string>& labels);
    IndexSet virtual_set(const std::vector<std::string>& labels);
    IndexSet general_set(const std::vector<std::string>& labels);
}

} // namespace qc

// Hash specialization for std::unordered_map usage
namespace std {
    template<>
    struct hash<qc::Index> {
        std::size_t operator()(const qc::Index& idx) const {
            return idx.hash();
        }
    };
} 