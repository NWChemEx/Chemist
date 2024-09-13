#include "export_wavefunction.hpp"
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <pybind11/operators.h>

namespace chemist::wavefunction {

void export_aos(python_module_reference m) {
    using aos_reference = AOs&;
    using ao_basis_set  = typename AOs::ao_basis_type;

    auto get_aos = [](aos_reference self) { return self.ao_basis_set(); };
    auto set_aos = [](aos_reference self, ao_basis_set& aos) {
        self.ao_basis_set() = aos;
    };

    python_class_type<AOs, VectorSpace>(m, "AOs")
      .def(pybind11::init<>())
      .def(pybind11::init<ao_basis_set>())
      .def_property("ao_basis_set", get_aos, set_aos)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist::wavefunction
