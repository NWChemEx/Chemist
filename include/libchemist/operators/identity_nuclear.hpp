#pragma once
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/operators/detail_/operator_impl.hpp"

namespace libchemist::operators {

/** @brief Models the identity function
 *
 *  @tparam Particles The type(s) of the entities identified by the function.
 */
template<typename... Particles>
using Identity_Nuclear =
  Derivative<Identity<Particles...>,libchemist::Nuclei>;

/// The type of the electronic identity function
using ElectronIdentity_Nuclear = Identity_Nuclear<libchemist::Electron>;

} // namespace libchemist::operators
