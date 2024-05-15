#include "export_fragmenting.hpp"
#include <chemist/fragmenting/fragmented_chemical_system.hpp>
#include <pybind11/operators.h>

namespace chemist::fragmenting {

void export_fragmented_chemical_system(python_module_reference m) {
    using fragmented_chemical_system = FragmentedChemicalSystem<ChemicalSystem>;
    using fragmented_molecule_type =
      typename fragmented_chemical_system::fragmented_molecule_type;
    using supersystem_type =
      typename fragmented_chemical_system::supersystem_type;
    using reference = fragmented_chemical_system&;
    using size_type = typename fragmented_chemical_system::size_type;

    auto supersystem = [](reference self) { return self.supersystem(); };
    auto at_fxn      = [](reference self, size_type i) { return self.at(i); };
    auto iter_fxn    = [](reference self) {
        return pybind11::make_iterator(self.begin(), self.end());
    };

    pybind11::keep_alive<0, 1> ka;

    python_class_type<fragmented_chemical_system>(m, "FragmentedChemicalSystem")
      .def(pybind11::init<>())
      .def(pybind11::init<fragmented_molecule_type>())
      .def("supersystem", supersystem, ka)
      .def("empty", [](reference self) { return self.empty(); })
      .def("at", at_fxn, ka)
      .def("size", [](reference self) { return self.size(); })
      .def("__iter__", iter_fxn, ka)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist::fragmenting
