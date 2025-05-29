#pragma once

#include "../core/expression.h"
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

namespace qc
{

    /**
     * @brief Main simplification engine for quantum chemistry expressions
     */
    class Simplifier
    {
    public:
        // Simplification rule type
        using Rule = std::function<std::unique_ptr<Expression>(const Expression &)>;

        enum class RuleType
        {
            ALGEBRAIC,    // Basic algebraic rules
            DISTRIBUTIVE, // Distributive law
            ASSOCIATIVE,  // Associative rules
            COMMUTATIVE,  // Commutative rules
            TENSOR,       // Tensor-specific rules
            OPERATOR,     // Operator-specific rules
            COMMUTATOR,   // Commutator rules
            INDEX,        // Index simplification
            SYMMETRY      // Symmetry-based rules
        };

    private:
        std::unordered_map<RuleType, std::vector<Rule>> rules_;
        bool enable_trace_;
        std::vector<std::string> trace_log_;

    public:
        Simplifier(bool enable_trace = false);

        // Main simplification interface
        std::unique_ptr<Expression> simplify(const Expression &expr) const;
        std::unique_ptr<Expression> simplify_recursive(const Expression &expr) const;
        std::unique_ptr<Expression> simplify_with_rules(const Expression &expr,
                                                        const std::vector<RuleType> &rule_types) const;

        // Apply specific rule types
        std::unique_ptr<Expression> apply_algebraic_rules(const Expression &expr) const;
        std::unique_ptr<Expression> apply_distributive_rules(const Expression &expr) const;
        std::unique_ptr<Expression> apply_commutator_rules(const Expression &expr) const;
        std::unique_ptr<Expression> apply_tensor_rules(const Expression &expr) const;
        std::unique_ptr<Expression> apply_operator_rules(const Expression &expr) const;
        std::unique_ptr<Expression> apply_symmetry_rules(const Expression &expr) const;

        // Rule management
        void add_rule(RuleType type, const Rule &rule);
        void remove_rules(RuleType type);
        void enable_rule_type(RuleType type, bool enable = true);
        bool is_rule_type_enabled(RuleType type) const;

        // Trace functionality
        void enable_trace(bool enable = true) { enable_trace_ = enable; }
        const std::vector<std::string> &get_trace() const { return trace_log_; }
        void clear_trace() { trace_log_.clear(); }

    private:
        std::unique_ptr<Expression> apply_rules(const Expression &expr,
                                                const std::vector<Rule> &rules) const;
        void log_trace(const std::string &message) const;
        void initialize_default_rules();
    };

    /**
     * @brief Collection of algebraic simplification rules
     */
    class AlgebraicRules
    {
    public:
        // Identity rules
        static std::unique_ptr<Expression> identity_addition(const Expression &expr);       // x + 0 = x
        static std::unique_ptr<Expression> identity_multiplication(const Expression &expr); // x * 1 = x
        static std::unique_ptr<Expression> zero_multiplication(const Expression &expr);     // x * 0 = 0
        static std::unique_ptr<Expression> zero_power(const Expression &expr);              // 0^x = 0
        static std::unique_ptr<Expression> one_power(const Expression &expr);               // 1^x = 1
        static std::unique_ptr<Expression> power_of_one(const Expression &expr);            // x^1 = x

        // Inverse rules
        static std::unique_ptr<Expression> additive_inverse(const Expression &expr);       // x + (-x) = 0
        static std::unique_ptr<Expression> multiplicative_inverse(const Expression &expr); // x * (1/x) = 1

        // Combination rules
        static std::unique_ptr<Expression> combine_constants(const Expression &expr);
        static std::unique_ptr<Expression> combine_like_terms(const Expression &expr);

        // Power rules
        static std::unique_ptr<Expression> power_of_product(const Expression &expr);  // (xy)^n = x^n * y^n
        static std::unique_ptr<Expression> product_of_powers(const Expression &expr); // x^m * x^n = x^(m+n)
        static std::unique_ptr<Expression> power_of_power(const Expression &expr);    // (x^m)^n = x^(mn)
    };

    /**
     * @brief Distributive law implementations
     */
    class DistributiveRules
    {
    public:
        // Basic distributive law: (a+b)*(c+d) = ac + ad + bc + bd
        static std::unique_ptr<Expression> distribute_multiplication(const Expression &expr);

        // Reverse distributive: ax + bx = (a+b)*x
        static std::unique_ptr<Expression> factor_common_terms(const Expression &expr);

        // Distributive over subtraction: a*(b-c) = ab - ac
        static std::unique_ptr<Expression> distribute_over_subtraction(const Expression &expr);

        // Distributive for tensor operations
        static std::unique_ptr<Expression> distribute_tensor_multiplication(const Expression &expr);

        // Distributive for operator products
        static std::unique_ptr<Expression> distribute_operator_products(const Expression &expr);

        // Complex distributive patterns
        static std::unique_ptr<Expression> distribute_nested_products(const Expression &expr);
        static std::unique_ptr<Expression> distribute_powers(const Expression &expr); // (a+b)^2 = a^2 + 2ab + b^2
    };

    /**
     * @brief Commutator-specific simplification rules
     */
    class CommutatorRules
    {
    public:
        // Basic commutator properties
        static std::unique_ptr<Expression> antisymmetry(const Expression &expr);    // [A,B] = -[B,A]
        static std::unique_ptr<Expression> zero_commutator(const Expression &expr); // [A,A] = 0
        static std::unique_ptr<Expression> linearity_left(const Expression &expr);  // [aA+bB,C] = a[A,C] + b[B,C]
        static std::unique_ptr<Expression> linearity_right(const Expression &expr); // [A,bB+cC] = b[A,B] + c[A,C]

        // Jacobi identity: [A,[B,C]] + [B,[C,A]] + [C,[A,B]] = 0
        static std::unique_ptr<Expression> jacobi_identity(const Expression &expr);

        // Product rules
        static std::unique_ptr<Expression> product_rule(const Expression &expr); // [AB,C] = A[B,C] + [A,C]B

        // Canonical commutation relations
        static std::unique_ptr<Expression> canonical_position_momentum(const Expression &expr);     // [x,p] = iℏ
        static std::unique_ptr<Expression> canonical_creation_annihilation(const Expression &expr); // [a,a†] = 1

        // Expand commutators
        static std::unique_ptr<Expression> expand_commutator(const Expression &expr);     // [A,B] → AB - BA
        static std::unique_ptr<Expression> expand_anticommutator(const Expression &expr); // {A,B} → AB + BA
    };

    /**
     * @brief Tensor-specific simplification rules
     */
    class TensorRules
    {
    public:
        // Index contraction
        static std::unique_ptr<Expression> contract_kronecker_delta(const Expression &expr);
        static std::unique_ptr<Expression> simplify_repeated_indices(const Expression &expr);
        static std::unique_ptr<Expression> einstein_summation(const Expression &expr);

        // Symmetry-based simplifications
        static std::unique_ptr<Expression> symmetric_tensor_simplification(const Expression &expr);
        static std::unique_ptr<Expression> antisymmetric_tensor_simplification(const Expression &expr);

        // Tensor product rules
        static std::unique_ptr<Expression> tensor_product_associativity(const Expression &expr);
        static std::unique_ptr<Expression> tensor_trace_rules(const Expression &expr);

        // Index relabeling
        static std::unique_ptr<Expression> relabel_dummy_indices(const Expression &expr);
        static std::unique_ptr<Expression> canonical_index_ordering(const Expression &expr);
    };

    /**
     * @brief Operator-specific simplification rules
     */
    class OperatorRules
    {
    public:
        // Normal ordering
        static std::unique_ptr<Expression> normal_order_fermions(const Expression &expr);
        static std::unique_ptr<Expression> normal_order_bosons(const Expression &expr);

        // Wick's theorem
        static std::unique_ptr<Expression> wicks_theorem(const Expression &expr);

        // Operator algebra
        static std::unique_ptr<Expression> creation_annihilation_algebra(const Expression &expr);
        static std::unique_ptr<Expression> number_operator_simplification(const Expression &expr);

        // Vacuum expectation values
        static std::unique_ptr<Expression> vacuum_expectation(const Expression &expr);

        // Operator exponentials
        static std::unique_ptr<Expression> exponential_operator_expansion(const Expression &expr);
    };

    /**
     * @brief Symmetry-based simplification rules
     */
    class SymmetryRules
    {
    public:
        // Point group symmetries
        static std::unique_ptr<Expression> apply_point_group_symmetry(const Expression &expr,
                                                                      const std::string &point_group);

        // Permutation symmetry
        static std::unique_ptr<Expression> permutation_symmetry(const Expression &expr);

        // Time-reversal symmetry
        static std::unique_ptr<Expression> time_reversal_symmetry(const Expression &expr);

        // Particle-hole symmetry
        static std::unique_ptr<Expression> particle_hole_symmetry(const Expression &expr);

        // Crystal field symmetry
        static std::unique_ptr<Expression> crystal_field_symmetry(const Expression &expr);
    };

    /**
     * @brief Pattern matching utilities for expression simplification
     */
    class PatternMatcher
    {
    public:
        struct Pattern
        {
            std::function<bool(const Expression &)> match;
            std::function<std::unique_ptr<Expression>(const Expression &)> transform;
            std::string description;
        };

        static bool matches_pattern(const Expression &expr, const Pattern &pattern);
        static std::unique_ptr<Expression> apply_pattern(const Expression &expr, const Pattern &pattern);

        // Common pattern builders
        static Pattern binary_op_pattern(Expression::Type op_type);
        static Pattern commutator_pattern();
        static Pattern sum_pattern();
        static Pattern tensor_contraction_pattern();

        // Pattern matching for specific structures
        static bool is_distributive_candidate(const Expression &expr);
        static bool is_commutator_expansion_candidate(const Expression &expr);
        static bool has_common_factors(const Expression &expr);
    };

    /**
     * @brief Simplification strategies
     */
    class SimplificationStrategy
    {
    public:
        enum class Strategy
        {
            AGGRESSIVE,     // Apply all possible simplifications
            CONSERVATIVE,   // Apply only safe simplifications
            ALGEBRAIC_ONLY, // Only algebraic simplifications
            QUANTUM_ONLY,   // Only quantum-specific simplifications
            CUSTOM          // User-defined strategy
        };

        static std::vector<Simplifier::RuleType> get_rule_types(Strategy strategy);
        static Simplifier create_simplifier(Strategy strategy);
    };

} // namespace qc