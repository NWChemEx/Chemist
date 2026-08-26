/*
 * Copyright 2026 NWChemEx-Project
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

#include "export_operator.hpp"
#include <chemist/quantum_mechanics/operator/fock.hpp>

namespace chemist::qm_operator {

void export_fock(python_module_reference m) {
    using size_type = typename Fock::size_type;

    auto size_fn  = [](const Fock& self) { return self.size(); };
    auto coeff_fn = [](const Fock& self, size_type i) {
        return self.coefficient(i);
    };
    auto get_op_fn = [](const Fock& self, size_type i) {
        return self.get_operator(i).clone();
    };
    auto emplace_back_fn = [](Fock& self, typename Fock::coefficient_type c,
                              typename Fock::base_pointer op) {
        self.emplace_back(c, std::move(op));
    };

    py::keep_alive<0, 1> ka;

    // N.B. Unlike the Hamiltonians, Fock has no core_hamiltonian conversion,
    // so this can't reuse export_hamiltonian_.
    python_class_type<Fock, OperatorBase, py::smart_holder>(m, "Fock")
      .def(py::init<>())
      .def("size", size_fn)
      .def("coefficient", coeff_fn)
      .def("get_operator", get_op_fn, ka)
      .def("emplace_back", emplace_back_fn)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace chemist::qm_operator
