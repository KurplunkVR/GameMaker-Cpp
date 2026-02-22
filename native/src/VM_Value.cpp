#include "VM_Value.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace GM {

// Constructors
Value::Value() : type_(Type::UNDEFINED), data_(0.0) {}
Value::Value(double real) : type_(Type::REAL), data_(real) {}
Value::Value(const std::string& str) : type_(Type::STRING), data_(str) {}
Value::Value(bool b) : type_(Type::BOOL), data_(b) {}
Value::Value(const char* str) : type_(Type::STRING), data_(std::string(str)) {}

// Conversions
double Value::AsReal() const {
    switch (type_) {
        case Type::REAL:
            return std::get<double>(data_);
        case Type::STRING: {
            try {
                return std::stod(std::get<std::string>(data_));
            } catch (...) {
                return 0.0;
            }
        }
        case Type::BOOL:
            return std::get<bool>(data_) ? 1.0 : 0.0;
        case Type::UNDEFINED:
            return 0.0;
        default:
            return 0.0;
    }
}

std::string Value::AsString() const {
    switch (type_) {
        case Type::REAL: {
            double val = std::get<double>(data_);
            // Check if it's an integer
            if (val == std::floor(val)) {
                return std::to_string(static_cast<int64_t>(val));
            }
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << val;
            std::string str = oss.str();
            // Remove trailing zeros
            str.erase(str.find_last_not_of('0') + 1);
            if (str.back() == '.') str.pop_back();
            return str;
        }
        case Type::STRING:
            return std::get<std::string>(data_);
        case Type::BOOL:
            return std::get<bool>(data_) ? "true" : "false";
        case Type::UNDEFINED:
            return "undefined";
        default:
            return "";
    }
}

bool Value::AsBool() const {
    switch (type_) {
        case Type::REAL:
            return std::get<double>(data_) != 0.0;
        case Type::STRING:
            return !std::get<std::string>(data_).empty();
        case Type::BOOL:
            return std::get<bool>(data_);
        case Type::UNDEFINED:
            return false;
        default:
            return false;
    }
}

// Arithmetic operators
Value Value::operator+(const Value& other) const {
    return Value(AsReal() + other.AsReal());
}

Value Value::operator-(const Value& other) const {
    return Value(AsReal() - other.AsReal());
}

Value Value::operator*(const Value& other) const {
    return Value(AsReal() * other.AsReal());
}

Value Value::operator/(const Value& other) const {
    double divisor = other.AsReal();
    if (divisor == 0.0) {
        return Value(0.0);  // Division by zero returns 0
    }
    return Value(AsReal() / divisor);
}

Value Value::operator%(const Value& other) const {
    return Value(std::fmod(AsReal(), other.AsReal()));
}

Value Value::operator-() const {
    return Value(-AsReal());
}

// Comparison operators
bool Value::operator==(const Value& other) const {
    // String comparison
    if (IsString() && other.IsString()) {
        return std::get<std::string>(data_) == std::get<std::string>(other.data_);
    }
    // Numeric comparison
    return AsReal() == other.AsReal();
}

bool Value::operator!=(const Value& other) const {
    return !(*this == other);
}

bool Value::operator<(const Value& other) const {
    if (IsString() && other.IsString()) {
        return std::get<std::string>(data_) < std::get<std::string>(other.data_);
    }
    return AsReal() < other.AsReal();
}

bool Value::operator<=(const Value& other) const {
    return (*this < other) || (*this == other);
}

bool Value::operator>(const Value& other) const {
    return !(*this <= other);
}

bool Value::operator>=(const Value& other) const {
    return !(*this < other);
}

// Bitwise operators
Value Value::operator&(const Value& other) const {
    return Value(static_cast<double>(static_cast<int64_t>(AsReal()) & static_cast<int64_t>(other.AsReal())));
}

Value Value::operator|(const Value& other) const {
    return Value(static_cast<double>(static_cast<int64_t>(AsReal()) | static_cast<int64_t>(other.AsReal())));
}

Value Value::operator^(const Value& other) const {
    return Value(static_cast<double>(static_cast<int64_t>(AsReal()) ^ static_cast<int64_t>(other.AsReal())));
}

Value Value::operator~() const {
    return Value(static_cast<double>(~static_cast<int64_t>(AsReal())));
}

Value Value::operator<<(const Value& other) const {
    return Value(static_cast<double>(static_cast<int64_t>(AsReal()) << static_cast<int64_t>(other.AsReal())));
}

Value Value::operator>>(const Value& other) const {
    return Value(static_cast<double>(static_cast<int64_t>(AsReal()) >> static_cast<int64_t>(other.AsReal())));
}

// Logical NOT
Value Value::operator!() const {
    return Value(!AsBool());
}

// String representation
std::string Value::ToString() const {
    std::ostringstream oss;
    oss << "Value(" << (IsReal() ? "real" : IsString() ? "string" : IsBool() ? "bool" : "undefined") << ": " << AsString() << ")";
    return oss.str();
}

} // namespace GM
