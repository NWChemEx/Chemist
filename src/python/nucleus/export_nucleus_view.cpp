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

#include "export_nucleus.hpp"
#include <chemist/nucleus/nucleus_view.hpp>
#include <pybind11/operators.h>
#include <sstream>

namespace chemist {

void export_nucleus_view(python_module_reference m) {
    using nucleus_type       = Nucleus;
    using view_type          = NucleusView<nucleus_type>;
    using view_reference     = view_type&;
    using nucleus_reference  = typename view_type::nucleus_reference;
    using charge_view_type   = typename view_type::charge_view_type;
    using name_type          = typename view_type::name_type;
    using atomic_number_type = typename view_type::atomic_number_type;
    using mass_type          = typename view_type::mass_type;

    python_class_type<view_type, charge_view_type>(m, "NucleusView")
      .def(pybind11::init<nucleus_reference>())
      .def_property(
        "name", [](view_reference self) { return self.name(); },
        [](view_reference self, name_type name) {
            self.name() = std::move(name);
        })
      .def_property(
        "Z", [](view_reference self) { return self.Z(); },
        [](view_reference self, atomic_number_type Z) { self.Z() = Z; })
      .def_property(
        "mass", [](view_reference self) { return self.mass(); },
        [](view_reference self, mass_type m) { self.mass() = m; })
      .def("__str__",
           [](const view_type& nv) {
               std::ostringstream stream;
               stream << nv;
               return stream.str();
           })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self == nucleus_type())
      .def(nucleus_type() == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def(pybind11::self != nucleus_type())
      .def(nucleus_type() != pybind11::self);
}

} // namespace chemist
