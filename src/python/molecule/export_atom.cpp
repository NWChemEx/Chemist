#include "export_molecule.hpp"
#include <chemist/molecule/atom.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_atom(python_module_reference m) {
    using atom_type          = Atom;
    using atom_reference     = atom_type&;
    using name_type          = typename atom_type::name_type;
    using atomic_number_type = typename atom_type::atomic_number_type;
    using mass_type          = typename atom_type::mass_type;
    using coord_type         = typename atom_type::coord_type;
    using coord_reference    = typename atom_type::coord_reference;
    using charge_type        = typename atom_type::charge_type;
    using nucleus_type       = typename atom_type::nucleus_type;
    using size_type          = typename atom_type::size_type;

    using coord_fxn = coord_reference (atom_type::*)(size_type);

    python_class_type<atom_type>(m, "Atom")
      .def(pybind11::init<>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type, coord_type,
                          coord_type, coord_type>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type, coord_type,
                          coord_type, coord_type, charge_type>())
      .def_property(
        "name", [](atom_reference self) { return self.name(); },
        [](atom_reference self, name_type name) { self.name() = name; })
      .def_property(
        "nucleus", [](atom_reference self) { return self.nucleus(); },
        [](atom_reference self, nucleus_type nuke) {
            self.nucleus() = std::move(nuke);
        })
      .def_property(
        "Z", [](atom_reference self) { return self.Z(); },
        [](atom_reference self, atomic_number_type Z) { self.Z() = Z; })
      .def_property(
        "mass", [](atom_reference self) { return self.mass(); },
        [](atom_reference self, mass_type m) { self.mass() = m; })
      .def_property(
        "charge", [](atom_reference self) { return self.charge(); },
        [](atom_reference self, charge_type q) { self.charge() = q; })
      .def("coord", static_cast<coord_fxn>(&Atom::coord))
      .def_property(
        "x", [](atom_reference self) { return self.x(); },
        [](atom_reference self, coord_type x) { self.x() = x; })
      .def_property(
        "y", [](atom_reference self) { return self.y(); },
        [](atom_reference self, coord_type y) { self.y() = y; })
      .def_property(
        "z", [](atom_reference self) { return self.z(); },
        [](atom_reference self, coord_type z) { self.z() = z; })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
