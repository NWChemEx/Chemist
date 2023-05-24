#pragma once
#include "../pychemist.hpp"
#include <chemist/chemical_system/chemical_system.hpp>
#include <pybind11/operators.h>

namespace chemist {

void inline export_chemical_system(python_module_reference m) {
    using chemical_system_type      = ChemicalSystem;
    using chemical_system_reference = chemical_system_type&;
    using molecule_type             = typename chemical_system_type::molecule_t;

    // N.B. We only expose the parts of the ChemicalSystem's API which have
    //      been finalized (nelectrons/charge are moving to molecule) and the
    //      potential class is not finished.

    python_class_type<ChemicalSystem>(m, "ChemicalSystem")
      .def(pybind11::init<>())
      .def(pybind11::init<molecule_type>())
      .def_property(
        "molecule",
        [](chemical_system_reference self) { return self.molecule(); },
        [](chemical_system_reference self, molecule_type mol) {
            self.molecule() = std::move(mol);
        })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
