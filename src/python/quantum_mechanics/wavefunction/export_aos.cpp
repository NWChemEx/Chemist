/*
 * Copyright 2024 NWChemEx-Project
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

#include "export_wavefunction.hpp"
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <pybind11/operators.h>

namespace chemist::wavefunction {

void export_aos(python_module_reference m) {
    using aos_reference = AOs&;
    using ao_basis_set  = typename AOs::ao_basis_type;

    auto get_aos = [](aos_reference self) { return self.ao_basis_set(); };
    auto set_aos = [](aos_reference self, ao_basis_set& aos) {
        self.ao_basis_set() = aos;
    };

    python_class_type<AOs, VectorSpace>(m, "AOs")
      .def(pybind11::init<>())
      .def(pybind11::init<ao_basis_set>())
      .def_property("ao_basis_set", get_aos, set_aos)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist::wavefunction
