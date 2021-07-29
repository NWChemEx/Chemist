#pragma once

namespace libchemist {

struct SlaterTypeGeminal {
    using coefficient_type = double;
    using exponent_type    = double;

    explicit SlaterTypeGeminal(exponent_type exp  = 1.2,
                               coefficient_type c = -1.0 / exp);

    SlaterTypeGeminal& operator*=(const SlaterTypeGeminal& rhs);
    SlaterTypeGeminal operator*(const SlaterTypeGeminal& rhs) const;

    double coefficient;
    double exponent;
};

inline SlaterTypeGeminal::SlaterTypeGeminal(exponent_type exp,
                                            coefficient_type c) :
  exponent(exp), coefficient(c) {}

inline SlaterTypeGeminal& SlaterTypeGeminal::operator*=(
  const SlaterTypeGeminal& rhs) {
    coefficient *= rhs.coefficient;
    exponent += rhs.exponent;
    return *this;
}

inline SlaterTypeGeminal SlaterTypeGeminal::operator*(
  const SlaterTypeGeminal& rhs) const {
    return SlaterTypeGeminal(*this) *= rhs;
}

} // namespace libchemist
