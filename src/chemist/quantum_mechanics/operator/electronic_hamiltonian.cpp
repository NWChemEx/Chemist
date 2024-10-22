#include "detail_/get_terms.hpp"
#include <chemist/quantum_mechanics/operator/electronic_hamiltonian.hpp>

namespace chemist::qm_operator {

ElectronicHamiltonian::const_core_hamiltonian_reference
ElectronicHamiltonian::core_hamiltonian() const {
    return detail_::get_terms<core_hamiltonian_type, detail_::CHVisitor>(*this);
}

} // namespace chemist::qm_operator