#pragma once
#include "libchemist/types.hpp"

namespace libchemist::basis_set::detail_ {

/** @brief Consolidates the types relevant to a Guassian primitive
 *
 *
 * @tparam T          double or float, used to set the defaults for the other
 *                    types.
 * @tparam ExpType    The type of a Gaussian's exponent, defaults to @p T
 * @tparam CoefType   The type of a Gaussian's contraction coefficient,
 *                    defaults to @p T
 * @tparam CenterType Type used to hold the Cartesian coordinates of the point
 *                    the basis function, shell, etc. is centered on. Defaults
 *                    type::coord<T>.
 */
template<typename CoefType, typename ExpType, typename CenterType>
struct GaussianTraits {
    /// Type used to hold the Gaussian's coefficient
    using coef_type = CoefType;
    /// Type used to hold the Gaussian's exponent
    using exp_type = ExpType;
    /// Type used to store the Gaussian's center
    using center_type = CenterType;
};

} // namespace libchemist::basis_set::detail_
