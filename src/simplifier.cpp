#include "simplification/simplifier.h"
#include "core/expression.h"
#include <algorithm>
#include <iostream>

namespace qc {

// Simplifier implementation
Simplifier::Simplifier(bool enable_trace) : enable_trace_(enable_trace) {
    initialize_default_rules();
}

std::unique_ptr<Expression> Simplifier::simplify(const Expression& expr) const {
    auto result = expr.clone();
    
    // Apply simplification rules in order
    std::vector<RuleType> rule_order = {
        RuleType::ALGEBRAIC,
        RuleType::DISTRIBUTIVE,
        RuleType::COMMUTATOR,
        RuleType::TENSOR,
        RuleType::OPERATOR,
        RuleType::SYMMETRY
    };
    
    bool changed = true;
    int iterations = 0;
    const int max_iterations = 10;
    
    while (changed && iterations < max_iterations) {
        changed = false;
        auto old_result = result->to_string();
        
        for (auto rule_type : rule_order) {
            auto it = rules_.find(rule_type);
            if (it != rules_.end()) {
                auto new_expr = apply_rules(*result, it->second);
                if (new_expr && new_expr->to_string() != result->to_string()) {
                    result = std::move(new_expr);
                    changed = true;
                    log_trace("Applied " + std::to_string(static_cast<int>(rule_type)) + 
                             " rule: " + old_result + " -> " + result->to_string());
                }
            }
        }
        iterations++;
    }
    
    return result;
}

std::unique_ptr<Expression> Simplifier::apply_distributive_rules(const Expression& expr) const {
    auto it = rules_.find(RuleType::DISTRIBUTIVE);
    if (it != rules_.end()) {
        return apply_rules(expr, it->second);
    }
    return expr.clone();
}

std::unique_ptr<Expression> Simplifier::apply_rules(const Expression& expr, 
                                                   const std::vector<Rule>& rules) const {
    for (const auto& rule : rules) {
        auto result = rule(expr);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

void Simplifier::log_trace(const std::string& message) const {
    if (enable_trace_) {
        trace_log_.push_back(message);
        if (trace_log_.size() > 1000) { // Limit trace size
            trace_log_.erase(trace_log_.begin(), trace_log_.begin() + 100);
        }
    }
}

void Simplifier::initialize_default_rules() {
    // Add distributive rules
    rules_[RuleType::DISTRIBUTIVE].push_back(&DistributiveRules::distribute_multiplication);
    rules_[RuleType::DISTRIBUTIVE].push_back(&DistributiveRules::factor_common_terms);
    rules_[RuleType::DISTRIBUTIVE].push_back(&DistributiveRules::distribute_over_subtraction);
    
    // Add algebraic rules
    rules_[RuleType::ALGEBRAIC].push_back(&AlgebraicRules::identity_addition);
    rules_[RuleType::ALGEBRAIC].push_back(&AlgebraicRules::identity_multiplication);
    rules_[RuleType::ALGEBRAIC].push_back(&AlgebraicRules::zero_multiplication);
    rules_[RuleType::ALGEBRAIC].push_back(&AlgebraicRules::combine_constants);
    
    // Add commutator rules
    rules_[RuleType::COMMUTATOR].push_back(&CommutatorRules::antisymmetry);
    rules_[RuleType::COMMUTATOR].push_back(&CommutatorRules::zero_commutator);
    rules_[RuleType::COMMUTATOR].push_back(&CommutatorRules::expand_commutator);
}

// DistributiveRules implementation
std::unique_ptr<Expression> DistributiveRules::distribute_multiplication(const Expression& expr) {
    // Pattern: (a+b)*(c+d) = ac + ad + bc + bd
    if (expr.type() != Expression::Type::MULTIPLY || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& left = expr.child(0);
    const auto& right = expr.child(1);
    
    // Check if left side is a sum
    if (left.type() == Expression::Type::ADD || left.type() == Expression::Type::SUM) {
        // Check if right side is a sum
        if (right.type() == Expression::Type::ADD || right.type() == Expression::Type::SUM) {
            // Both sides are sums - distribute fully
            auto result = std::make_unique<SumExpression>();
            
            // For binary addition, treat as 2-term sum
            if (left.type() == Expression::Type::ADD && right.type() == Expression::Type::ADD) {
                // (a+b)*(c+d) = ac + ad + bc + bd
                auto ac = ExpressionFactory::multiply(left.child(0).clone(), right.child(0).clone());
                auto ad = ExpressionFactory::multiply(left.child(0).clone(), right.child(1).clone());
                auto bc = ExpressionFactory::multiply(left.child(1).clone(), right.child(0).clone());
                auto bd = ExpressionFactory::multiply(left.child(1).clone(), right.child(1).clone());
                
                result->add_term(std::move(ac));
                result->add_term(std::move(ad));
                result->add_term(std::move(bc));
                result->add_term(std::move(bd));
                
                return std::move(result);
            }
        } else {
            // Only left side is a sum: (a+b)*c = ac + bc
            if (left.type() == Expression::Type::ADD) {
                auto ac = ExpressionFactory::multiply(left.child(0).clone(), right.clone());
                auto bc = ExpressionFactory::multiply(left.child(1).clone(), right.clone());
                return ExpressionFactory::add(std::move(ac), std::move(bc));
            }
        }
    } else if (right.type() == Expression::Type::ADD || right.type() == Expression::Type::SUM) {
        // Only right side is a sum: a*(b+c) = ab + ac
        if (right.type() == Expression::Type::ADD) {
            auto ab = ExpressionFactory::multiply(left.clone(), right.child(0).clone());
            auto ac = ExpressionFactory::multiply(left.clone(), right.child(1).clone());
            return ExpressionFactory::add(std::move(ab), std::move(ac));
        }
    }
    
    return nullptr;
}

std::unique_ptr<Expression> DistributiveRules::factor_common_terms(const Expression& expr) {
    // Pattern: ax + bx = (a+b)*x
    if (expr.type() != Expression::Type::ADD || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& left = expr.child(0);
    const auto& right = expr.child(1);
    
    // Look for common factors in multiplication terms
    if (left.type() == Expression::Type::MULTIPLY && right.type() == Expression::Type::MULTIPLY) {
        if (left.is_binary() && right.is_binary()) {
            // Check if they share a common factor
            if (left.child(1).equals(right.child(1))) {
                // ax + bx = (a+b)*x
                auto sum = ExpressionFactory::add(left.child(0).clone(), right.child(0).clone());
                return ExpressionFactory::multiply(std::move(sum), left.child(1).clone());
            } else if (left.child(0).equals(right.child(0))) {
                // xa + xb = x*(a+b)
                auto sum = ExpressionFactory::add(left.child(1).clone(), right.child(1).clone());
                return ExpressionFactory::multiply(left.child(0).clone(), std::move(sum));
            }
        }
    }
    
    return nullptr;
}

std::unique_ptr<Expression> DistributiveRules::distribute_over_subtraction(const Expression& expr) {
    // Pattern: a*(b-c) = ab - ac
    if (expr.type() != Expression::Type::MULTIPLY || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& left = expr.child(0);
    const auto& right = expr.child(1);
    
    if (right.type() == Expression::Type::SUBTRACT && right.is_binary()) {
        // a*(b-c) = ab - ac
        auto ab = ExpressionFactory::multiply(left.clone(), right.child(0).clone());
        auto ac = ExpressionFactory::multiply(left.clone(), right.child(1).clone());
        return ExpressionFactory::subtract(std::move(ab), std::move(ac));
    } else if (left.type() == Expression::Type::SUBTRACT && left.is_binary()) {
        // (a-b)*c = ac - bc
        auto ac = ExpressionFactory::multiply(left.child(0).clone(), right.clone());
        auto bc = ExpressionFactory::multiply(left.child(1).clone(), right.clone());
        return ExpressionFactory::subtract(std::move(ac), std::move(bc));
    }
    
    return nullptr;
}

// AlgebraicRules implementation
std::unique_ptr<Expression> AlgebraicRules::identity_addition(const Expression& expr) {
    // x + 0 = x
    if (expr.type() != Expression::Type::ADD || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& left = expr.child(0);
    const auto& right = expr.child(1);
    
    // Check if either operand is zero
    if (left.type() == Expression::Type::SYMBOL) {
        auto* sym_expr = dynamic_cast<const SymbolExpression*>(&left);
        if (sym_expr && sym_expr->symbol().is_scalar()) {
            auto* scalar = dynamic_cast<const ScalarSymbol*>(&sym_expr->symbol());
            if (scalar && scalar->value() == 0.0) {
                return right.clone();
            }
        }
    }
    
    if (right.type() == Expression::Type::SYMBOL) {
        auto* sym_expr = dynamic_cast<const SymbolExpression*>(&right);
        if (sym_expr && sym_expr->symbol().is_scalar()) {
            auto* scalar = dynamic_cast<const ScalarSymbol*>(&sym_expr->symbol());
            if (scalar && scalar->value() == 0.0) {
                return left.clone();
            }
        }
    }
    
    return nullptr;
}

std::unique_ptr<Expression> AlgebraicRules::identity_multiplication(const Expression& expr) {
    // x * 1 = x
    if (expr.type() != Expression::Type::MULTIPLY || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& left = expr.child(0);
    const auto& right = expr.child(1);
    
    // Check if either operand is one
    if (left.type() == Expression::Type::SYMBOL) {
        auto* sym_expr = dynamic_cast<const SymbolExpression*>(&left);
        if (sym_expr && sym_expr->symbol().is_scalar()) {
            auto* scalar = dynamic_cast<const ScalarSymbol*>(&sym_expr->symbol());
            if (scalar && scalar->value() == 1.0) {
                return right.clone();
            }
        }
    }
    
    if (right.type() == Expression::Type::SYMBOL) {
        auto* sym_expr = dynamic_cast<const SymbolExpression*>(&right);
        if (sym_expr && sym_expr->symbol().is_scalar()) {
            auto* scalar = dynamic_cast<const ScalarSymbol*>(&sym_expr->symbol());
            if (scalar && scalar->value() == 1.0) {
                return left.clone();
            }
        }
    }
    
    return nullptr;
}

std::unique_ptr<Expression> AlgebraicRules::zero_multiplication(const Expression& expr) {
    // x * 0 = 0
    if (expr.type() != Expression::Type::MULTIPLY || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& left = expr.child(0);
    const auto& right = expr.child(1);
    
    // Check if either operand is zero
    auto check_zero = [](const Expression& e) -> bool {
        if (e.type() == Expression::Type::SYMBOL) {
            auto* sym_expr = dynamic_cast<const SymbolExpression*>(&e);
            if (sym_expr && sym_expr->symbol().is_scalar()) {
                auto* scalar = dynamic_cast<const ScalarSymbol*>(&sym_expr->symbol());
                return scalar && scalar->value() == 0.0;
            }
        }
        return false;
    };
    
    if (check_zero(left) || check_zero(right)) {
        return ExpressionFactory::zero();
    }
    
    return nullptr;
}

std::unique_ptr<Expression> AlgebraicRules::combine_constants(const Expression& expr) {
    // Combine constant terms in addition/multiplication
    if (expr.type() == Expression::Type::ADD && expr.is_binary()) {
        const auto& left = expr.child(0);
        const auto& right = expr.child(1);
        
        auto get_scalar_value = [](const Expression& e) -> std::pair<bool, double> {
            if (e.type() == Expression::Type::SYMBOL) {
                auto* sym_expr = dynamic_cast<const SymbolExpression*>(&e);
                if (sym_expr && sym_expr->symbol().is_scalar()) {
                    auto* scalar = dynamic_cast<const ScalarSymbol*>(&sym_expr->symbol());
                    if (scalar) {
                        return {true, scalar->value()};
                    }
                }
            }
            return {false, 0.0};
        };
        
        auto left_val = get_scalar_value(left);
        auto right_val = get_scalar_value(right);
        
        if (left_val.first && right_val.first) {
            // Both are scalars, combine them
            return ExpressionFactory::constant(left_val.second + right_val.second);
        }
    } else if (expr.type() == Expression::Type::MULTIPLY && expr.is_binary()) {
        const auto& left = expr.child(0);
        const auto& right = expr.child(1);
        
        auto get_scalar_value = [](const Expression& e) -> std::pair<bool, double> {
            if (e.type() == Expression::Type::SYMBOL) {
                auto* sym_expr = dynamic_cast<const SymbolExpression*>(&e);
                if (sym_expr && sym_expr->symbol().is_scalar()) {
                    auto* scalar = dynamic_cast<const ScalarSymbol*>(&sym_expr->symbol());
                    if (scalar) {
                        return {true, scalar->value()};
                    }
                }
            }
            return {false, 0.0};
        };
        
        auto left_val = get_scalar_value(left);
        auto right_val = get_scalar_value(right);
        
        if (left_val.first && right_val.first) {
            // Both are scalars, combine them
            return ExpressionFactory::constant(left_val.second * right_val.second);
        }
    }
    
    return nullptr;
}

// CommutatorRules implementation
std::unique_ptr<Expression> CommutatorRules::antisymmetry(const Expression& expr) {
    // [A,B] = -[B,A]
    if (expr.type() != Expression::Type::COMMUTATOR || !expr.is_binary()) {
        return nullptr;
    }
    
    // This rule is more for checking/validation - doesn't typically transform
    return nullptr;
}

std::unique_ptr<Expression> CommutatorRules::zero_commutator(const Expression& expr) {
    // [A,A] = 0
    if (expr.type() != Expression::Type::COMMUTATOR || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& A = expr.child(0);
    const auto& B = expr.child(1);
    
    if (A.equals(B)) {
        return ExpressionFactory::zero();
    }
    
    return nullptr;
}

std::unique_ptr<Expression> CommutatorRules::expand_commutator(const Expression& expr) {
    // [A,B] → AB - BA
    if (expr.type() != Expression::Type::COMMUTATOR || !expr.is_binary()) {
        return nullptr;
    }
    
    const auto& A = expr.child(0);
    const auto& B = expr.child(1);
    
    auto AB = ExpressionFactory::multiply(A.clone(), B.clone());
    auto BA = ExpressionFactory::multiply(B.clone(), A.clone());
    
    return ExpressionFactory::subtract(std::move(AB), std::move(BA));
}

} // namespace qc 