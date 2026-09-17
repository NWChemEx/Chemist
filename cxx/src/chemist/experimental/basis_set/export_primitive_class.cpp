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

void export_primitive_class(python_module_reference m) {
    auto c =
      python_class_type<Primitive>(m, "Primitive")
        .def(py::init<>())
        .def(py::init<double, double, std::size_t, double, double, double>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_primitive_readers(c);
    detail_::add_primitive_writers(c);
}

} // namespace chemist::experimental
