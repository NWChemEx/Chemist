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

#include "basis_set/export_basis_set.hpp"
#include "chemical_system/export_chemical_system.hpp"
#include "enums.hpp"
#include "molecule/export_molecule.hpp"
#include "nucleus/export_nucleus.hpp"
#include "point/export_point.hpp"
#include "point_charge/export_point_charge.hpp"
#include "pychemist.hpp"

namespace chemist {

void export_chemist_enums(python_module_reference m) {
    python_enum_type<ShellType>(m, "ShellType")
      .value("cartesian", ShellType::cartesian)
      .value("pure", ShellType::pure);

    python_enum_type<GaugeType>(m, "GaugeType")
      .value("length", GaugeType::length)
      .value("velocity", GaugeType::velocity);
}

PYBIND11_MODULE(chemist, m) {
    m.doc() = "PyChemist : Python bindings for Chemist";

    // Need to be exported in hierarchical order
    export_point(m);
    export_point_charge(m);
    export_nucleus(m);
    export_molecule(m);
    export_chemical_system(m);

    export_chemist_enums(m);
    export_basis_set(m);
}

} // namespace chemist
