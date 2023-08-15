/*
 * Copyright 2023 NWChemEx-Project
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
#include "../pychemist.hpp"
#include <pybind11/operators.h>

namespace chemist {

void export_ao_basis_set(python_module_reference m);
void export_atomic_basis_set(python_module_reference m);
void export_atomic_basis_set_view(python_module_reference m);
void export_shell(python_module_reference m);
void export_shell_view(python_module_reference m);
void export_contracted_gaussian(python_module_reference m);
void export_contracted_gaussian_view(python_module_reference m);
void export_primitive(python_module_reference m);
void export_primitive_view(python_module_reference m);

inline void export_basis_set(python_module_reference m) {
    export_ao_basis_set(m);
    export_atomic_basis_set(m);
    export_atomic_basis_set_view(m);
    export_shell(m);
    export_shell_view(m);
    export_contracted_gaussian(m);
    export_contracted_gaussian_view(m);
    export_primitive(m);
    export_primitive_view(m);
}

} // namespace chemist