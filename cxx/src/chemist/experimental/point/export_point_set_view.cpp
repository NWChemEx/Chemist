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

void export_point_set_view(python_module_reference m) {
    // -- The mutable view ---------------------------------------------------
    auto mutable_view =
      python_class_type<point_set_view>(m, "PointSetView")
        .def(py::init<>())
        .def(py::init<PointSet&>(), py::keep_alive<1, 2>())
        .def("as_point_set", &point_set_view::as_point_set)
        .def(
          "__getitem__",
          [](point_set_view& s, std::size_t i) { return s.at(i); },
          py::keep_alive<0, 1>())
        .def("__setitem__", [](point_set_view& s, std::size_t i,
                               const Point& p) { s.at(i) = p; })
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_set_readers(mutable_view);

    // -- The read-only view --------------------------------------------------
    auto immutable_view =
      python_class_type<const_point_set_view>(m, "ImmutablePointSetView")
        .def(py::init<>())
        .def(py::init<const PointSet&>(), py::keep_alive<1, 2>())
        .def(py::init<const point_set_view&>(), py::keep_alive<1, 2>())
        .def("as_point_set", &const_point_set_view::as_point_set)
        .def(
          "__getitem__",
          [](const const_point_set_view& s, std::size_t i) { return s.at(i); },
          py::keep_alive<0, 1>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_set_readers(immutable_view);

    // Mirrors the implicit conversions the C++ classes provide.
    py::implicitly_convertible<PointSet, const_point_set_view>();
    py::implicitly_convertible<point_set_view, const_point_set_view>();
    py::implicitly_convertible<PointSet, point_set_view>();
}

} // namespace chemist::experimental
