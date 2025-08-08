/*
 * Copyright 2024 NWChemEx-Project
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
