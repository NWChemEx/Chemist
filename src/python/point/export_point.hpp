#pragma once
#include "../pychemist.hpp"
#include <chemist/point/point.hpp>
#include <pybind11/operators.h>

namespace chemist {
namespace detail_ {

template<typename T>
inline void export_point_(const char* name, python_module_reference m) {
    using point_type      = Point<T>;
    using coord_type      = typename point_type::coord_type;
    using coord_reference = typename point_type::coord_reference;
    using size_type       = typename point_type::size_type;

    using coord_fxn = coord_reference (point_type::*)(size_type);

    python_class_type<point_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<coord_type, coord_type, coord_type>())
      .def("coord", static_cast<coord_fxn>(&point_type::coord))
      .def_property(
        "x", [](point_type& p) { return p.x(); },
        [](point_type& p, coord_type x) { p.x() = x; })
      .def_property(
        "y", [](point_type& p) { return p.y(); },
        [](point_type& p, coord_type y) { p.y() = y; })
      .def_property(
        "z", [](point_type& p) { return p.z(); },
        [](point_type& p, coord_type z) { p.z() = z; })
      .def("magnitude", &point_type::magnitude)
      .def(pybind11::self - pybind11::self)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_point_set(python_module_reference m);
void export_point_view(python_module_reference m);

inline void export_point(python_module_reference m) {
    detail_::export_point_<float>("PointF", m);
    detail_::export_point_<double>("PointD", m);

    export_point_view(m);
    export_point_set(m);
}

} // namespace chemist
