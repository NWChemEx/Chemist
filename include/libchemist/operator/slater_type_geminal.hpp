#pragma once
#include "libchemist/operator/electron.hpp"
#include <pluginplay/hasher.hpp>
#include <tuple> // for std::tie

namespace libchemist {

/** @brief Object describing a geminal, which is modeled by a Slater function.
 *
 *  Geminals are a single object used to describe the correlation of two
 *  particles. The SlaterTypeGeminal class models a geminal which uses a
 *  function of the form @f$\exp(-\gamma r_{12})@f$, where @f$\gamma@f$ is a
 *  constant.
 *
 *  @tparam Particle1 The type of the first particle.
 *  @tparam Particle2 The type of the second partilce.
 */
template<typename Particle1, typename Particle2>
struct SlaterTypeGeminal {
    using coefficient_type = double;
    using exponent_type    = double;

    explicit SlaterTypeGeminal(exponent_type exp  = 1.2,
                               coefficient_type c = -1.0 / 1.2);

    auto& operator*=(const SlaterTypeGeminal& rhs);
    auto operator*(const SlaterTypeGeminal& rhs) const;

    void hash(pluginplay::Hasher& h) const { h(coefficient, exponent); }

    double coefficient;
    double exponent;
};

#define SLATER_TYPE_GEMINAL SlaterTypeGeminal<Particle1, Particle2>

template<typename Particle1, typename Particle2>
SLATER_TYPE_GEMINAL::SlaterTypeGeminal(exponent_type exp, coefficient_type c) :
  exponent(exp), coefficient(c) {}

template<typename Particle1, typename Particle2>
auto& SLATER_TYPE_GEMINAL::operator*=(const SlaterTypeGeminal& rhs) {
    coefficient *= rhs.coefficient;
    exponent += rhs.exponent;
    return *this;
}

template<typename Particle1, typename Particle2>
auto SLATER_TYPE_GEMINAL::operator*(const SlaterTypeGeminal& rhs) const {
    return SlaterTypeGeminal(*this) *= rhs;
}

#undef SLATER_TYPE_GEMIAL

template<typename LHS1, typename LHS2, typename RHS1, typename RHS2>
bool operator==(const SlaterTypeGeminal<LHS1, LHS2>& lhs,
                const SlaterTypeGeminal<RHS1, RHS2>& rhs) {
    if constexpr(std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return std::tie(lhs.coefficient, lhs.exponent) ==
               std::tie(rhs.coefficient, rhs.exponent);
    } else {
        return false;
    }
}

template<typename LHS1, typename LHS2, typename RHS1, typename RHS2>
bool operator!=(const SlaterTypeGeminal<LHS1, LHS2>& lhs,
                const SlaterTypeGeminal<RHS1, RHS2>& rhs) {
    return !(lhs == rhs);
}

/// Type of a SlaterTypeGeminal correlating two electrons
using STG = SlaterTypeGeminal<Electron, Electron>;

} // namespace libchemist
