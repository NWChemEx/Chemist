#pragma once

#include "libchemist/operators/correlation_factor.hpp"
#include "libchemist/operators/coulomb.hpp"
#include "libchemist/operators/derivative.hpp"
#include "libchemist/operators/dirac_delta.hpp"
#include "libchemist/operators/exchange.hpp"
#include "libchemist/operators/exchange_correlation.hpp"
#include "libchemist/operators/f12_commutator.hpp"
#include "libchemist/operators/fock.hpp"
#include "libchemist/operators/hamiltonian/core_hamiltonian.hpp"
#include "libchemist/operators/hamiltonian/core_hamiltonian_nuclear.hpp"
#include "libchemist/operators/hamiltonian/electronic_hamiltonian.hpp"
#include "libchemist/operators/hamiltonian/electronic_hamiltonian_nuclear.hpp"
#include "libchemist/operators/hamiltonian/hamiltonian.hpp"
#include "libchemist/operators/hamiltonian/hamiltonian_nuclear.hpp"
#include "libchemist/operators/identity.hpp"
#include "libchemist/operators/kinetic.hpp"
#include "libchemist/operators/multipole.hpp"
#include "libchemist/operators/slater_type_geminal.hpp"
#include "libchemist/operators/type_traits/type_traits.hpp"
#include "libchemist/operators/yukawa.hpp"

namespace libchemist::operators {

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
} // namespace libchemist::operators
