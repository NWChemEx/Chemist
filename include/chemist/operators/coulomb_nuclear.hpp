/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
  Derivative<CoulombInteraction<Particles...>, chemist::Nuclei>;

/// Type of the two-electron repulsion operator
using ElectronRepulsion_Nuclear =
  CoulombInteraction_Nuclear<chemist::Electron, chemist::Electron>;

/// Type of the nuclei-nuclei repulsion operator
using NuclearRepulsion_Nuclear = CoulombInteraction_Nuclear<chemist::Nuclei>;

/// Type of the electron-nuclei attraction operator
using ElectronNuclearAttraction_Nuclear =
  CoulombInteraction_Nuclear<chemist::Electron, chemist::Nuclei>;

/// Type of a one-electron Coulomb interaction with a one-electron density
using ElectronEDensityCoulomb_Nuclear =
  CoulombInteraction_Nuclear<chemist::Electron, chemist::OneElectronDensity>;

/// Type of the many-electron repulsion operator
using NElectronRepulsion_Nuclear =
  CoulombInteraction_Nuclear<chemist::ManyElectrons>;

/// Type of the many-electron, nuclei attraction operator
using NElectronNuclearAttraction_Nuclear =
  CoulombInteraction_Nuclear<chemist::ManyElectrons, chemist::Nuclei>;

} // namespace chemist::operators
