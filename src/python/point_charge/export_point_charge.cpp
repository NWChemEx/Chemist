/*
 * Copyright 2023 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "export_point_charge.hpp"
#include <pybind11/operators.h>

namespace chemist {
namespace detail_ {

template<typename T>
void export_point_charge_(const char* name, python_module_reference m) {
    using point_charge_type = PointCharge<T>;
    using point_type        = typename point_charge_type::point_type;
    using charge_type       = typename point_charge_type::charge_type;
    using coord_type        = typename point_charge_type::coord_type;

    python_class_type<point_charge_type, point_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<charge_type, coord_type, coord_type, coord_type>())
      .def_property(
        "charge", [](point_charge_type& self) { return self.charge(); },
        [](point_charge_type& self, charge_type q) { self.charge() = q; })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_point_charge(python_module_reference m) {
    detail_::export_point_charge_<float>("PointChargeF", m);
    detail_::export_point_charge_<double>("PointChargeD", m);

    export_charge_view(m);
    export_charges(m);
}

} // namespace chemist
