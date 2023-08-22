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
    auto mbasis_set = m.def_submodule("basis_set");

    export_primitive(mbasis_set);
    export_primitive_view(mbasis_set);
    export_contracted_gaussian(mbasis_set);
    export_contracted_gaussian_view(mbasis_set);
    export_shell(mbasis_set);
    export_shell_view(mbasis_set);
    export_atomic_basis_set(mbasis_set);
    export_atomic_basis_set_view(mbasis_set);
    export_ao_basis_set(mbasis_set);
}

} // namespace chemist