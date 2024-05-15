#include "export_fragmenting.hpp"
#include <chemist/fragmenting/fragmented_chemical_system.hpp>
#include <pybind11/operators.h>

namespace chemist::fragmenting {

void export_fragmented_chemical_system(python_module_reference m) {
    // using fragmented_molecule_type = FragmentedMolecule<Molecule>;
    // using fragmented_nuclei_type =
    //   typename fragmented_molecule_type::fragmented_nuclei_type;
    // using supersystem_type =
    //   typename fragmented_molecule_type::supersystem_type;
    // using reference   = fragmented_molecule_type&;
    // using size_type   = typename fragmented_molecule_type::size_type;
    // using charge_type = typename fragmented_molecule_type::charge_type;
    // using multiplicity_type =
    //   typename fragmented_molecule_type::multiplicity_type;

    // pybind11::keep_alive<0, 1> ka;

    // auto value_ctor = [](fragmented_nuclei_type nuclear_frags,
    //                      charge_type charge, multiplicity_type multiplicity,
    //                      pybind11::list py_charges,
    //                      pybind11::list py_multiplicities) {
    //     auto charges = pybind_to_buffer<charge_type>(py_charges);
    //     auto multiplicities =
    //       pybind_to_buffer<multiplicity_type>(py_multiplicities);
    //     return fragmented_molecule_type(std::move(nuclear_frags), charge,
    //                                     multiplicity, std::move(charges),
    //                                     std::move(multiplicities));
    // };

    // auto supersystem = [](reference self) { return self.supersystem(); };
    // auto at_fxn      = [](reference self, size_type i) { return self.at(i);
    // }; auto iter_fxn    = [](reference self) {
    //     return pybind11::make_iterator(self.begin(), self.end());
    // };

    // python_class_type<fragmented_molecule_type>(m, "FragmentedMolecule")
    //   .def(pybind11::init<>())
    //   .def(pybind11::init<supersystem_type>())
    //   .def(pybind11::init<fragmented_nuclei_type, charge_type,
    //                       multiplicity_type>())
    //   .def(pybind11::init(value_ctor))
    //   .def("supersystem", supersystem, ka)
    //   .def("empty", [](reference self) { return self.empty(); })
    //   .def("at", at_fxn, ka)
    //   .def("size", [](reference self) { return self.size(); })
    //   .def("__iter__", iter_fxn, ka)
    //   .def(pybind11::self == pybind11::self)
    //   .def(pybind11::self != pybind11::self);
}

} // namespace chemist::fragmenting
