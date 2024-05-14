/*
 * Copyright 2024 NWChemEx Community
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
#include "export_chemical_system.hpp"
#include <chemist/chemical_system/chemical_system/chemical_system_view/chemical_system_view.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_chemical_system_view(python_module_reference m) {
    using chemical_system_type = ChemicalSystem;
    using view_type            = ChemicalSystemView<chemical_system_type>;
    using reference            = view_type&;
    using molecule_type        = typename chemical_system_type::molecule_t;

    python_class_type<view_type>(m, "ChemicalSystemView")
      .def(pybind11::init<>())
      .def(pybind11::init<chemical_system_type&>())
      .def_property(
        "molecule", [](reference self) { return self.molecule(); },
        [](reference self, molecule_type mol) { self.molecule() = mol; })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
