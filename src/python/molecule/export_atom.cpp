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
#include <chemist/molecule/atom.hpp>
#include <pybind11/operators.h>
#include <sstream>

namespace chemist {

void export_atom(python_module_reference m) {
    using atom_type          = Atom;
    using atom_reference     = atom_type&;
    using name_type          = typename atom_type::name_type;
    using atomic_number_type = typename atom_type::atomic_number_type;
    using mass_type          = typename atom_type::mass_type;
    using coord_type         = typename atom_type::coord_type;
    using coord_reference    = typename atom_type::coord_reference;
    using charge_type        = typename atom_type::charge_type;
    using nucleus_type       = typename atom_type::nucleus_type;
    using size_type          = typename atom_type::size_type;

    using coord_fxn = coord_reference (atom_type::*)(size_type);

    python_class_type<atom_type>(m, "Atom")
      .def(pybind11::init<>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type, coord_type,
                          coord_type, coord_type>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type, coord_type,
                          coord_type, coord_type, charge_type>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type, coord_type,
                          coord_type, coord_type, charge_type, size_type>())
      .def_property(
        "name", [](atom_reference self) { return self.name(); },
        [](atom_reference self, name_type name) { self.name() = name; })
      .def_property(
        "nucleus", [](atom_reference self) { return self.nucleus(); },
        [](atom_reference self, nucleus_type nuke) {
            self.nucleus() = std::move(nuke);
        })
      .def_property(
        "Z", [](atom_reference self) { return self.Z(); },
        [](atom_reference self, atomic_number_type Z) { self.Z() = Z; })
      .def_property(
        "mass", [](atom_reference self) { return self.mass(); },
        [](atom_reference self, mass_type m) { self.mass() = m; })
      .def_property(
        "nuclear_charge",
        [](atom_reference self) { return self.nuclear_charge(); },
        [](atom_reference self, charge_type q) { self.nuclear_charge() = q; })
      .def_property(
        "n_electrons", [](atom_reference self) { return self.n_electrons(); },
        [](atom_reference self, size_type n) { self.n_electrons() = n; })
      .def("charge", &Atom::charge)
      .def("coord", static_cast<coord_fxn>(&Atom::coord))
      .def_property(
        "x", [](atom_reference self) { return self.x(); },
        [](atom_reference self, coord_type x) { self.x() = x; })
      .def_property(
        "y", [](atom_reference self) { return self.y(); },
        [](atom_reference self, coord_type y) { self.y() = y; })
      .def_property(
        "z", [](atom_reference self) { return self.z(); },
        [](atom_reference self, coord_type z) { self.z() = z; })
      .def("__str__",
           [](const chemist::Atom& atom) {
               std::ostringstream stream;
               stream << atom;
               return stream.str();
           })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
