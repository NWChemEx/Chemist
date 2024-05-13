/*
 * Copyright 2023 NWChemEx Community
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

#include "export_molecule.hpp"
#include <chemist/chemical_system/molecule/molecule_view/molecule_view.hpp>
#include <chemist/chemical_system/nucleus/nuclei.hpp>
#include <pybind11/operators.h>
#include <sstream>
#include <vector>

namespace chemist {

void export_molecule_view(python_module_reference m) {
    using molecule_type = Molecule;
    using view_type     = MoleculeView<molecule_type>;
    using reference     = view_type&;
    using atom_type     = typename molecule_type::atom_type;
    using size_type     = typename molecule_type::size_type;
    using charge_type   = typename molecule_type::charge_type;
    using nuclei_type   = typename molecule_type::nuclei_type;

    python_class_type<view_type>(m, "MoleculeView")
      .def(pybind11::init<>())
      .def(pybind11::init<molecule_type&>())
      .def("empty", [](reference self) { return self.empty(); })
      .def(
        "at", [](reference self, size_type i) { return self[i]; },
        pybind11::keep_alive<0, 1>())
      .def("size", [](reference self) { return self.size(); })
      .def("charge", &view_type::charge)
      .def("n_electrons", &view_type::n_electrons)
      .def("set_charge",
           [](reference self, charge_type c) { self.set_charge(c); })
      .def("multiplicity", &molecule_type::multiplicity)
      .def("set_multiplicity",
           [](reference self, size_type m) { self.set_multiplicity(m); })
      .def(
        "__iter__",
        [](reference self) {
            return pybind11::make_iterator(self.begin(), self.end());
        },
        pybind11::keep_alive<0, 1>())
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self == molecule_type{})
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
