#pragma once
#include "libchemist/density/density.hpp"
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/molecule/molecule.hpp"
#include "libchemist/operators/coulomb.hpp"
#include "libchemist/operators/derivative.hpp"
#include "libchemist/operators/detail_/operator_impl.hpp"

namespace libchemist::operators {

/** @brief Describes the electrostatic interaction among particles.
 *
 *  The CoulombInteraction describes the electrostatic interaction among charged
 *  particles.
 *
 *  @tparam Particles The type or types of the entities participating in the
 *                    interaction. Can be a single type if the type entails more
 *                    than one particle (e.g. Nuclei)
 *
 */
template<typename... Particles>
using CoulombInteraction_Nuclear =
  Derivative<CoulombInteraction<Particles...>, libchemist::Nuclei>;

/// Type of the two-electron repulsion operator
using ElectronRepulsion_Nuclear =
  CoulombInteraction_Nuclear<libchemist::Electron, libchemist::Electron>;

/// Type of the nuclei-nuclei repulsion operator
using NuclearRepulsion_Nuclear = CoulombInteraction_Nuclear<libchemist::Nuclei>;

/// Type of the electron-nuclei attraction operator
using ElectronNuclearAttraction_Nuclear =
  CoulombInteraction_Nuclear<libchemist::Electron, libchemist::Nuclei>;

/// Type of a one-electron Coulomb interaction with a one-electron density
using ElectronEDensityCoulomb_Nuclear =
  CoulombInteraction_Nuclear<libchemist::Electron,
                             libchemist::OneElectronDensity>;

/// Type of the many-electron repulsion operator
using NElectronRepulsion_Nuclear =
  CoulombInteraction_Nuclear<libchemist::ManyElectrons>;

/// Type of the many-electron, nuclei attraction operator
using NElectronNuclearAttraction_Nuclear =
  CoulombInteraction_Nuclear<libchemist::ManyElectrons, libchemist::Nuclei>;

} // namespace libchemist::operators
