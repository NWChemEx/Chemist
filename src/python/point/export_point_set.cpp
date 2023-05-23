/*
 * Copyright 2023 NWChemEx-Project
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

#include "export_point.hpp"
#include <chemist/point/point_set.hpp>

namespace chemist {
namespace detail_ {

template<typename T>
void export_point_set_(const char* name, python_module_reference m) {
    // N.B. We don't want to export the base class so we use lambdas to
    //      call the methods.

    using point_set_type      = PointSet<T>;
    using point_set_reference = point_set_type&;
    using value_type          = typename point_set_type::value_type;
    using reference           = typename point_set_type::reference;
    using size_type           = typename point_set_type::size_type;

    using at_fxn = reference (point_set_type::*)(size_type);

    python_class_type<point_set_type>(m, name)
      .def(pybind11::init<>())
      .def("empty", [](point_set_reference self) { return self.empty(); })
      .def("push_back",
           [](point_set_reference self, value_type v) {
               return self.push_back(std::move(v));
           })
      .def("at", [](point_set_reference self, size_type i) { return self[i]; })
      .def("size", [](point_set_reference self) { return self.size(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_point_set(python_module_reference m) {
    detail_::export_point_set_<float>("PointSetF", m);
    detail_::export_point_set_<double>("PointSetD", m);
}

} // namespace chemist
