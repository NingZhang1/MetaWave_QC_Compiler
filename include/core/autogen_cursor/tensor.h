#pragma once

#include "symbol.h"
#include "index.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace qc
{

    /**
     * @brief Represents tensor objects in quantum chemistry expressions
     */
    class Tensor
    {
    public:
        enum class Type
        {
            GENERAL,
            SYMMETRIC,
            ANTISYMMETRIC,
            HERMITIAN,
            UNITARY
        };

        enum class Rank
        {
            SCALAR = 0,
            VECTOR = 1,
            MATRIX = 2,
            RANK3 = 3,
            RANK4 = 4,
            RANK_N = -1 // General rank
        };

    private:
        std::unique_ptr<Symbol> symbol_;
        IndexSet indices_;
        Type type_;
        Rank rank_;
        std::unordered_map<std::string, std::string> properties_;

    public:
        // Constructors
        Tensor(const Symbol &symbol, const IndexSet &indices,
               Type type = Type::GENERAL);
        Tensor(const std::string &name, const IndexSet &indices,
               Type type = Type::GENERAL);
        Tensor(const Tensor &other);
        Tensor &operator=(const Tensor &other);
        Tensor(Tensor &&) = default;
        Tensor &operator=(Tensor &&) = default;

        // Accessors
        const Symbol &symbol() const { return *symbol_; }
        const IndexSet &indices() const { return indices_; }
        Type type() const { return type_; }
        Rank rank() const { return rank_; }
        size_t actual_rank() const { return indices_.size(); }

        // Setters
        void set_type(Type type) { type_ = type; }
        void set_indices(const IndexSet &indices);

        // Property management
        void set_property(const std::string &key, const std::string &value);
        std::string get_property(const std::string &key) const;
        bool has_property(const std::string &key) const;

        // Type checking
        bool is_symmetric() const { return type_ == Type::SYMMETRIC; }
        bool is_antisymmetric() const { return type_ == Type::ANTISYMMETRIC; }
        bool is_hermitian() const { return type_ == Type::HERMITIAN; }
        bool is_unitary() const { return type_ == Type::UNITARY; }

        bool is_scalar() const { return actual_rank() == 0; }
        bool is_vector() const { return actual_rank() == 1; }
        bool is_matrix() const { return actual_rank() == 2; }

        // Symmetry operations
        bool has_symmetric_indices() const;
        bool has_antisymmetric_indices() const;
        std::vector<std::pair<size_t, size_t>> get_symmetric_pairs() const;

        // Index operations
        bool shares_indices(const Tensor &other) const;
        IndexSet common_indices(const Tensor &other) const;
        bool can_contract_with(const Tensor &other) const;

        // Comparison
        bool operator==(const Tensor &other) const;
        bool operator!=(const Tensor &other) const;
        bool operator<(const Tensor &other) const;

        // String representation
        std::string to_string() const;

        // Hash support
        std::size_t hash() const;

        // Clone
        std::unique_ptr<Tensor> clone() const;

        // Transpose operations
        Tensor transpose() const;
        Tensor transpose(const std::vector<size_t> &permutation) const;

        // Conjugate operations
        Tensor conjugate() const;
        Tensor hermitian_conjugate() const;
    };

    /**
     * @brief Factory class for creating common tensor types
     */
    class TensorFactory
    {
    public:
        // Common quantum chemistry tensors
        static Tensor one_electron_integral(const std::string &name,
                                            const Index &i, const Index &j);
        static Tensor two_electron_integral(const std::string &name,
                                            const Index &i, const Index &j,
                                            const Index &k, const Index &l);
        static Tensor amplitude_singles(const Index &i, const Index &a);
        static Tensor amplitude_doubles(const Index &i, const Index &j,
                                        const Index &a, const Index &b);

        // Density matrices
        static Tensor density_matrix(const std::string &name,
                                     const Index &p, const Index &q);
        static Tensor reduced_density_matrix(const std::string &name, int order,
                                             const IndexSet &indices);

        // Operators
        static Tensor creation_operator(const Index &p);
        static Tensor annihilation_operator(const Index &p);
        static Tensor number_operator(const Index &p);

        // Metric tensors
        static Tensor kronecker_delta(const Index &i, const Index &j);
        static Tensor levi_civita(const IndexSet &indices);

        // Identity and zero tensors
        static Tensor identity(size_t rank);
        static Tensor zero(const IndexSet &indices);
    };

    /**
     * @brief Tensor contraction utilities
     */
    class TensorContraction
    {
    public:
        struct ContractionPath
        {
            std::vector<std::pair<size_t, size_t>> tensor_pairs;
            std::vector<IndexSet> contracted_indices;
            double cost_estimate;
        };

        static Tensor contract(const Tensor &A, const Tensor &B,
                               const IndexSet &contracted_indices);
        static ContractionPath optimize_contraction(const std::vector<Tensor> &tensors);
        static double estimate_contraction_cost(const Tensor &A, const Tensor &B,
                                                const IndexSet &contracted_indices);
    };

} // namespace qc

// Hash specialization
namespace std
{
    template <>
    struct hash<qc::Tensor>
    {
        std::size_t operator()(const qc::Tensor &t) const
        {
            return t.hash();
        }
    };
}