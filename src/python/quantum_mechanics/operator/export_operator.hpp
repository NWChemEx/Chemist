/*
 * Copyright 2025 NWChemEx-Project
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
#include "../../pychemist.hpp"

namespace chemist::qm_operator {

void export_identity(python_module_reference m);
void export_kinetic(python_module_reference m);
void export_coulomb(python_module_reference m);
void export_exchange(python_module_reference m);
void export_exchange_correlation(python_module_reference m);

inline void export_qm_operator(python_module_reference m) {
    auto m_op = m.def_submodule("qm_operator");

    export_identity(m_op);
    export_kinetic(m_op);
    export_coulomb(m_op);
    export_exchange(m_op);
    export_exchange_correlation(m_op);
}

} // namespace chemist::qm_operator
