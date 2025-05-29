#pragma once

#include "symbol.h"
#include "tensor.h"
#include "operator.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace qc
{

    /**
     * @brief Base class for all expression tree nodes
     */
    class Expression
    {
    public:
        enum class Type
        {
            SYMBOL,
            TENSOR,
            OPERATOR,
            OPERATOR_PRODUCT,
            ADD,
            SUBTRACT,
            MULTIPLY,
            DIVIDE,
            POWER,
            COMMUTATOR,
            ANTICOMMUTATOR,
            CONTRACT,
            SUM,          // Index summation
            DERIVATIVE,   // Partial derivative
            INTEGRAL,     // Integration
            FUNCTION_CALL // General function
        };

    protected:
        Type type_;
        std::vector<std::unique_ptr<Expression>> children_;
        std::unordered_map<std::string, std::string> properties_;

    public:
        Expression(Type type) : type_(type) {}
        virtual ~Expression() = default;

        // Type checking
        Type type() const { return type_; }
        bool is_leaf() const { return children_.empty(); }
        bool is_binary() const { return children_.size() == 2; }
        bool is_unary() const { return children_.size() == 1; }

        // Tree structure
        size_t num_children() const { return children_.size(); }
        const Expression &child(size_t i) const { return *children_[i]; }
        Expression &child(size_t i) { return *children_[i]; }
        void add_child(std::unique_ptr<Expression> child);
        void set_child(size_t i, std::unique_ptr<Expression> child);

        // Property management
        void set_property(const std::string &key, const std::string &value);
        std::string get_property(const std::string &key) const;
        bool has_property(const std::string &key) const;

        // Virtual methods
        virtual std::string to_string() const = 0;
        virtual std::unique_ptr<Expression> clone() const = 0;
        virtual std::unique_ptr<Expression> derivative(const Symbol &var) const;
        virtual bool equals(const Expression &other) const;
        virtual std::size_t hash() const;

        // Tree traversal
        void visit(std::function<void(const Expression &)> visitor) const;
        void visit_mut(std::function<void(Expression &)> visitor);

        // Find subexpressions
        std::vector<const Expression *> find(Type type) const;
        std::vector<Expression *> find_mut(Type type);

        // Comparison
        bool operator==(const Expression &other) const { return equals(other); }
        bool operator!=(const Expression &other) const { return !equals(other); }
    };

    /**
     * @brief Leaf node containing a symbol
     */
    class SymbolExpression : public Expression
    {
    private:
        std::unique_ptr<Symbol> symbol_;

    public:
        SymbolExpression(const Symbol &symbol);
        SymbolExpression(std::unique_ptr<Symbol> symbol);

        const Symbol &symbol() const { return *symbol_; }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        std::unique_ptr<Expression> derivative(const Symbol &var) const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;
    };

    /**
     * @brief Leaf node containing a tensor
     */
    class TensorExpression : public Expression
    {
    private:
        std::unique_ptr<Tensor> tensor_;

    public:
        TensorExpression(const Tensor &tensor);
        TensorExpression(std::unique_ptr<Tensor> tensor);

        const Tensor &tensor() const { return *tensor_; }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;
    };

    /**
     * @brief Leaf node containing an operator
     */
    class OperatorExpression : public Expression
    {
    private:
        std::unique_ptr<Operator> operator_;

    public:
        OperatorExpression(const Operator &op);
        OperatorExpression(std::unique_ptr<Operator> op);

        const Operator &operator_() const { return *operator_; }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;
    };

    /**
     * @brief Node containing an operator product
     */
    class OperatorProductExpression : public Expression
    {
    private:
        std::unique_ptr<OperatorProduct> product_;

    public:
        OperatorProductExpression(const OperatorProduct &product);
        OperatorProductExpression(std::unique_ptr<OperatorProduct> product);

        const OperatorProduct &product() const { return *product_; }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;
    };

    /**
     * @brief Binary operation expression (add, subtract, multiply, divide, power)
     */
    class BinaryOpExpression : public Expression
    {
    private:
        std::string operator_symbol_;

    public:
        BinaryOpExpression(Type type, std::unique_ptr<Expression> left,
                           std::unique_ptr<Expression> right);

        const Expression &left() const { return child(0); }
        const Expression &right() const { return child(1); }
        Expression &left() { return child(0); }
        Expression &right() { return child(1); }

        const std::string &operator_symbol() const { return operator_symbol_; }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        std::unique_ptr<Expression> derivative(const Symbol &var) const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;

    private:
        void set_operator_symbol();
    };

    /**
     * @brief Commutator expression [A, B] = AB - BA
     */
    class CommutatorExpression : public Expression
    {
    public:
        CommutatorExpression(std::unique_ptr<Expression> A, std::unique_ptr<Expression> B);

        const Expression &A() const { return child(0); }
        const Expression &B() const { return child(1); }
        Expression &A() { return child(0); }
        Expression &B() { return child(1); }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;

        // Expand to AB - BA
        std::unique_ptr<Expression> expand() const;
    };

    /**
     * @brief Anticommutator expression {A, B} = AB + BA
     */
    class AnticommutatorExpression : public Expression
    {
    public:
        AnticommutatorExpression(std::unique_ptr<Expression> A, std::unique_ptr<Expression> B);

        const Expression &A() const { return child(0); }
        const Expression &B() const { return child(1); }
        Expression &A() { return child(0); }
        Expression &B() { return child(1); }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;

        // Expand to AB + BA
        std::unique_ptr<Expression> expand() const;
    };

    /**
     * @brief Sum expression (n-ary addition)
     */
    class SumExpression : public Expression
    {
    private:
        std::vector<double> coefficients_;

    public:
        SumExpression();
        SumExpression(const std::vector<std::unique_ptr<Expression>> &terms,
                      const std::vector<double> &coefficients = {});

        void add_term(std::unique_ptr<Expression> term, double coefficient = 1.0);
        void set_coefficient(size_t i, double coeff);
        double coefficient(size_t i) const;

        size_t num_terms() const { return children_.size(); }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        std::unique_ptr<Expression> derivative(const Symbol &var) const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;
    };

    /**
     * @brief Tensor contraction expression
     */
    class ContractionExpression : public Expression
    {
    private:
        IndexSet contracted_indices_;

    public:
        ContractionExpression(std::unique_ptr<Expression> A, std::unique_ptr<Expression> B,
                              const IndexSet &contracted_indices);

        const Expression &A() const { return child(0); }
        const Expression &B() const { return child(1); }
        const IndexSet &contracted_indices() const { return contracted_indices_; }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;
    };

    /**
     * @brief Index summation expression
     */
    class IndexSumExpression : public Expression
    {
    private:
        Index sum_index_;

    public:
        IndexSumExpression(std::unique_ptr<Expression> expr, const Index &sum_index);

        const Expression &expression() const { return child(0); }
        const Index &sum_index() const { return sum_index_; }

        std::string to_string() const override;
        std::unique_ptr<Expression> clone() const override;
        bool equals(const Expression &other) const override;
        std::size_t hash() const override;
    };

    /**
     * @brief Factory functions for creating expressions
     */
    namespace ExpressionFactory
    {
        // Leaf expressions
        std::unique_ptr<Expression> symbol(const Symbol &sym);
        std::unique_ptr<Expression> tensor(const Tensor &tensor);
        std::unique_ptr<Expression> operator_(const Operator &op);
        std::unique_ptr<Expression> operator_product(const OperatorProduct &product);

        // Binary operations
        std::unique_ptr<Expression> add(std::unique_ptr<Expression> left,
                                        std::unique_ptr<Expression> right);
        std::unique_ptr<Expression> subtract(std::unique_ptr<Expression> left,
                                             std::unique_ptr<Expression> right);
        std::unique_ptr<Expression> multiply(std::unique_ptr<Expression> left,
                                             std::unique_ptr<Expression> right);
        std::unique_ptr<Expression> divide(std::unique_ptr<Expression> left,
                                           std::unique_ptr<Expression> right);
        std::unique_ptr<Expression> power(std::unique_ptr<Expression> base,
                                          std::unique_ptr<Expression> exponent);

        // Quantum operations
        std::unique_ptr<Expression> commutator(std::unique_ptr<Expression> A,
                                               std::unique_ptr<Expression> B);
        std::unique_ptr<Expression> anticommutator(std::unique_ptr<Expression> A,
                                                   std::unique_ptr<Expression> B);
        std::unique_ptr<Expression> contract(std::unique_ptr<Expression> A,
                                             std::unique_ptr<Expression> B,
                                             const IndexSet &indices);

        // Aggregate operations
        std::unique_ptr<Expression> sum(const std::vector<std::unique_ptr<Expression>> &terms);
        std::unique_ptr<Expression> index_sum(std::unique_ptr<Expression> expr,
                                              const Index &sum_index);

        // Constants
        std::unique_ptr<Expression> zero();
        std::unique_ptr<Expression> one();
        std::unique_ptr<Expression> constant(double value);
    }

} // namespace qc