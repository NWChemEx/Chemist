/*
 * Copyright 2022 NWChemEx-Project
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

#include "chemist/operators/coulomb_nuclear.hpp"
#include "chemist/operators/hamiltonian/core_hamiltonian_nuclear.hpp"
#include "chemist/operators/kinetic_nuclear.hpp"

namespace chemist::operators {

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

} // namespace chemist::operators
