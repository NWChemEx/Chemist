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

#include "export_density.hpp"
#include <chemist/density/density_class.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_density_class(python_module_reference m) {
    using density_t = Density<Electron>;
    using basis_t   = typename density_t::basis_type;
    using value_t   = typename density_t::value_type;

    // Property getters and setters
    auto get_value = [](density_t& d) { return d.value(); };
    auto set_value = [](density_t& d, value_t& v) { d.value() = v; };
    auto get_basis = [](density_t& d) { return d.basis_set(); };
    auto set_basis = [](density_t& d, basis_t& bs) { d.basis_set() = bs; };

    // Ensure that tensorwrapper is imported
    python_module_type::import("tensorwrapper");

    python_class_type<density_t>(m, "Density")
      .def(pybind11::init<>())
      .def(pybind11::init<value_t, basis_t>())
      .def_property("value", get_value, set_value)
      .def_property("basis_set", get_basis, set_basis)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
