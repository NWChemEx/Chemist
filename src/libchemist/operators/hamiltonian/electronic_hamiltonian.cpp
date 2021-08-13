#include "libchemist/operators/coulomb.hpp"
#include "libchemist/operators/hamiltonian/electronic_hamiltonian.hpp"
#include "libchemist/operators/kinetic.hpp"

namespace libchemist::operators {

ElectronicHamiltonian::ElectronicHamiltonian(const Hamiltonian& H) {
    using T_type = NElectronKinetic;
    using V_type = NElectronNuclearAttraction;
    using G_type = NElectronRepulsion;

    for(const auto& T : H.get_terms<T_type>()) add_terms(*T);
    for(const auto& V : H.get_terms<V_type>()) add_terms(*V);
    for(const auto& G : H.get_terms<G_type>()) add_terms(*G);
}

} // namespace libchemist::operators
