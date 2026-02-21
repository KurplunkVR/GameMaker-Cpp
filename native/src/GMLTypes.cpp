#include "GMLTypes.h"
#include <sstream>
#include <cmath>

namespace GM {

double Variant::AsDouble() const {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stod(std::get<std::string>(value));
        } catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

int Variant::AsInt() const {
    return (int)AsDouble();
}

std::string Variant::AsString() const {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<double>(value)) {
        double d = std::get<double>(value);
        if (d == std::floor(d)) {
            return std::to_string((int)d);
        } else {
            std::ostringstream oss;
            oss << d;
            return oss.str();
        }
    }
    return "";
}

bool Variant::AsBool() const {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    }
    return false;
}

bool Variant::IsReal() const {
    return std::holds_alternative<double>(value);
}

bool Variant::IsString() const {
    return std::holds_alternative<std::string>(value);
}

bool Variant::IsUndefined() const {
    return std::holds_alternative<std::nullptr_t>(value);
}

} // namespace GM
