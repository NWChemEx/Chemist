#include "export_nucleus.hpp"
#include <chemist/nucleus/nucleus.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_nucleus(python_module_reference m) {
    using nucleus_type       = Nucleus;
    using nucleus_reference  = nucleus_type&;
    using point_charge_type  = typename Nucleus::point_charge_type;
    using name_type          = typename Nucleus::name_type;
    using atomic_number_type = typename Nucleus::atomic_number_type;
    using mass_type          = typename Nucleus::mass_type;
    using coord_type         = typename Nucleus::coord_type;
    using charge_type        = typename Nucleus::charge_type;

    python_class_type<nucleus_type, point_charge_type>(m, "Nucleus")
      .def(pybind11::init<>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type, coord_type,
                          coord_type, coord_type>())
      .def(pybind11::init<name_type, atomic_number_type, mass_type, coord_type,
                          coord_type, coord_type, charge_type>())
      .def_property(
        "name", [](nucleus_reference self) { return self.name(); },
        [](nucleus_reference self, name_type name) {
            self.name() = std::move(name);
        })
      .def_property(
        "Z", [](nucleus_reference self) { return self.Z(); },
        [](nucleus_reference self, atomic_number_type Z) { self.Z() = Z; })
      .def_property(
        "mass", [](nucleus_reference self) { return self.mass(); },
        [](nucleus_reference self, mass_type m) { self.mass() = m; })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);

    export_nucleus_view(m);
    export_nuclei(m);
}

} // namespace chemist
