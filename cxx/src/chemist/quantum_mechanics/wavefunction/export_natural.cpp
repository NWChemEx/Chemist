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

#include "export_wavefunction.hpp"
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <chemist/quantum_mechanics/wavefunction/natural.hpp>

namespace chemist::wavefunction {

namespace detail_ {

template<typename T>
void export_natural_(const char* name, python_module_reference m) {
    using natural_t    = Natural<T>;
    using from_space_t = typename natural_t::from_space_type;
    using transform_t  = typename natural_t::transform_type;
    using tensor_t     = typename natural_t::tensor_type;

    auto get_matrix = [](natural_t& n) { return n.diagonalized_matrix(); };
    auto set_matrix = [](natural_t& n, tensor_t& d) {
        n.diagonalized_matrix() = d;
    };

    // Ensure that tensorwrapper is imported
    python_module_type::import("tensorwrapper");

    python_class_type<natural_t, Transformed<T>>(m, name)
      .def(py::init<>())
      .def(py::init<tensor_t, from_space_t, transform_t>())
      .def_property("diagonalized_matrix", get_matrix, set_matrix)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace detail_

void export_natural(python_module_reference m) {
    detail_::export_natural_<AOs>("NaturalAOs", m);
}

} // namespace chemist::wavefunction
