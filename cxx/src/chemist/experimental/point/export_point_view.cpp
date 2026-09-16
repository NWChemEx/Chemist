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

void export_point_view(python_module_reference m) {
    // -- The mutable view ---------------------------------------------------
    auto mutable_view =
      python_class_type<point_view>(m, "PointView")
        .def(py::init<Point&>(), py::keep_alive<1, 2>())
        .def("as_point", &point_view::as_point)
        .def("inner_product",
             [](const point_view& lhs, const point_view& rhs) {
                 return detail_::to_py_float(lhs.inner_product(rhs));
             })
        .def("inner_product",
             [](const point_view& lhs, const Point& rhs) {
                 return detail_::to_py_float(lhs.inner_product(rhs));
             })
        .def("__sub__", [](const point_view& lhs,
                           const point_view& rhs) { return lhs - rhs; })
        .def("__sub__",
             [](const point_view& lhs, const Point& rhs) { return lhs - rhs; })
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__eq__",
             [](const point_view& lhs, const Point& rhs) { return lhs == rhs; })
        .def("__ne__", [](const point_view& lhs, const Point& rhs) {
            return lhs != rhs;
        });

    detail_::add_point_readers(mutable_view);
    detail_::add_point_writers(mutable_view);

    // -- The read-only view --------------------------------------------------
    auto immutable_view =
      python_class_type<const_point_view>(m, "ImmutablePointView")
        .def(py::init<const Point&>(), py::keep_alive<1, 2>())
        .def(py::init<const point_view&>(), py::keep_alive<1, 2>())
        .def("as_point", &const_point_view::as_point)
        .def("inner_product",
             [](const const_point_view& lhs, const const_point_view& rhs) {
                 return detail_::to_py_float(lhs.inner_product(rhs));
             })
        .def("inner_product",
             [](const const_point_view& lhs, const Point& rhs) {
                 return detail_::to_py_float(lhs.inner_product(rhs));
             })
        .def("__sub__", [](const const_point_view& lhs,
                           const const_point_view& rhs) { return lhs - rhs; })
        .def("__sub__", [](const const_point_view& lhs,
                           const Point& rhs) { return lhs - rhs; })
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__eq__", [](const const_point_view& lhs,
                          const Point& rhs) { return lhs == rhs; })
        .def("__ne__", [](const const_point_view& lhs, const Point& rhs) {
            return lhs != rhs;
        });

    detail_::add_point_readers(immutable_view);

    // A Point and a mutable view are usable wherever a read-only view is, on
    // the C++ side, because both convert to one implicitly. Registering those
    // conversions here gives Python the same behavior.
    py::implicitly_convertible<Point, const_point_view>();
    py::implicitly_convertible<point_view, const_point_view>();
    py::implicitly_convertible<Point, point_view>();
}

} // namespace chemist::experimental
