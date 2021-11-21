#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/hamiltonian/electronic_hamiltonian.hpp"
#include "chemist/operators/kinetic.hpp"

namespace chemist::operators {

ElectronicHamiltonian::ElectronicHamiltonian(const Hamiltonian& H) {
    using T_type = NElectronKinetic;
    using V_type = NElectronNuclearAttraction;
    using G_type = NElectronRepulsion;

    for(const auto& T : H.get_terms<T_type>()) add_terms(*T);
    for(const auto& V : H.get_terms<V_type>()) add_terms(*V);
    for(const auto& G : H.get_terms<G_type>()) add_terms(*G);
}

} // namespace chemist::operators
