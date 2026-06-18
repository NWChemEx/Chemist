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
#include <chemist/quantum_mechanics/operator/electronic_hamiltonian.hpp>
#include <chemist/quantum_mechanics/operator/hamiltonian.hpp>

namespace chemist::qm_operator {

namespace detail_ {

template<typename HamiltonianType>
void export_hamiltonian_(const char* name, python_module_reference m) {
    using h_t       = HamiltonianType;
    using size_type = typename h_t::size_type;
    using op_base_t = OperatorBase;

    auto size_fn  = [](const h_t& self) { return self.size(); };
    auto coeff_fn = [](const h_t& self, size_type i) {
        return self.coefficient(i);
    };
    auto get_op_fn = [](const h_t& self, size_type i) {
        return self.get_operator(i).clone();
    };
    auto emplace_back_fn = [](h_t& self, typename h_t::coefficient_type c,
                              typename h_t::base_pointer op) {
        self.emplace_back(c, std::move(op));
    };

    py::keep_alive<0, 1> ka;

    auto c = python_class_type<h_t, op_base_t, py::smart_holder>(m, name)
               .def(py::init<>())
               .def("size", size_fn)
               .def("coefficient", coeff_fn)
               .def("get_operator", get_op_fn, ka)
               .def("emplace_back", emplace_back_fn)
               .def(py::self == py::self)
               .def(py::self != py::self);

    if constexpr(!std::is_same_v<h_t, CoreHamiltonian>) {
        auto core_hamiltonian_fn = [](const h_t& self) {
            return self.core_hamiltonian();
        };
        c.def("core_hamiltonian", core_hamiltonian_fn, ka);
    }

    if constexpr(std::is_same_v<h_t, Hamiltonian>) {
        auto electronic_hamiltonian_fn = [](const h_t& self) {
            return self.electronic_hamiltonian();
        };
        c.def("electronic_hamiltonian", electronic_hamiltonian_fn, ka);
    }
}

} // namespace detail_

void export_core_hamiltonian(python_module_reference m) {
    detail_::export_hamiltonian_<CoreHamiltonian>("CoreHamiltonian", m);
}

void export_electronic_hamiltonian(python_module_reference m) {
    detail_::export_hamiltonian_<ElectronicHamiltonian>("ElectronicHamiltonian",
                                                        m);
}

void export_hamiltonian(python_module_reference m) {
    detail_::export_hamiltonian_<Hamiltonian>("Hamiltonian", m);
}

} // namespace chemist::qm_operator
