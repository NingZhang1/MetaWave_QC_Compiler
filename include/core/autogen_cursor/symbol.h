#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace qc
{

    /**
     * @brief Base class for all symbolic entities in quantum chemistry expressions
     */
    class Symbol
    {
    public:
        enum class Type
        {
            SCALAR,
            VARIABLE,
            CONSTANT,
            COMPLEX
        };

    private:
        std::string name_;
        Type type_;
        std::unordered_map<std::string, std::string> properties_;

    public:
        Symbol(const std::string &name, Type type = Type::VARIABLE);
        virtual ~Symbol() = default;

        // Basic accessors
        const std::string &name() const { return name_; }
        Type type() const { return type_; }

        // Property management
        void set_property(const std::string &key, const std::string &value);
        std::string get_property(const std::string &key) const;
        bool has_property(const std::string &key) const;

        // Comparison operators
        bool operator==(const Symbol &other) const;
        bool operator!=(const Symbol &other) const;
        bool operator<(const Symbol &other) const;

        // String representation
        virtual std::string to_string() const;

        // Hash support for use in containers
        std::size_t hash() const;

        // Clone method for deep copying
        virtual std::unique_ptr<Symbol> clone() const;

        // Type checking helpers
        bool is_scalar() const { return type_ == Type::SCALAR; }
        bool is_variable() const { return type_ == Type::VARIABLE; }
        bool is_constant() const { return type_ == Type::CONSTANT; }
        bool is_complex() const { return type_ == Type::COMPLEX; }
    };

    /**
     * @brief Specialized symbol for scalar values
     */
    class ScalarSymbol : public Symbol
    {
    private:
        double value_;

    public:
        ScalarSymbol(const std::string &name, double value);
        double value() const { return value_; }
        void set_value(double value) { value_ = value; }

        std::string to_string() const override;
        std::unique_ptr<Symbol> clone() const override;
    };

    /**
     * @brief Specialized symbol for complex numbers
     */
    class ComplexSymbol : public Symbol
    {
    private:
        double real_, imag_;

    public:
        ComplexSymbol(const std::string &name, double real = 0.0, double imag = 0.0);
        double real() const { return real_; }
        double imag() const { return imag_; }
        void set_real(double real) { real_ = real; }
        void set_imag(double imag) { imag_ = imag; }

        std::string to_string() const override;
        std::unique_ptr<Symbol> clone() const override;
    };

} // namespace qc

// Hash specialization for std::unordered_map usage
namespace std
{
    template <>
    struct hash<qc::Symbol>
    {
        std::size_t operator()(const qc::Symbol &s) const
        {
            return s.hash();
        }
    };
}