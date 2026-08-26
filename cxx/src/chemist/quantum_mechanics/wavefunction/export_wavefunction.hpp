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
#include "chemist/pychemist.hpp"

namespace chemist::wavefunction {

void export_vector_space(python_module_reference m);
void export_aos(python_module_reference m);
void export_transformed(python_module_reference m);
void export_natural(python_module_reference m);
void export_mos(python_module_reference m);
void export_cmos(python_module_reference m);
void export_wavefunction_class(python_module_reference m);
void export_determinant(python_module_reference m);

inline void export_wavefunction(python_module_reference m) {
    auto m_wf = m.def_submodule("wavefunction");

    // Need to be exported in hierarchical order
    export_vector_space(m_wf);
    export_aos(m_wf);
    export_transformed(m_wf);
    export_natural(m_wf);
    export_mos(m_wf);
    export_cmos(m_wf);
    export_wavefunction_class(m_wf);
    export_determinant(m_wf);
}

} // namespace chemist::wavefunction
