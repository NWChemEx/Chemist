#include "export_electron.hpp"
#include <chemist/chemical_system/electron/electron_class.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_electron_class(python_module_reference m) {
    python_class_type<Electron>(m, "Electron")
      .def(pybind11::init<>())
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist