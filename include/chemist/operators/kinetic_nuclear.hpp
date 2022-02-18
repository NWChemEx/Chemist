#pragma once
#include "chemist/electrons/electrons.hpp"
#include "chemist/molecule/molecule.hpp"
#include "chemist/operators/kinetic.hpp"
#include "chemist/operators/derivative.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

/** @brief Models the kinetic energy operator of a particle
 *
 *  @tparam Particle The type of the particle.
 */
template<typename Particle>
using Kinetic_Nuclear =
  Derivative<Kinetic<Particle>,chemist::Nuclei>;

/// Type of the electronic kinetic energy operator
using ElectronKinetic_Nuclear = Kinetic_Nuclear<chemist::Electron>;

/// Type of the many electron kinetic energy operator
using NElectronKinetic_Nuclear = Kinetic_Nuclear<chemist::ManyElectrons>;

} // namespace chemist::operators
