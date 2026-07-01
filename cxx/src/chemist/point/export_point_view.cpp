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

namespace chemist {

namespace detail_ {

template<typename T>
inline void export_mutable_(const char* name, python_module_reference m) {
    using point_type      = Point<T>;
    using point_view_type = PointView<point_type>;
    using coord_type      = typename point_view_type::coord_type;
    using coord_reference = typename point_view_type::coord_reference;
    using size_type       = typename point_view_type::size_type;

    using coord_fxn = coord_reference (point_view_type::*)(size_type);

    python_class_type<point_view_type>(m, name)
      .def(py::init<point_type&>())
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
      .def(py::self == py::self)
      .def(py::self == point_type())
      .def(point_type() == py::self)
      .def(py::self != py::self)
      .def(py::self != point_type());
}

template<typename T>
inline void export_immutable_(const char* name, python_module_reference m) {
    using point_type      = Point<T>;
    using point_view_type = PointView<const point_type>;
    using coord_reference = typename point_view_type::coord_reference;
    using size_type       = typename point_view_type::size_type;

    using coord_fxn = coord_reference (point_view_type::*)(size_type);

    python_class_type<point_view_type>(m, name)
      .def(py::init<point_type&>())
      .def("coord", static_cast<coord_fxn>(&point_view_type::coord))
      .def_property_readonly("x", [](point_view_type& p) { return p.x(); })
      .def_property_readonly("y", [](point_view_type& p) { return p.y(); })
      .def_property_readonly("z", [](point_view_type& p) { return p.z(); })
      .def("magnitude", &point_view_type::magnitude)
      .def(py::self == py::self)
      .def(py::self == point_type())
      .def(point_type() == py::self)
      .def(py::self != py::self)
      .def(py::self != point_type());
}

} // namespace detail_

void export_point_view(python_module_reference m) {
    detail_::export_mutable_<float>("PointViewF", m);
    detail_::export_mutable_<double>("PointViewD", m);
    detail_::export_immutable_<float>("ImmutablePointViewF", m);
    detail_::export_immutable_<double>("ImmutablePointViewD", m);
}

} // namespace chemist
