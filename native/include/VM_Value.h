#pragma once

#include <string>
#include <variant>
#include <memory>
#include <cmath>

namespace GM {

/**
 * GML Value - Dynamically typed value that can hold any GML data type
 * Supports: real (double), string, bool, undefined
 */
class Value {
public:
    enum class Type {
        UNDEFINED,
        REAL,
        STRING,
        BOOL
    };

    // Constructors
    Value();
    Value(double real);
    Value(const std::string& str);
    Value(bool b);
    Value(const char* str);
    
    // Type checking
    Type GetType() const { return type_; }
    bool IsReal() const { return type_ == Type::REAL; }
    bool IsString() const { return type_ == Type::STRING; }
    bool IsBool() const { return type_ == Type::BOOL; }
    bool IsUndefined() const { return type_ == Type::UNDEFINED; }

    // Conversions
    double AsReal() const;
    std::string AsString() const;
    bool AsBool() const;

    // Operators
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;
    
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;
    bool operator<(const Value& other) const;
    bool operator<=(const Value& other) const;
    bool operator>(const Value& other) const;
    bool operator>=(const Value& other) const;

    Value operator&(const Value& other) const;  // Bitwise AND
    Value operator|(const Value& other) const;  // Bitwise OR
    Value operator^(const Value& other) const;  // Bitwise XOR
    Value operator~() const;                     // Bitwise NOT
    Value operator<<(const Value& other) const; // Left shift
    Value operator>>(const Value& other) const; // Right shift

    // Unary operations
    Value operator-() const;  // Negation
    Value operator!() const;  // Logical NOT

    // String representation
    std::string ToString() const;

private:
    Type type_;
    std::variant<double, std::string, bool> data_;
};

} // namespace GM
