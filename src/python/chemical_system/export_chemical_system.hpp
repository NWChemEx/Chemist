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
#include "chemical_system/export_chemical_system.hpp"
#include "electron/export_electron.hpp"
#include "molecule/export_molecule.hpp"
#include "nucleus/export_nucleus.hpp"
#include "point_charge/export_point_charge.hpp"
#include <chemist/chemical_system/chemical_system.hpp>
#include <pybind11/operators.h>

namespace chemist {

void inline export_chemical_system(python_module_reference m) {
    export_electron(m);
    export_point_charge(m);
    export_nucleus(m);
    export_molecule(m);
    export_chemical_system_subcomponent(m);
}

} // namespace chemist
