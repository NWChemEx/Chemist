#include "detail_/get_terms.hpp"
#include <chemist/quantum_mechanics/operator/hamiltonian.hpp>

namespace chemist::qm_operator {

typename Hamiltonian::const_electronic_hamiltonian_reference
Hamiltonian::electronic_hamiltonian() const {
    return detail_::get_terms<electronic_hamiltonian_type, detail_::EHVisitor>(
      *this);
}

Hamiltonian::const_core_hamiltonian_reference Hamiltonian::core_hamiltonian()
  const {
    return detail_::get_terms<core_hamiltonian_type, detail_::CHVisitor>(*this);
}

} // namespace chemist::qm_operator