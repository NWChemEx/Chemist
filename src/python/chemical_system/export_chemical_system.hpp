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
#include <chemist/chemical_system/chemical_system.hpp>
#include <pybind11/operators.h>

namespace chemist {

void inline export_chemical_system(python_module_reference m) {
    using chemical_system_type      = ChemicalSystem;
    using chemical_system_reference = chemical_system_type&;
    using molecule_type             = typename chemical_system_type::molecule_t;
    using size_type                 = typename chemical_system_type::size_type;

    // N.B. We only expose the parts of the ChemicalSystem's API which have
    //      been finalized (n_electrons/charge are moving to molecule) and the
    //      potential class is not finished.

    python_class_type<ChemicalSystem>(m, "ChemicalSystem")
      .def(pybind11::init<>())
      .def(pybind11::init<molecule_type>())
      .def_property(
        "molecule",
        [](chemical_system_reference self) { return self.molecule(); },
        [](chemical_system_reference self, molecule_type mol) {
            self.molecule() = std::move(mol);
        })
      .def("charge", &ChemicalSystem::charge)
      // .def("n_electrons", &ChemicalSystem::n_electrons)
      .def_property(
        "n_electrons",
        [](chemical_system_reference self) { return self.n_electrons(); },
        [](chemical_system_reference self, size_type n) {
            self.n_electrons() = n;
        })
      .def("__str__",
           [](const chemist::ChemicalSystem& cs) {
               std::ostringstream stream;
               stream << cs;
               return stream.str();
           })
      .def("__repr__",
           [](const chemist::ChemicalSystem& cs) {
               std::ostringstream stream;
               stream << cs;
               return stream.str();
           })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
