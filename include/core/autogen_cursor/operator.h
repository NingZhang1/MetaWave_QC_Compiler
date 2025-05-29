#pragma once

#include "tensor.h"
#include "symbol.h"
#include "index.h"
#include <memory>
#include <vector>
#include <functional>

namespace qc
{

    /**
     * @brief Represents quantum mechanical operators
     */
    class Operator
    {
    public:
        enum class Type
        {
            CREATION,
            ANNIHILATION,
            NUMBER,
            HAMILTONIAN,
            DENSITY,
            GENERAL,
            COMPOSITE
        };

        enum class Algebra
        {
            FERMION, // Anticommuting (fermionic)
            BOSON,   // Commuting (bosonic)
            GENERAL  // General non-commuting
        };

    private:
        std::unique_ptr<Symbol> symbol_;
        IndexSet indices_;
        Type type_;
        Algebra algebra_;
        std::unordered_map<std::string, std::string> properties_;

    public:
        // Constructors
        Operator(const Symbol &symbol, const IndexSet &indices,
                 Type type = Type::GENERAL, Algebra algebra = Algebra::GENERAL);
        Operator(const std::string &name, const IndexSet &indices,
                 Type type = Type::GENERAL, Algebra algebra = Algebra::GENERAL);
        Operator(const Operator &other);
        Operator &operator=(const Operator &other);
        Operator(Operator &&) = default;
        Operator &operator=(Operator &&) = default;

        // Accessors
        const Symbol &symbol() const { return *symbol_; }
        const IndexSet &indices() const { return indices_; }
        Type type() const { return type_; }
        Algebra algebra() const { return algebra_; }

        // Setters
        void set_type(Type type) { type_ = type; }
        void set_algebra(Algebra algebra) { algebra_ = algebra; }
        void set_indices(const IndexSet &indices);

        // Property management
        void set_property(const std::string &key, const std::string &value);
        std::string get_property(const std::string &key) const;
        bool has_property(const std::string &key) const;

        // Type checking
        bool is_creation() const { return type_ == Type::CREATION; }
        bool is_annihilation() const { return type_ == Type::ANNIHILATION; }
        bool is_number() const { return type_ == Type::NUMBER; }
        bool is_hamiltonian() const { return type_ == Type::HAMILTONIAN; }
        bool is_density() const { return type_ == Type::DENSITY; }

        // Algebra checking
        bool is_fermionic() const { return algebra_ == Algebra::FERMION; }
        bool is_bosonic() const { return algebra_ == Algebra::BOSON; }
        bool anticommutes_with(const Operator &other) const;
        bool commutes_with(const Operator &other) const;

        // Comparison
        bool operator==(const Operator &other) const;
        bool operator!=(const Operator &other) const;
        bool operator<(const Operator &other) const;

        // String representation
        std::string to_string() const;

        // Hash support
        std::size_t hash() const;

        // Clone
        std::unique_ptr<Operator> clone() const;

        // Adjoint operations
        Operator adjoint() const;
        Operator hermitian_conjugate() const;

        // Normal ordering
        bool is_normal_ordered() const;
        int normal_ordering_sign() const;
    };

    /**
     * @brief Represents products of operators
     */
    class OperatorProduct
    {
    private:
        std::vector<Operator> operators_;
        double coefficient_;
        bool is_normal_ordered_;

    public:
        OperatorProduct(double coefficient = 1.0);
        OperatorProduct(const std::vector<Operator> &operators, double coefficient = 1.0);

        // Accessors
        const std::vector<Operator> &operators() const { return operators_; }
        double coefficient() const { return coefficient_; }
        bool is_normal_ordered() const { return is_normal_ordered_; }
        size_t size() const { return operators_.size(); }

        // Modifiers
        void add_operator(const Operator &op);
        void set_coefficient(double coeff) { coefficient_ = coeff; }
        void multiply_coefficient(double factor) { coefficient_ *= factor; }

        // Operations
        OperatorProduct operator*(const OperatorProduct &other) const;
        OperatorProduct operator*(double scalar) const;
        OperatorProduct &operator*=(double scalar);

        // Normal ordering
        OperatorProduct normal_order() const;
        void set_normal_ordered(bool ordered) { is_normal_ordered_ = ordered; }

        // Comparison
        bool operator==(const OperatorProduct &other) const;
        bool operator!=(const OperatorProduct &other) const;

        // String representation
        std::string to_string() const;

        // Clone
        std::unique_ptr<OperatorProduct> clone() const;
    };

    /**
     * @brief Commutator and anticommutator operations
     */
    class CommutatorAlgebra
    {
    public:
        // Commutator [A, B] = AB - BA
        static OperatorProduct commutator(const Operator &A, const Operator &B);
        static OperatorProduct commutator(const OperatorProduct &A, const OperatorProduct &B);

        // Anticommutator {A, B} = AB + BA
        static OperatorProduct anticommutator(const Operator &A, const Operator &B);
        static OperatorProduct anticommutator(const OperatorProduct &A, const OperatorProduct &B);

        // Nested commutators
        static OperatorProduct nested_commutator(const std::vector<Operator> &operators);

        // Baker-Campbell-Hausdorff expansion (truncated)
        static std::vector<OperatorProduct> bch_expansion(const Operator &A, const Operator &B, int order = 4);

        // Canonical commutation relations
        static OperatorProduct canonical_commutation(const Operator &p, const Operator &q);
        static OperatorProduct canonical_anticommutation(const Operator &a, const Operator &a_dag);

        // Evaluate commutator relations
        static bool is_zero_commutator(const Operator &A, const Operator &B);
        static double evaluate_commutator_coefficient(const Operator &A, const Operator &B);
    };

    /**
     * @brief Factory for creating common quantum operators
     */
    class OperatorFactory
    {
    public:
        // Second quantization operators
        static Operator creation(const Index &p, Algebra algebra = Algebra::FERMION);
        static Operator annihilation(const Index &p, Algebra algebra = Algebra::FERMION);
        static Operator number(const Index &p, Algebra algebra = Algebra::FERMION);

        // Many-body operators
        static OperatorProduct one_body_operator(const Tensor &h,
                                                 const IndexSet &occupied,
                                                 const IndexSet &virtual_orbs);
        static OperatorProduct two_body_operator(const Tensor &g,
                                                 const IndexSet &occupied,
                                                 const IndexSet &virtual_orbs);

        // Cluster operators
        static OperatorProduct cluster_operator_singles(const Tensor &t1);
        static OperatorProduct cluster_operator_doubles(const Tensor &t2);
        static OperatorProduct cluster_operator(const std::vector<Tensor> &amplitudes);

        // Excitation operators
        static OperatorProduct single_excitation(const Index &i, const Index &a);
        static OperatorProduct double_excitation(const Index &i, const Index &j,
                                                 const Index &a, const Index &b);

        // Angular momentum operators
        static Operator angular_momentum_plus(const Index &j, const Index &m);
        static Operator angular_momentum_minus(const Index &j, const Index &m);
        static Operator angular_momentum_z(const Index &j, const Index &m);

        // Spin operators
        static Operator spin_x();
        static Operator spin_y();
        static Operator spin_z();
        static Operator spin_plus();
        static Operator spin_minus();
    };

} // namespace qc

// Hash specializations
namespace std
{
    template <>
    struct hash<qc::Operator>
    {
        std::size_t operator()(const qc::Operator &op) const
        {
            return op.hash();
        }
    };

    template <>
    struct hash<qc::OperatorProduct>
    {
        std::size_t operator()(const qc::OperatorProduct &prod) const
        {
            std::size_t seed = 0;
            for (const auto &op : prod.operators())
            {
                seed ^= hash<qc::Operator>{}(op) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}