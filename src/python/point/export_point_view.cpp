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
#include <chemist/point/point_view.hpp>
#include <pybind11/operators.h>

namespace chemist {
namespace detail_ {

template<typename T>
inline void export_point_view_(const char* name, python_module_reference m) {
    using point_type      = Point<T>;
    using point_view_type = PointView<point_type>;
    using coord_type      = typename point_view_type::coord_type;
    using coord_reference = typename point_view_type::coord_reference;
    using size_type       = typename point_view_type::size_type;

    using coord_fxn = coord_reference (point_view_type::*)(size_type);

    python_class_type<point_view_type>(m, name)
      .def(pybind11::init<point_type&>())
      .def("coord", static_cast<coord_fxn>(&point_view_type::coord))
      .def_property(
        "x", [](point_view_type& p) { return p.x(); },
        [](point_view_type& p, coord_type x) { p.x() = x; })
      .def_property(
        "y", [](point_view_type& p) { return p.y(); },
        [](point_view_type& p, coord_type y) { p.y() = y; })
      .def_property(
        "z", [](point_view_type& p) { return p.z(); },
        [](point_view_type& p, coord_type z) { p.z() = z; })
      .def("magnitude", &point_view_type::magnitude)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self == point_type())
      .def(point_type() == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def(pybind11::self != point_type());
}

} // namespace detail_

void export_point_view(python_module_reference m) {
    detail_::export_point_view_<float>("PointViewF", m);
    detail_::export_point_view_<double>("PointViewD", m);
}

} // namespace chemist
