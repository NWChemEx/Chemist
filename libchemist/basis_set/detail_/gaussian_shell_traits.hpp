#pragma once
#include "libchemist/types.hpp"
#include <array>
#include <vector>

namespace libchemist::basis_set::detail_ {

/** @brief Holds the types if a GaussianShell_'s members
 *
 * @tparam CoefArray The type of the container holding the coefficients
 * @tparam ExpArray The type of the container holding the exponents
 * @tparam CenterType The type holding the Cartesian coordinates
 * @tparam IsPure True if this shell is spherical and false if it is Cartesian
 * @tparam AMType The type used to store the magnitude of the angular momentum.
 *                 Defaults to type::angular_momentum.
 * @tparam PureFlag Type used to hold the flag which determines if a shell is
 *                  pure or not.
 */
template<typename CoefArray, typename ExpArray, typename CenterType,
         typename AMType = type::angular_momentum, typename PureFlag = bool>
struct GaussianShellTraits_ {
    /// Type used to hold whether the shell is pure or not
    using pure_flag = PureFlag;
    /// Type used to hold the angular momentum
    using am_type = AMType;
    /// Type of the container used to hold contraction coefficients
    using coef_container = CoefArray;
    /// Type of the container used to hold the exponents
    using exp_container = ExpArray;
    /// Type of the Cartesian coordinates of the center
    using center_type = CenterType;
};

/// Traits for a shell of Gaussians with a dynamic number of primitives
template<typename T>
using GaussianShellTraits =
  GaussianShellTraits_<std::vector<T>, std::vector<T>, type::coord<T>>;

/// Shell of Gaussians made from a compile-time known number of primitives
template<typename T, type::size N>
using StaticGuassianShellTraits =
  GaussianShellTraits_<std::array<T, N>, std::array<T, N>, type::coord<T>>;

} // namespace libchemist::basis_set::detail_
