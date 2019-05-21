#pragma once
#include "libchemist/basis_set/gaussian.hpp"
#include "libchemist/types.hpp"
#include <array>
#include <vector>

namespace libchemist::basis_set::detail_ {

/** @brief Traits for a GaussianAO_ instance
 *
 *  This class is used by the GaussianAO_ class to determine if it owns or
 *  aliases its members and whether they are read/writeable.
 *
 *  @tparam CoefArray The type of the container holding the contraction
 *                    coefficients.
 * @tparam ExpArray   The type of the container holding the exponents.
 * @tparam CenterType The type of the Cartesian coordinates.
 */
template<typename CoefArray, typename ExpArray, typename CenterType>
struct GaussianAOTraits_ {
    /// Type of the container holding the coefficients
    using coef_container = CoefArray;
    /// Type of the coefficients
    using coef_type = typename coef_container::value_type;
    /// Type of the container holding the exponents
    using exp_container = ExpArray;
    /// Type of the exponents
    using exp_type = typename exp_container::value_type;
    /// Type of the Cartesian coordinates
    using center_type = CenterType;
};

/// Defines the traits for a GaussianAO_ with a dynamic number of primitives
template<typename T>
using GaussianAOTraits =
  GaussianAOTraits_<std::vector<T>, std::vector<T>, type::coord<T>>;

/// Traits for a GaussianAO_ with a compile-time known number of primitives
template<typename T, type::size N>
using StaticGaussianAOTraits =
  GaussianAOTraits_<std::array<T, N>, std::array<T, N>, type::coord<T>>;

} // namespace libchemist::basis_set::detail_
