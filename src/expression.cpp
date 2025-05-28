#include "core/expression.h"
#include <sstream>
#include <functional>

namespace qc {

// Expression base class implementation
void Expression::add_child(std::unique_ptr<Expression> child) {
    children_.push_back(std::move(child));
}

void Expression::set_child(size_t i, std::unique_ptr<Expression> child) {
    if (i < children_.size()) {
        children_[i] = std::move(child);
    }
}

void Expression::set_property(const std::string& key, const std::string& value) {
    properties_[key] = value;
}

std::string Expression::get_property(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : "";
}

bool Expression::has_property(const std::string& key) const {
    return properties_.find(key) != properties_.end();
}

std::unique_ptr<Expression> Expression::derivative(const Symbol& var) const {
    // Default implementation returns zero
    return ExpressionFactory::zero();
}

bool Expression::equals(const Expression& other) const {
    return type_ == other.type_ && hash() == other.hash();
}

std::size_t Expression::hash() const {
    std::size_t seed = std::hash<int>{}(static_cast<int>(type_));
    for (const auto& child : children_) {
        seed ^= child->hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

// SymbolExpression implementation
SymbolExpression::SymbolExpression(const Symbol& symbol) 
    : Expression(Type::SYMBOL), symbol_(symbol.clone()) {}

SymbolExpression::SymbolExpression(std::unique_ptr<Symbol> symbol)
    : Expression(Type::SYMBOL), symbol_(std::move(symbol)) {}

std::string SymbolExpression::to_string() const {
    return symbol_->to_string();
}

std::unique_ptr<Expression> SymbolExpression::clone() const {
    return std::make_unique<SymbolExpression>(symbol_->clone());
}

std::unique_ptr<Expression> SymbolExpression::derivative(const Symbol& var) const {
    if (*symbol_ == var) {
        return ExpressionFactory::one();
    }
    return ExpressionFactory::zero();
}

bool SymbolExpression::equals(const Expression& other) const {
    if (other.type() != Type::SYMBOL) return false;
    auto* other_sym = dynamic_cast<const SymbolExpression*>(&other);
    return other_sym && *symbol_ == other_sym->symbol();
}

std::size_t SymbolExpression::hash() const {
    return symbol_->hash();
}

// BinaryOpExpression implementation
BinaryOpExpression::BinaryOpExpression(Type type, std::unique_ptr<Expression> left, 
                                     std::unique_ptr<Expression> right)
    : Expression(type) {
    add_child(std::move(left));
    add_child(std::move(right));
    set_operator_symbol();
}

void BinaryOpExpression::set_operator_symbol() {
    switch (type_) {
        case Type::ADD: operator_symbol_ = "+"; break;
        case Type::SUBTRACT: operator_symbol_ = "-"; break;
        case Type::MULTIPLY: operator_symbol_ = "*"; break;
        case Type::DIVIDE: operator_symbol_ = "/"; break;
        case Type::POWER: operator_symbol_ = "^"; break;
        default: operator_symbol_ = "?"; break;
    }
}

std::string BinaryOpExpression::to_string() const {
    std::ostringstream oss;
    bool need_parens_left = left().type() == Type::ADD || left().type() == Type::SUBTRACT;
    bool need_parens_right = right().type() == Type::ADD || right().type() == Type::SUBTRACT;
    
    if (need_parens_left) oss << "(";
    oss << left().to_string();
    if (need_parens_left) oss << ")";
    
    oss << " " << operator_symbol_ << " ";
    
    if (need_parens_right) oss << "(";
    oss << right().to_string();
    if (need_parens_right) oss << ")";
    
    return oss.str();
}

std::unique_ptr<Expression> BinaryOpExpression::clone() const {
    return std::make_unique<BinaryOpExpression>(type_, left().clone(), right().clone());
}

std::unique_ptr<Expression> BinaryOpExpression::derivative(const Symbol& var) const {
    switch (type_) {
        case Type::ADD:
        case Type::SUBTRACT: {
            auto left_deriv = left().derivative(var);
            auto right_deriv = right().derivative(var);
            if (type_ == Type::ADD) {
                return ExpressionFactory::add(std::move(left_deriv), std::move(right_deriv));
            } else {
                return ExpressionFactory::subtract(std::move(left_deriv), std::move(right_deriv));
            }
        }
        case Type::MULTIPLY: {
            // Product rule: (fg)' = f'g + fg'
            auto f_prime_g = ExpressionFactory::multiply(left().derivative(var), right().clone());
            auto f_g_prime = ExpressionFactory::multiply(left().clone(), right().derivative(var));
            return ExpressionFactory::add(std::move(f_prime_g), std::move(f_g_prime));
        }
        default:
            return ExpressionFactory::zero();
    }
}

bool BinaryOpExpression::equals(const Expression& other) const {
    if (other.type() != type_) return false;
    auto* other_bin = dynamic_cast<const BinaryOpExpression*>(&other);
    return other_bin && left().equals(other_bin->left()) && right().equals(other_bin->right());
}

std::size_t BinaryOpExpression::hash() const {
    std::size_t seed = std::hash<int>{}(static_cast<int>(type_));
    seed ^= left().hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= right().hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

// CommutatorExpression implementation
CommutatorExpression::CommutatorExpression(std::unique_ptr<Expression> A, std::unique_ptr<Expression> B)
    : Expression(Type::COMMUTATOR) {
    add_child(std::move(A));
    add_child(std::move(B));
}

std::string CommutatorExpression::to_string() const {
    return "[" + A().to_string() + ", " + B().to_string() + "]";
}

std::unique_ptr<Expression> CommutatorExpression::clone() const {
    return std::make_unique<CommutatorExpression>(A().clone(), B().clone());
}

bool CommutatorExpression::equals(const Expression& other) const {
    if (other.type() != Type::COMMUTATOR) return false;
    auto* other_comm = dynamic_cast<const CommutatorExpression*>(&other);
    return other_comm && A().equals(other_comm->A()) && B().equals(other_comm->B());
}

std::size_t CommutatorExpression::hash() const {
    std::size_t seed = std::hash<int>{}(static_cast<int>(type_));
    seed ^= A().hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= B().hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

std::unique_ptr<Expression> CommutatorExpression::expand() const {
    // [A,B] = AB - BA
    auto AB = ExpressionFactory::multiply(A().clone(), B().clone());
    auto BA = ExpressionFactory::multiply(B().clone(), A().clone());
    return ExpressionFactory::subtract(std::move(AB), std::move(BA));
}

// SumExpression implementation
SumExpression::SumExpression() : Expression(Type::SUM) {}

SumExpression::SumExpression(const std::vector<std::unique_ptr<Expression>>& terms,
                           const std::vector<double>& coefficients) 
    : Expression(Type::SUM) {
    for (size_t i = 0; i < terms.size(); ++i) {
        add_child(terms[i]->clone());
        coefficients_.push_back(i < coefficients.size() ? coefficients[i] : 1.0);
    }
}

void SumExpression::add_term(std::unique_ptr<Expression> term, double coefficient) {
    add_child(std::move(term));
    coefficients_.push_back(coefficient);
}

void SumExpression::set_coefficient(size_t i, double coeff) {
    if (i < coefficients_.size()) {
        coefficients_[i] = coeff;
    }
}

double SumExpression::coefficient(size_t i) const {
    return i < coefficients_.size() ? coefficients_[i] : 1.0;
}

std::string SumExpression::to_string() const {
    if (children_.empty()) return "0";
    
    std::ostringstream oss;
    for (size_t i = 0; i < children_.size(); ++i) {
        if (i > 0) {
            oss << " + ";
        }
        double coeff = coefficient(i);
        if (coeff != 1.0) {
            oss << coeff << "*";
        }
        oss << children_[i]->to_string();
    }
    return oss.str();
}

std::unique_ptr<Expression> SumExpression::clone() const {
    auto result = std::make_unique<SumExpression>();
    for (size_t i = 0; i < children_.size(); ++i) {
        result->add_term(children_[i]->clone(), coefficient(i));
    }
    return std::move(result);
}

std::unique_ptr<Expression> SumExpression::derivative(const Symbol& var) const {
    auto result = std::make_unique<SumExpression>();
    for (size_t i = 0; i < children_.size(); ++i) {
        auto term_deriv = children_[i]->derivative(var);
        result->add_term(std::move(term_deriv), coefficient(i));
    }
    return std::move(result);
}

bool SumExpression::equals(const Expression& other) const {
    if (other.type() != Type::SUM) return false;
    auto* other_sum = dynamic_cast<const SumExpression*>(&other);
    if (!other_sum || other_sum->children_.size() != children_.size()) return false;
    
    for (size_t i = 0; i < children_.size(); ++i) {
        if (!children_[i]->equals(*other_sum->children_[i]) || 
            coefficient(i) != other_sum->coefficient(i)) {
            return false;
        }
    }
    return true;
}

std::size_t SumExpression::hash() const {
    std::size_t seed = std::hash<int>{}(static_cast<int>(type_));
    for (size_t i = 0; i < children_.size(); ++i) {
        seed ^= children_[i]->hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<double>{}(coefficient(i)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

// ExpressionFactory implementation
namespace ExpressionFactory {

std::unique_ptr<Expression> symbol(const Symbol& sym) {
    return std::make_unique<SymbolExpression>(sym);
}

std::unique_ptr<Expression> add(std::unique_ptr<Expression> left, 
                               std::unique_ptr<Expression> right) {
    return std::make_unique<BinaryOpExpression>(Expression::Type::ADD, 
                                               std::move(left), std::move(right));
}

std::unique_ptr<Expression> subtract(std::unique_ptr<Expression> left,
                                    std::unique_ptr<Expression> right) {
    return std::make_unique<BinaryOpExpression>(Expression::Type::SUBTRACT,
                                               std::move(left), std::move(right));
}

std::unique_ptr<Expression> multiply(std::unique_ptr<Expression> left,
                                    std::unique_ptr<Expression> right) {
    return std::make_unique<BinaryOpExpression>(Expression::Type::MULTIPLY,
                                               std::move(left), std::move(right));
}

std::unique_ptr<Expression> divide(std::unique_ptr<Expression> left,
                                  std::unique_ptr<Expression> right) {
    return std::make_unique<BinaryOpExpression>(Expression::Type::DIVIDE,
                                               std::move(left), std::move(right));
}

std::unique_ptr<Expression> power(std::unique_ptr<Expression> base,
                                 std::unique_ptr<Expression> exponent) {
    return std::make_unique<BinaryOpExpression>(Expression::Type::POWER,
                                               std::move(base), std::move(exponent));
}

std::unique_ptr<Expression> commutator(std::unique_ptr<Expression> A,
                                      std::unique_ptr<Expression> B) {
    return std::make_unique<CommutatorExpression>(std::move(A), std::move(B));
}

std::unique_ptr<Expression> sum(const std::vector<std::unique_ptr<Expression>>& terms) {
    auto result = std::make_unique<SumExpression>();
    for (const auto& term : terms) {
        result->add_term(term->clone());
    }
    return std::move(result);
}

std::unique_ptr<Expression> zero() {
    return symbol(ScalarSymbol("0", 0.0));
}

std::unique_ptr<Expression> one() {
    return symbol(ScalarSymbol("1", 1.0));
}

std::unique_ptr<Expression> constant(double value) {
    return symbol(ScalarSymbol(std::to_string(value), value));
}

} // namespace ExpressionFactory

} // namespace qc 