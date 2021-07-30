#pragma once
#include <pluginplay/hasher.hpp>
#include <tuple> // for std::tie

namespace libchemist {

struct SlaterTypeGeminal {
    using coefficient_type = double;
    using exponent_type    = double;

    explicit SlaterTypeGeminal(exponent_type exp  = 1.2,
                               coefficient_type c = -1.0 / 1.2);

    SlaterTypeGeminal& operator*=(const SlaterTypeGeminal& rhs);
    SlaterTypeGeminal operator*(const SlaterTypeGeminal& rhs) const;

    void hash(pluginplay::Hasher& h) const { h(coefficient, exponent); }

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

inline bool operator==(const SlaterTypeGeminal& lhs,
                       const SlaterTypeGeminal& rhs) {
    return std::tie(lhs.coefficient, lhs.exponent) ==
           std::tie(rhs.coefficient, rhs.exponent);
}

inline bool operator!=(const SlaterTypeGeminal& lhs,
                       const SlaterTypeGeminal& rhs) {
    return !(lhs == rhs);
}
} // namespace libchemist
