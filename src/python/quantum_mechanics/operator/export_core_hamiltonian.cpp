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
#include <chemist/quantum_mechanics/operator/core_hamiltonian.hpp>
#include <pybind11/operators.h>

namespace chemist::qm_operator {

void export_core_hamiltonian(python_module_reference m) {
    using hamiltonian_t = CoreHamiltonian;
    using size_type     = typename hamiltonian_t::size_type;
    using op_base_t     = OperatorBase;

    auto size_fn  = [](const hamiltonian_t& self) { return self.size(); };
    auto coeff_fn = [](const hamiltonian_t& self, size_type i) {
        return self.coefficient(i);
    };
    auto get_op_fn = [](const hamiltonian_t& self, size_type i) {
        return self.get_operator(i).clone();
    };
    auto emplace_back_fn = [](hamiltonian_t& self,
                              typename hamiltonian_t::coefficient_type c,
                              typename hamiltonian_t::base_pointer op) {
        self.emplace_back(c, std::move(op));
    };

    pybind11::keep_alive<0, 1> ka;

    python_class_type<hamiltonian_t, op_base_t, pybind11::smart_holder>(
      m, "CoreHamiltonian")
      .def(pybind11::init<>())
      .def("size", size_fn)
      .def("coefficient", coeff_fn)
      .def("get_operator", get_op_fn, ka)
      .def("emplace_back", emplace_back_fn)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist::qm_operator
