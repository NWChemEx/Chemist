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
