#pragma once

#include "chemist/operators/correlation_factor.hpp"
#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/derivative.hpp"
#include "chemist/operators/dirac_delta.hpp"
#include "chemist/operators/exchange.hpp"
#include "chemist/operators/exchange_correlation.hpp"
#include "chemist/operators/f12_commutator.hpp"
#include "chemist/operators/fock.hpp"
#include "chemist/operators/hamiltonian/core_hamiltonian.hpp"
#include "chemist/operators/hamiltonian/core_hamiltonian_nuclear.hpp"
#include "chemist/operators/hamiltonian/electronic_hamiltonian.hpp"
#include "chemist/operators/hamiltonian/electronic_hamiltonian_nuclear.hpp"
#include "chemist/operators/hamiltonian/hamiltonian.hpp"
#include "chemist/operators/hamiltonian/hamiltonian_nuclear.hpp"
#include "chemist/operators/identity.hpp"
#include "chemist/operators/kinetic.hpp"
#include "chemist/operators/multipole.hpp"
#include "chemist/operators/slater_type_geminal.hpp"
#include "chemist/operators/type_traits/type_traits.hpp"
#include "chemist/operators/yukawa.hpp"

namespace chemist::operators {

using ElectronIdentity_Nuclear = Derivative<ElectronIdentity, Nuclei>;

using ElectronKinetic_Nuclear = Derivative<ElectronKinetic, Nuclei>;

using NElectronKinetic_Nuclear = Derivative<NElectronKinetic, Nuclei>;

using ElectronNuclearAttraction_Nuclear =
  Derivative<ElectronNuclearAttraction, Nuclei>;

using NElectronNuclearAttraction_Nuclear =
  Derivative<NElectronNuclearAttraction, Nuclei>;

using ElectronRepulsion_Nuclear = Derivative<ElectronRepulsion, Nuclei>;

using NElectronRepulsion_Nuclear = Derivative<NElectronRepulsion, Nuclei>;

using NuclearRepulsion_Nuclear = Derivative<NuclearRepulsion, Nuclei>;
/*
using CoreHamiltonian_Nuclear = LinearCombination().add_terms(
          ElectronKinetic_Nuclear,
          ElectronNuclearAttraction_Nuclear);

using ElectronicHamiltonian_Nuclear = LinearCombination().add_terms(
          ElectronKinetic_Nuclear,
          ElectronNuclearAttraction_Nuclear,
          ElectronRepulsion_Nuclear);

using ElectronicHamiltonian_Nuclear = LinearCombination().add_terms(
          ElectronRepulsion_Nuclear,
          ElectronKinetic_Nuclear,
          ElectronNuclearAttraction_Nuclear,
          ElectronRepulsion_Nuclear);
*/
} // namespace chemist::operators
