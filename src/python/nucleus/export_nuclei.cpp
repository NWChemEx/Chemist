#include "export_nucleus.hpp"
#include <chemist/nucleus/nuclei.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_nuclei(python_module_reference m) {
        // N.B. We don't want to export the base class so we use lambdas to
    //      call the methods.

    using nuclei_type      = Nuclei;
    using nuclei_reference = nuclei_type&;
    using value_type        = typename nuclei_type::value_type;
    using reference         = typename nuclei_type::reference;
    using size_type         = typename nuclei_type::size_type;

    using at_fxn = reference (nuclei_type::*)(size_type);

    python_class_type<nuclei_type>(m, "Nuclei")
      .def(pybind11::init<>())
      .def("empty", [](nuclei_reference self) { return self.empty(); })
      .def("push_back",
           [](nuclei_reference self, value_type v) {
               return self.push_back(std::move(v));
           })
      .def("at", [](nuclei_reference self, size_type i) { return self[i]; })
      .def("size", [](nuclei_reference self) { return self.size(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
