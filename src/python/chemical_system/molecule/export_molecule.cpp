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
#include <chemist/chemical_system/molecule/molecule.hpp>
#include <chemist/chemical_system/nucleus/nuclei.hpp>
#include <pybind11/operators.h>
#include <sstream>
#include <vector>

namespace chemist {
namespace detail_ {
void export_molecule_(python_module_reference m) {
    using molecule_type      = Molecule;
    using molecule_reference = molecule_type&;
    using atom_type          = typename Molecule::atom_type;
    using size_type          = typename Molecule::size_type;
    using charge_type        = typename Molecule::charge_type;
    using multiplicity_type  = typename Molecule::multiplicity_type;
    using nuclei_type        = typename Molecule::nuclei_type;

    // Lambdas needed for exposing class, defined here to make pybind code
    // easier to read
    //@{
    auto value_ctor = [](charge_type q, multiplicity_type m,
                         pybind11::list py_atoms) {
        auto atoms = pybind_to_buffer<atom_type>(py_atoms);
        nuclei_type nuclei;
        for(auto& atom_i : atoms) nuclei.push_back(atom_i.nucleus());
        return Molecule(q, m, std::move(nuclei));
    };

    auto push_back = [](molecule_reference self, atom_type v) {
        return self.push_back(std::move(v));
    };

    auto nuclei_fxn = [](molecule_reference self) { return self.nuclei(); };
    auto at_fxn = [](molecule_reference self, size_type i) { return self[i]; };

    auto set_charge = [](molecule_reference self, charge_type c) {
        self.set_charge(c);
    };

    auto set_mult = [](molecule_reference self, size_type m) {
        self.set_multiplicity(m);
    };

    auto str_fxn = [](const chemist::Molecule& mol) {
        std::ostringstream stream;
        stream << mol;
        return stream.str();
    };

    auto iter_fxn = [](molecule_reference self) {
        return pybind11::make_iterator(self.begin(), self.end());
    };
    //@}

    pybind11::keep_alive<0, 1> ka;

    python_class_type<Molecule>(m, "Molecule")
      .def(pybind11::init<>())
      .def(pybind11::init<charge_type, size_type, nuclei_type>())
      .def(pybind11::init(value_ctor))
      .def("empty", [](molecule_reference self) { return self.empty(); })
      .def("push_back", push_back)
      .def("nuclei", nuclei_fxn, ka)
      .def("at", at_fxn, ka)
      .def("size", [](molecule_reference self) { return self.size(); })
      .def("charge", &Molecule::charge)
      .def("n_electrons", &Molecule::n_electrons)
      .def("set_charge", set_charge)
      .def("multiplicity", &Molecule::multiplicity)
      .def("set_multiplicity", set_mult)
      .def("__str__", str_fxn)
      .def("__iter__", iter_fxn, ka)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}
} // namespace detail_

void export_molecule(python_module_reference m) {
    export_atom(m);
    detail_::export_molecule_(m);
    export_molecule_view(m);
}

} // namespace chemist
