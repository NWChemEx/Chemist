#pragma once
#include "libchemist/types.hpp"

namespace libchemist::basis_set::detail_ {

/** @brief Determines the types of a GaussianCenter's member variables
 *
 *  @tparam CenterType The type of the Cartesian coefficient
 *  @tparam AMCont Type of a container holding the angular momentum of each
 * shell
 *  @tparam PureCont Type of a container holding the purity of each shell
 *  @tparam NPrimCont Type of a container holding the number of primitives on
 *                    each center
 *  @tparam CoefCont Type of a container holding all the coefficients for the
 *                   shells.
 *  @tparam ExpCont Type of a container holding all the exponents for the
 * shells.
 */
template<typename CenterType, typename AMCont, typename PureCont,
         typename NPrimCont, typename CoefCont, typename ExpCont>
struct GaussianCenterTraits {
    /// Type to use to hold the Cartesian coordinate
    using center_type = CenterType;
    /// Type used to hold the angular momenta
    using am_container = AMCont;
    /// Type used to hold whether a shell is pure or not
    using pure_container = PureCont;
    /// Type used to hold the number of primitives per shell
    using nprim_container = NPrimCont;
    /// Type used to hold the primitive contraction coefficients
    using coef_container = CoefCont;
    /// Type used to hold the primitive exponents
    using exp_container = ExpCont;
};

} // namespace libchemist::basis_set::detail_
