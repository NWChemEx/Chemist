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
