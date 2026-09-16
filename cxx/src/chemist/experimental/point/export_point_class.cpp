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

void export_point_class(python_module_reference m) {
    auto c = python_class_type<Point>(m, "Point")
               .def(py::init<>())
               .def(py::init<double, double, double>())
               .def("inner_product",
                    [](const Point& lhs, const Point& rhs) {
                        return detail_::to_py_float(lhs.inner_product(rhs));
                    })
               .def("__sub__", [](const Point& lhs,
                                  const Point& rhs) { return lhs - rhs; })
               .def(py::self == py::self)
               .def(py::self != py::self);

    detail_::add_point_readers(c);
    detail_::add_point_writers(c);
}

} // namespace chemist::experimental
