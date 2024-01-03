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
#include <chemist/nucleus/nuclei.hpp>
#include <pybind11/operators.h>
#include <vector>

namespace chemist {

void export_molecule(python_module_reference m) {
    export_atom(m);

    using molecule_type         = Molecule;
    using molecule_reference    = molecule_type&;
    using atom_type             = typename Molecule::atom_type;
    using size_type             = typename Molecule::size_type;
    using atom_initializer_list = std::initializer_list<atom_type>;
    using charge_type           = typename Molecule::charge_type;
    using nuclei_type           = typename Molecule::nuclei_type;

    python_class_type<Molecule>(m, "Molecule")
      .def(pybind11::init<>())
      .def(pybind11::init<charge_type, size_type, nuclei_type>())
      //.def(pybind11::init<atom_initializer_list>())
      //   .def(pybind11::init<charge_type, size_type, atom_initializer_list>())
      .def("empty", [](molecule_reference self) { return self.empty(); })
      .def("push_back",
           [](molecule_reference self, atom_type v) {
               return self.push_back(std::move(v));
           })
      .def("at", [](molecule_reference self, size_type i) { return self[i]; })
      .def("size", [](molecule_reference self) { return self.size(); })
      .def("charge", &Molecule::charge)
      .def("n_electrons", &Molecule::n_electrons)
      .def("set_charge",
           [](molecule_reference self, charge_type c) { self.set_charge(c); })
      .def("multiplicity", &Molecule::multiplicity)
      .def("set_multiplicity", [](molecule_reference self,
                                  size_type m) { self.set_multiplicity(m); })
      .def("__str__",
           [](const chemist::Molecule& mol) {
               std::ostringstream stream;
               stream << mol;
               return stream.str();
           })
      .def(
        "__iter__",
        [](molecule_reference self) {
            return pybind11::make_iterator(self.begin(), self.end());
        },
        pybind11::keep_alive<0, 1>())
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
