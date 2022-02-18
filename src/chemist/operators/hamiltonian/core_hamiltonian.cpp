#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/hamiltonian/core_hamiltonian.hpp"
#include "chemist/operators/kinetic.hpp"

namespace chemist::operators {

CoreHamiltonian::CoreHamiltonian(const ElectronicHamiltonian& He) {
    using T_type = NElectronKinetic;
    using t_type = ElectronKinetic;
    using V_type = NElectronNuclearAttraction;
    using v_type = ElectronNuclearAttraction;

    if(He.has_term<T_type>()) add_term(t_type{});
    for(const auto& V : He.get_terms<V_type>())
        add_terms(v_type{chemist::Electron{}, V->at<1>()});
}

} // namespace chemist::operators
