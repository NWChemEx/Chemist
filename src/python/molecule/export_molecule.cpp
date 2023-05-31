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

#include "export_molecule.hpp"
#include <chemist/molecule/molecule.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_molecule(python_module_reference m) {
    export_atom(m);

    using molecule_type      = Molecule;
    using molecule_reference = molecule_type&;
    using atom_type          = typename Molecule::atom_type;
    using size_type          = typename Molecule::size_type;

    python_class_type<Molecule>(m, "Molecule")
      .def(pybind11::init<>())
      .def("empty", [](molecule_reference self) { return self.empty(); })
      .def("push_back",
           [](molecule_reference self, atom_type v) {
               return self.push_back(std::move(v));
           })
      .def("at", [](molecule_reference self, size_type i) { return self[i]; })
      .def("size", [](molecule_reference self) { return self.size(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
