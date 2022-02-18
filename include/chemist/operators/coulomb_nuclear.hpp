#pragma once
#include "chemist/density/density.hpp"
#include "chemist/electrons/electrons.hpp"
#include "chemist/molecule/molecule.hpp"
#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/derivative.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

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
  Derivative<CoulombInteraction<Particles...>,chemist::Nuclei>;

/// Type of the two-electron repulsion operator
using ElectronRepulsion_Nuclear =
  CoulombInteraction_Nuclear<chemist::Electron, chemist::Electron>;

/// Type of the nuclei-nuclei repulsion operator
using NuclearRepulsion_Nuclear = 
  CoulombInteraction_Nuclear<chemist::Nuclei>;

/// Type of the electron-nuclei attraction operator
using ElectronNuclearAttraction_Nuclear =
  CoulombInteraction_Nuclear<chemist::Electron, chemist::Nuclei>;

/// Type of a one-electron Coulomb interaction with a one-electron density
using ElectronEDensityCoulomb_Nuclear =
  CoulombInteraction_Nuclear<chemist::Electron,
                             chemist::OneElectronDensity>;

/// Type of the many-electron repulsion operator
using NElectronRepulsion_Nuclear =
  CoulombInteraction_Nuclear<chemist::ManyElectrons>;

/// Type of the many-electron, nuclei attraction operator
using NElectronNuclearAttraction_Nuclear =
  CoulombInteraction_Nuclear<chemist::ManyElectrons, chemist::Nuclei>;

} // namespace chemist::operators
