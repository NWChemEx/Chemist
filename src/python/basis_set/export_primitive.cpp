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

#include "export_basis_set.hpp"
#include <chemist/basis_set/primitive.hpp>

namespace chemist {
namespace detail_ {

template<typename T>
void export_primitive_(const char* name, python_module_reference m) {
    using primitive_type = basis_set::Primitive<T>;
    using coeff_type     = typename primitive_type::coefficient_type;
    using expo_type      = typename primitive_type::exponent_type;
    using center_type    = typename primitive_type::center_type;
    using coord_type     = typename primitive_type::coord_type;

    python_class_type<primitive_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<coeff_type, expo_type, coord_type, coord_type,
                          coord_type>())
      .def(pybind11::init<coeff_type, expo_type, center_type>())
      .def_property(
        "center", [](primitive_type& p) { return p.center(); },
        [](primitive_type& p, center_type c) { p.center() = c; })
      .def_property(
        "coefficient", [](primitive_type& p) { return p.coefficient(); },
        [](primitive_type& p, coeff_type c) { p.coefficient() = c; })
      .def_property(
        "exponent", [](primitive_type& p) { return p.exponent(); },
        [](primitive_type& p, expo_type e) { p.exponent() = e; })
      .def("is_null", [](primitive_type& p) { return p.is_null(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_primitive(python_module_reference m) {
    detail_::export_primitive_<float>("PrimitiveF", m);
    detail_::export_primitive_<double>("PrimitiveD", m);
}

} // namespace chemist