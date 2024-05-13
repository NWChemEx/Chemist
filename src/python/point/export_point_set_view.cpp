/*
 * Copyright 2024 NWChemEx
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
#include <chemist/point/point_set_view/point_set_view.hpp>

namespace chemist {
namespace detail_ {
template<typename T>
void export_point_set_view_(const char* name, python_module_reference m) {
    using point_set_view_type = PointSetView<PointSet<T>>;
    using point_set_type      = typename point_set_view_type::point_set_type;
    using reference           = point_set_view_type&;
    using size_type           = typename point_set_view_type::size_type;

    python_class_type<point_set_view_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<point_set_type&>())
      .def("empty", [](reference self) { return self.empty(); })
      .def("at", [](reference self, size_type i) { return self[i]; })
      .def("size", [](reference self) { return self.size(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_point_set_view(python_module_reference m) {
    detail_::export_point_set_view_<float>("PointSetViewF", m);
    detail_::export_point_set_view_<double>("PointSetViewD", m);
}

} // namespace chemist
