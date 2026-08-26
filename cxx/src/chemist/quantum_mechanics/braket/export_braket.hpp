/*
 * Copyright 2026 NWChemEx-Project
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

namespace chemist::braket {

void export_tensor_representation(python_module_reference m);
void export_py_braket(python_module_reference m);
void export_braket_instantiations(python_module_reference m);

inline void export_braket(python_module_reference m) {
    auto m_bk = m.def_submodule("braket");

    // Need to be exported in hierarchical order. The instantiations must come
    // last: each one registers an implicit conversion from BraKet (the erased
    // handle exported by export_py_braket), so that class must already exist.
    export_tensor_representation(m_bk);
    export_py_braket(m_bk);
    export_braket_instantiations(m_bk);
}

} // namespace chemist::braket
