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

#pragma once
#include "braket/export_braket.hpp"
#include "operator/operator/export_operator.hpp"
#include "wavefunction/export_wavefunction.hpp"

namespace chemist {

inline void export_quantum_mechanics(python_module_reference m) {
    // Need to be exported in hierarchical order. BraKet comes last: its
    // instantiations name operator and wavefunction types, so those classes
    // have to be registered with pybind11 first.
    qm_operator::export_qm_operator(m);
    wavefunction::export_wavefunction(m);
    braket::export_braket(m);
}

} // namespace chemist
