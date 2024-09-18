#include "export_electron.hpp"
#include <chemist/chemical_system/electron/many_electrons.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_many_electrons(python_module_reference m) {
    using size_type = typename ManyElectrons::size_type;

    python_class_type<ManyElectrons>(m, "ManyElectrons")
      .def(pybind11::init<>())
      .def(pybind11::init<size_type>())
      .def("at", [](ManyElectrons& self, size_type i) { return self.at(i); })
      .def("size", [](ManyElectrons& self) { return self.size(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist