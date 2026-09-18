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

void export_primitive_view(python_module_reference m) {
    // -- The mutable view ---------------------------------------------------
    auto mutable_view =
      python_class_type<primitive_view>(m, "PrimitiveView")
        .def(py::init<Primitive&>(), py::keep_alive<1, 2>())
        .def("as_primitive", &primitive_view::as_primitive)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__eq__", [](const primitive_view& lhs,
                          const Primitive& rhs) { return lhs == rhs; })
        .def("__ne__", [](const primitive_view& lhs, const Primitive& rhs) {
            return lhs != rhs;
        });

    detail_::add_primitive_readers(mutable_view);
    detail_::add_primitive_writers(mutable_view);

    // -- The read-only view --------------------------------------------------
    auto immutable_view =
      python_class_type<const_primitive_view>(m, "ImmutablePrimitiveView")
        .def(py::init<const Primitive&>(), py::keep_alive<1, 2>())
        .def(py::init<const primitive_view&>(), py::keep_alive<1, 2>())
        .def("as_primitive", &const_primitive_view::as_primitive)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__eq__", [](const const_primitive_view& lhs,
                          const Primitive& rhs) { return lhs == rhs; })
        .def("__ne__", [](const const_primitive_view& lhs,
                          const Primitive& rhs) { return lhs != rhs; });

    detail_::add_primitive_readers(immutable_view);

    // A Primitive and a mutable view are usable wherever a read-only view is,
    // on the C++ side, because both convert to one implicitly. Registering
    // those conversions here gives Python the same behavior.
    py::implicitly_convertible<Primitive, const_primitive_view>();
    py::implicitly_convertible<primitive_view, const_primitive_view>();
    py::implicitly_convertible<Primitive, primitive_view>();
}

} // namespace chemist::experimental
