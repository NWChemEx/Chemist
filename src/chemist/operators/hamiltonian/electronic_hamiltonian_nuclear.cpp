#include "chemist/operators/coulomb_nuclear.hpp"
#include "chemist/operators/hamiltonian/electronic_hamiltonian_nuclear.hpp"
#include "chemist/operators/kinetic_nuclear.hpp"

namespace chemist::operators {

ElectronicHamiltonian_Nuclear::ElectronicHamiltonian_Nuclear(
  const Hamiltonian_Nuclear& H) {
    using T_type = NElectronKinetic_Nuclear;
    using V_type = NElectronNuclearAttraction_Nuclear;
    using G_type = NElectronRepulsion_Nuclear;

    for(const auto& T : H.get_terms<T_type>()) add_terms(*T);
    for(const auto& V : H.get_terms<V_type>()) add_terms(*V);
    for(const auto& G : H.get_terms<G_type>()) add_terms(*G);
}

} // namespace chemist::operators
