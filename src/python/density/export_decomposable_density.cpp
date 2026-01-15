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
#include <chemist/density/decomposable_density.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_decomposable_density(python_module_reference m) {
    using decomposable_density_t = DecomposableDensity<Electron>;
    using density_t              = Density<Electron>;
    using basis_t                = typename density_t::basis_type;
    using value_t                = typename decomposable_density_t::value_type;
    using transformed_basis_t =
      typename decomposable_density_t::transformed_basis_type;

    python_class_type<decomposable_density_t, density_t>(m,
                                                         "DecomposableDensity")
      .def(pybind11::init<>())
      .def(pybind11::init<value_t, transformed_basis_t>())
      .def(pybind11::init<value_t, basis_t, value_t>())
      .def_property_readonly(
        "left_factor",
        [](decomposable_density_t& d) { return d.left_factor(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
