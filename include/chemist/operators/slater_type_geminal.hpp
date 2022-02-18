#pragma once
#include "chemist/detail_/hashing.hpp"
#include "chemist/electrons/electrons.hpp"
#include <tuple> // for std::tie

namespace chemist::operators {

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
    /// Type of the scalar multiplying the geminal
    using coefficient_type = double;

    /// Type of the Slater function's exponent
    using exponent_type = double;

    /** @brief Creates a new STG with the provided parameters.
     *
     *  The STG has two parameters: the exponent and the scale factor.
     *  The scale factor is usually fixed at -1.0 over the exponent; however we
     *  allow the user to specify whatever coefficient they want (N.B. the
     *  default coefficient is always -1.0/1.2 even when the exponent is not
     *  1.2).
     *
     *  @param[in] exp The exponent of the STG. Defaults to 1.2.
     *  @param[in] c The scalar multiplying the gemain. Defaults to -1.0/1.2
     *
     *  @throw None No throw guarantee.
     */
    explicit SlaterTypeGeminal(exponent_type exp  = 1.2,
                               coefficient_type c = -1.0 / 1.2);

    /** @brief Sets this STG to the product of itself with @p rhs.
     *
     *  The product of two STGs is the sum of their exponents and the product of
     *  their coeffcients. This function will set the current STG to product of
     *  this STG with @p rhs.
     *
     *  @param[in] rhs The STG we are taking the product with.
     *
     *  @return The current STG after setting it to the product of this and
     *          @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    auto& operator*=(const SlaterTypeGeminal& rhs);

    /** @brief Returns th product of this geminal with @p rhs.
     *
     *  The product of two STGs is the sum of their exponents and the product of
     *  their coeffcients. This function will set the current STG to product of
     *  this STG with @p rhs.
     *
     *  @param[in] rhs The STG we are taking the product with.
     *
     *  @return A new STG set to the product of this with @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    auto operator*(const SlaterTypeGeminal& rhs) const;

    /** @brief Hashes the current STG.
     *
     *  An STG has two pieces of state: the coefficient and the exponent. This
     *  function updates the hash of the provided hasher with the hashes of this
     *  STG's state.
     *
     *  @param[in,out] h The hasher to use for hashing. After this call the
     *                   internal state of @p h will be updated with the hashes
     *                   of this instance's state.
     */
    void hash(chemist::detail_::Hasher& h) const { h(coefficient, exponent); }

    /// The scalar multiplying the STG
    double coefficient;

    /// The exponent of the STG
    double exponent;
};

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

/// Type of a SlaterTypeGeminal correlating two electrons
using STG = SlaterTypeGeminal<chemist::Electron, chemist::Electron>;

} // namespace chemist::operators
