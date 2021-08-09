#pragma once
#include "libchemist/molecule/molecule.hpp"
#include "libchemist/operator/density.hpp"
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/many_electrons.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist {

template<typename... Particles>
class CoulombInteraction
  : public detail_::OperatorImpl<CoulombInteraction, Particles...> {
private:
    using base_type = detail_::OperatorImpl<CoulombInteraction, Particles...>;

public:
    using base_type::base_type;
};

using ElectronElectronCoulomb = CoulombInteraction<Electron, Electron>;
using NuclearNuclearCoulomb   = CoulombInteraction<Nuclei, Nuclei>;
using ElectronNuclearCoulomb  = CoulombInteraction<Electron, Nuclei>;
using ElectronEDensityCoulomb =
  CoulombInteraction<Electron, OneElectronDensity>;
using NElectronNElectronCoulomb =
  CoulombInteraction<ManyElectrons, ManyElectrons>;
using NElectronNuclearCoulomb  =
  CoulombInteraction<ManyElectrons, Nuclei>;

} // namespace libchemist
