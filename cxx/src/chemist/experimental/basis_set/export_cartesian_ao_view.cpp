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

void export_cartesian_ao_view(python_module_reference m) {
    using size_type = std::size_t;

    // -- The mutable view ---------------------------------------------------
    auto mutable_view =
      python_class_type<cartesian_ao_view, AOView>(m, "CartesianAOView")
        .def(py::init<CartesianAO&>(), py::keep_alive<1, 2>())
        .def(
          py::init<contracted_gaussian_view, size_type, size_type, size_type>(),
          py::keep_alive<1, 2>())
        .def("as_cartesian_ao", &cartesian_ao_view::as_cartesian_ao)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_comparisons_with<CartesianAO>(mutable_view);
    detail_::add_cartesian_ao_readers(mutable_view);
    detail_::add_cartesian_ao_writers(mutable_view);

    // -- The read-only view --------------------------------------------------
    auto immutable_view =
      python_class_type<const_cartesian_ao_view, AOView>(
        m, "ImmutableCartesianAOView")
        .def(py::init<const CartesianAO&>(), py::keep_alive<1, 2>())
        .def(py::init<const cartesian_ao_view&>(), py::keep_alive<1, 2>())
        .def(py::init<const_contracted_gaussian_view, size_type, size_type,
                      size_type>(),
             py::keep_alive<1, 2>())
        .def("as_cartesian_ao", &const_cartesian_ao_view::as_cartesian_ao)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_comparisons_with<CartesianAO>(immutable_view);
    detail_::add_cartesian_ao_readers(immutable_view);

    // Mirrors the implicit conversions the C++ classes provide.
    py::implicitly_convertible<CartesianAO, const_cartesian_ao_view>();
    py::implicitly_convertible<cartesian_ao_view, const_cartesian_ao_view>();
    py::implicitly_convertible<CartesianAO, cartesian_ao_view>();
}

} // namespace chemist::experimental
