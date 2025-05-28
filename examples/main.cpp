#include "qc_expression_tree.h"
#include <iostream>
#include <memory>

using namespace qc;

void demonstrate_basic_expressions() {
    std::cout << "=== Basic Expression Demonstrations ===" << std::endl;
    
    // Create basic symbols
    Symbol a("a");
    Symbol b("b");
    Symbol c("c");
    Symbol d("d");
    
    // Create symbol expressions
    auto expr_a = ExpressionFactory::symbol(a);
    auto expr_b = ExpressionFactory::symbol(b);
    auto expr_c = ExpressionFactory::symbol(c);
    auto expr_d = ExpressionFactory::symbol(d);
    
    std::cout << "Symbol a: " << expr_a->to_string() << std::endl;
    std::cout << "Symbol b: " << expr_b->to_string() << std::endl;
    
    // Create compound expressions
    auto sum_ab = ExpressionFactory::add(expr_a->clone(), expr_b->clone());
    auto sum_cd = ExpressionFactory::add(expr_c->clone(), expr_d->clone());
    
    std::cout << "Sum (a+b): " << sum_ab->to_string() << std::endl;
    std::cout << "Sum (c+d): " << sum_cd->to_string() << std::endl;
    
    // Create product (a+b)*(c+d) - this is what we'll expand
    auto product = ExpressionFactory::multiply(sum_ab->clone(), sum_cd->clone());
    std::cout << "Product (a+b)*(c+d): " << product->to_string() << std::endl;
    
    std::cout << std::endl;
}

void demonstrate_distributive_law() {
    std::cout << "=== Distributive Law Demonstration ===" << std::endl;
    
    // Create symbols
    Symbol a("a"), b("b"), c("c"), d("d");
    
    // Build expression (a+b)*(c+d)
    auto sum_ab = ExpressionFactory::add(
        ExpressionFactory::symbol(a), 
        ExpressionFactory::symbol(b)
    );
    auto sum_cd = ExpressionFactory::add(
        ExpressionFactory::symbol(c), 
        ExpressionFactory::symbol(d)
    );
    auto product = ExpressionFactory::multiply(sum_ab, sum_cd);
    
    std::cout << "Original expression: " << product->to_string() << std::endl;
    
    // Apply distributive rule manually to show the expansion
    // (a+b)*(c+d) = ac + ad + bc + bd
    auto ac = ExpressionFactory::multiply(
        ExpressionFactory::symbol(a), 
        ExpressionFactory::symbol(c)
    );
    auto ad = ExpressionFactory::multiply(
        ExpressionFactory::symbol(a), 
        ExpressionFactory::symbol(d)
    );
    auto bc = ExpressionFactory::multiply(
        ExpressionFactory::symbol(b), 
        ExpressionFactory::symbol(c)
    );
    auto bd = ExpressionFactory::multiply(
        ExpressionFactory::symbol(b), 
        ExpressionFactory::symbol(d)
    );
    
    // Create the expanded sum
    auto expanded = std::make_unique<SumExpression>();
    expanded->add_term(std::move(ac));
    expanded->add_term(std::move(ad));
    expanded->add_term(std::move(bc));
    expanded->add_term(std::move(bd));
    
    std::cout << "Manual expansion: " << expanded->to_string() << std::endl;
    
    // Now use the simplifier
    Simplifier simplifier(true); // Enable tracing
    auto simplified = simplifier.apply_distributive_rules(*product);
    
    if (simplified) {
        std::cout << "Simplified expression: " << simplified->to_string() << std::endl;
    } else {
        std::cout << "No simplification rules applied" << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrate_commutators() {
    std::cout << "=== Commutator Demonstrations ===" << std::endl;
    
    // Create operator symbols
    Symbol A("A"), B("B");
    
    // Create commutator [A,B]
    auto commutator_AB = ExpressionFactory::commutator(
        ExpressionFactory::symbol(A),
        ExpressionFactory::symbol(B)
    );
    
    std::cout << "Commutator: " << commutator_AB->to_string() << std::endl;
    
    // Expand commutator [A,B] = AB - BA
    auto* comm_expr = dynamic_cast<CommutatorExpression*>(commutator_AB.get());
    if (comm_expr) {
        auto expanded = comm_expr->expand();
        std::cout << "Expanded commutator: " << expanded->to_string() << std::endl;
    }
    
    // Create self-commutator [A,A] which should simplify to 0
    auto self_commutator = ExpressionFactory::commutator(
        ExpressionFactory::symbol(A),
        ExpressionFactory::symbol(A)
    );
    
    std::cout << "Self-commutator [A,A]: " << self_commutator->to_string() << std::endl;
    
    Simplifier simplifier;
    auto simplified_self = simplifier.apply_commutator_rules(*self_commutator);
    if (simplified_self) {
        std::cout << "Simplified [A,A]: " << simplified_self->to_string() << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrate_algebraic_simplification() {
    std::cout << "=== Algebraic Simplification Demonstrations ===" << std::endl;
    
    // Create symbols and constants
    Symbol x("x");
    ScalarSymbol zero("0", 0.0);
    ScalarSymbol one("1", 1.0);
    
    // Test x + 0 = x
    auto x_plus_zero = ExpressionFactory::add(
        ExpressionFactory::symbol(x),
        ExpressionFactory::symbol(zero)
    );
    std::cout << "x + 0: " << x_plus_zero->to_string() << std::endl;
    
    Simplifier simplifier;
    auto simplified = simplifier.apply_algebraic_rules(*x_plus_zero);
    if (simplified) {
        std::cout << "Simplified: " << simplified->to_string() << std::endl;
    }
    
    // Test x * 1 = x
    auto x_times_one = ExpressionFactory::multiply(
        ExpressionFactory::symbol(x),
        ExpressionFactory::symbol(one)
    );
    std::cout << "x * 1: " << x_times_one->to_string() << std::endl;
    
    simplified = simplifier.apply_algebraic_rules(*x_times_one);
    if (simplified) {
        std::cout << "Simplified: " << simplified->to_string() << std::endl;
    }
    
    // Test x * 0 = 0
    auto x_times_zero = ExpressionFactory::multiply(
        ExpressionFactory::symbol(x),
        ExpressionFactory::symbol(zero)
    );
    std::cout << "x * 0: " << x_times_zero->to_string() << std::endl;
    
    simplified = simplifier.apply_algebraic_rules(*x_times_zero);
    if (simplified) {
        std::cout << "Simplified: " << simplified->to_string() << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrate_full_simplification() {
    std::cout << "=== Full Simplification Example ===" << std::endl;
    
    // Create a more complex expression that requires multiple simplification steps
    Symbol x("x"), y("y");
    ScalarSymbol zero("0", 0.0);
    ScalarSymbol one("1", 1.0);
    
    // Build expression: (x + 0) * (y + 1*0) + 0
    auto x_plus_zero = ExpressionFactory::add(
        ExpressionFactory::symbol(x),
        ExpressionFactory::symbol(zero)
    );
    
    auto one_times_zero = ExpressionFactory::multiply(
        ExpressionFactory::symbol(one),
        ExpressionFactory::symbol(zero)
    );
    
    auto y_plus_one_times_zero = ExpressionFactory::add(
        ExpressionFactory::symbol(y),
        std::move(one_times_zero)
    );
    
    auto product = ExpressionFactory::multiply(
        std::move(x_plus_zero),
        std::move(y_plus_one_times_zero)
    );
    
    auto final_expr = ExpressionFactory::add(
        std::move(product),
        ExpressionFactory::symbol(zero)
    );
    
    std::cout << "Complex expression: " << final_expr->to_string() << std::endl;
    
    // Apply full simplification
    Simplifier simplifier(true); // Enable tracing
    auto simplified = simplifier.simplify(*final_expr);
    
    std::cout << "After simplification: " << simplified->to_string() << std::endl;
    
    // Print trace if available
    const auto& trace = simplifier.get_trace();
    if (!trace.empty()) {
        std::cout << "\nSimplification trace:" << std::endl;
        for (const auto& step : trace) {
            std::cout << "  " << step << std::endl;
        }
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "MetaWave Quantum Chemistry Expression Tree Demo" << std::endl;
    std::cout << "===============================================" << std::endl << std::endl;
    
    try {
        demonstrate_basic_expressions();
        demonstrate_distributive_law();
        demonstrate_commutators();
        demonstrate_algebraic_simplification();
        demonstrate_full_simplification();
        
        std::cout << "All demonstrations completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 