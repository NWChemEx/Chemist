#include "libchemist/operators/coulomb_nuclear.hpp"
#include "libchemist/operators/hamiltonian/core_hamiltonian_nuclear.hpp"
#include "libchemist/operators/kinetic_nuclear.hpp"

namespace libchemist::operators {

CoreHamiltonian_Nuclear::CoreHamiltonian_Nuclear(
  const ElectronicHamiltonian_Nuclear& He) {
    using T_type = NElectronKinetic_Nuclear;
    using t_type = ElectronKinetic_Nuclear;
    using V_type = NElectronNuclearAttraction_Nuclear;
    using v_type = ElectronNuclearAttraction_Nuclear;

    if(He.has_term<T_type>()) add_term(t_type{});
    for(const auto& V : He.get_terms<V_type>()) {
        add_term(v_type{});
        // The line above should be. Everything is encapsulated in the
        // ElectronNuclearAttraction_Nuclear data type.
        // add_terms(v_type{libchemist::Electron{}, V->at<1>()});
    }
}

} // namespace libchemist::operators
