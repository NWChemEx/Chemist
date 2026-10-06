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

#include "../export_experimental.hpp"

namespace chemist::experimental {

void export_spherical_ao_view(python_module_reference m) {
    using magnetic_index_type =
      typename const_spherical_ao_view::magnetic_index_type;
    using shell_type      = typename const_spherical_ao_view::shell_type;
    using shell_view_type = typename const_spherical_ao_view::shell_view_type;

    // Only a read-only view of a SphericalAO exists in C++, so there is no
    // mutable counterpart to bind.
    auto immutable_view =
      python_class_type<const_spherical_ao_view, AOView>(
        m, "ImmutableSphericalAOView")
        .def(py::init<const SphericalAO&>(), py::keep_alive<1, 2>())
        .def(py::init<const shell_type&, magnetic_index_type>(),
             py::keep_alive<1, 2>())
        .def(py::init<const shell_view_type&, magnetic_index_type>(),
             py::keep_alive<1, 2>())
        .def("as_spherical_ao", &const_spherical_ao_view::as_spherical_ao)
        // A clone, rather than the stored shell view itself, so that what
        // Python holds can not dangle; see add_cca_shell_readers.
        .def(
          "get_cartesian_shell",
          [](const const_spherical_ao_view& ao) {
              return ao.get_cartesian_shell().clone();
          },
          py::keep_alive<0, 1>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_comparisons_with<SphericalAO>(immutable_view);
    detail_::add_spherical_ao_readers(immutable_view);

    // Mirrors the implicit conversion the C++ class provides.
    py::implicitly_convertible<SphericalAO, const_spherical_ao_view>();
}

} // namespace chemist::experimental
