#include "core/symbol.h"
#include <sstream>
#include <functional>

namespace qc
{

    // Symbol implementation
    Symbol::Symbol(const std::string &name, Type type)
        : name_(name), type_(type) {}

    void Symbol::set_property(const std::string &key, const std::string &value)
    {
        properties_[key] = value;
    }

    std::string Symbol::get_property(const std::string &key) const
    {
        auto it = properties_.find(key);
        return (it != properties_.end()) ? it->second : "";
    }

    bool Symbol::has_property(const std::string &key) const
    {
        return properties_.find(key) != properties_.end();
    }

    bool Symbol::operator==(const Symbol &other) const
    {
        return name_ == other.name_ && type_ == other.type_;
    }

    bool Symbol::operator!=(const Symbol &other) const
    {
        return !(*this == other);
    }

    bool Symbol::operator<(const Symbol &other) const
    {
        if (name_ != other.name_)
            return name_ < other.name_;
        return type_ < other.type_;
    }

    std::string Symbol::to_string() const
    {
        std::string result = name_;
        if (type_ == Type::COMPLEX)
        {
            result += "ℂ";
        }
        else if (type_ == Type::CONSTANT)
        {
            result += "ᶜ";
        }
        return result;
    }

    std::size_t Symbol::hash() const
    {
        std::size_t h1 = std::hash<std::string>{}(name_);
        std::size_t h2 = std::hash<int>{}(static_cast<int>(type_));
        return h1 ^ (h2 << 1);
    }

    std::unique_ptr<Symbol> Symbol::clone() const
    {
        auto clone = std::make_unique<Symbol>(name_, type_);
        clone->properties_ = properties_;
        return clone;
    }

    // ScalarSymbol implementation
    ScalarSymbol::ScalarSymbol(const std::string &name, double value)
        : Symbol(name, Type::SCALAR), value_(value) {}

    std::string ScalarSymbol::to_string() const
    {
        std::ostringstream oss;
        oss << name_ << "=" << value_;
        return oss.str();
    }

    std::unique_ptr<Symbol> ScalarSymbol::clone() const
    {
        auto clone = std::make_unique<ScalarSymbol>(name_, value_);
        clone->properties_ = properties_;
        return clone;
    }

    // ComplexSymbol implementation
    ComplexSymbol::ComplexSymbol(const std::string &name, double real, double imag)
        : Symbol(name, Type::COMPLEX), real_(real), imag_(imag) {}

    std::string ComplexSymbol::to_string() const
    {
        std::ostringstream oss;
        oss << name_ << "=" << real_;
        if (imag_ >= 0)
            oss << "+";
        oss << imag_ << "i";
        return oss.str();
    }

    std::unique_ptr<Symbol> ComplexSymbol::clone() const
    {
        auto clone = std::make_unique<ComplexSymbol>(name_, real_, imag_);
        clone->properties_ = properties_;
        return clone;
    }

} // namespace qc