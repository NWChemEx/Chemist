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
#include <chemist/quantum_mechanics/wavefunction/mos.hpp>

namespace chemist::wavefunction {

void export_mos(python_module_reference m) {
    using from_space_t = typename MOs::from_space_type;
    using transform_t  = typename MOs::transform_type;

    auto eq_fn = [](const MOs& self, const VectorSpace& rhs) {
        return self.are_equal(rhs);
    };
    auto ne_fn = [](const MOs& self, const VectorSpace& rhs) {
        return self.are_different(rhs);
    };

    // Ensure that tensorwrapper is imported
    python_module_type::import("tensorwrapper");

    // N.B. MOs adds no state to Transformed<AOs>; it is a strong type. State
    // properties are inherited from the base binding, but equality is
    // bound here via are_equal so that the strong type is respected --
    // the inherited operator== would report MOs(a, c) equal to a plain
    // TransformedAOs(a, c).
    python_class_type<MOs, Transformed<AOs>>(m, "MOs")
      .def(py::init<>())
      .def(py::init<from_space_t, transform_t>())
      .def("__eq__", eq_fn)
      .def("__ne__", ne_fn);
}

} // namespace chemist::wavefunction
