#pragma once
#include "libchemist/operator/stateless_operator.hpp"

namespace libchemist {

template<typename Particle1, typename Particle2>
class CoulombInteraction
  : public detail_::OperatorImpl<CoulombInteraction, Particle1, Particle2> {};

using ElectronElectronCoulomb = CoulombInteraction<Electron, Electron>;
using NuclearNuclearCoulomb   = CoulombInteraction<Nuclei, Nuclei>;
using ElectronNuclearCoulomb  = CoulombInteraction<Electron, Nuclei>;

} // namespace libchemist
