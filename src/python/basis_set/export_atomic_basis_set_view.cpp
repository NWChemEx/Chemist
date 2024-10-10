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
#include <chemist/basis_set/atomic_basis_set_view.hpp>
#include <pybind11/stl.h>

namespace chemist {
namespace detail_ {

template<typename T>
void export_abs_view_(const char* name, python_module_reference m) {
    using primitive_type     = basis_set::Primitive<T>;
    using cg_type            = basis_set::ContractedGaussian<primitive_type>;
    using shell_type         = basis_set::Shell<cg_type>;
    using abs_type           = basis_set::AtomicBasisSet<shell_type>;
    using abs_view_type      = basis_set::AtomicBasisSetView<abs_type>;
    using center_type        = typename primitive_type::center_type;
    using size_type          = typename abs_type::size_type;
    using name_type          = typename abs_type::name_type;
    using atomic_number_type = typename abs_type::atomic_number_type;

    python_class_type<abs_view_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<abs_type&>())
      .def_property(
        "basis_set_name", [](abs_view_type& s) { return s.basis_set_name(); },
        [](abs_view_type& s, name_type n) { s.basis_set_name() = n; })
      .def_property(
        "atomic_number", [](abs_view_type& s) { return s.atomic_number(); },
        [](abs_view_type& s, atomic_number_type l) { s.atomic_number() = l; })
      .def_property(
        "center", [](abs_view_type& s) { return s.center(); },
        [](abs_view_type& s, center_type r) { s.center() = r; })
      .def("n_aos", [](abs_view_type& s) { return s.n_aos(); })
      .def("n_primitives", [](abs_view_type& s) { return s.n_primitives(); })
      .def("primitive_range",
           [](abs_view_type& s, size_type i) { return s.primitive_range(i); })
      .def(
        "primitive_to_shell",
        [](abs_view_type& s, size_type i) { return s.primitive_to_shell(i); })
      .def("primitive",
           [](abs_view_type& s, size_type i) { return s.primitive(i); })
      .def("empty", [](abs_view_type& s) { return s.empty(); })
      .def("at", [](abs_view_type& s, size_type i) { return s[i]; })
      .def("size", [](abs_view_type& s) { return s.size(); })
      .def("is_null", [](abs_view_type& s) { return s.is_null(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self == abs_type())
      .def(abs_type() == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def(pybind11::self != abs_type())
      .def(abs_type() != pybind11::self);
}

} // namespace detail_

void export_atomic_basis_set_view(python_module_reference m) {
    detail_::export_abs_view_<float>("AtomicBasisSetViewF", m);
    detail_::export_abs_view_<double>("AtomicBasisSetViewD", m);
}

} // namespace chemist