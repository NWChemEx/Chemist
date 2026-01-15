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
#include <chemist/quantum_mechanics/wavefunction/wavefunction.hpp>
#include <pybind11/operators.h>

namespace chemist::wavefunction {
namespace detail_ {

template<typename T>
void export_transformed_(const char* name, python_module_reference m) {
    using transformed_t = Transformed<T>;
    using from_space_t  = typename transformed_t::from_space_type;
    using transform_t   = typename transformed_t::transform_type;

    // Property getters and setters
    auto get_from_space = [](transformed_t& t) { return t.from_space(); };
    auto set_from_space = [](transformed_t& t, from_space_t& s) {
        t.from_space() = s;
    };
    auto get_transform = [](transformed_t& t) { return t.transform(); };
    auto set_transform = [](transformed_t& t, transform_t& c) {
        t.transform() = c;
    };

    // Ensure that tensorwrapper is imported
    python_module_type::import("tensorwrapper");

    python_class_type<transformed_t, VectorSpace>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<from_space_t, transform_t>())
      .def_property("from_space", get_from_space, set_from_space)
      .def_property("transform", get_transform, set_transform)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}
} // namespace detail_

void export_transformed(python_module_reference m) {
    detail_::export_transformed_<AOs>("TransformedAOs", m);
}

} // namespace chemist::wavefunction
