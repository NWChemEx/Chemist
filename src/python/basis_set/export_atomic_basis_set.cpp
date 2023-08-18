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
#include <chemist/basis_set2/atomic_basis_set/atomic_basis_set.hpp>
#include <pybind11/stl.h>

namespace chemist {
namespace detail_ {

template<typename T>
void export_atomic_basis_set_(const char* name, python_module_reference m) {
    using primitive_type        = basis_set::Primitive<T>;
    using cg_type               = basis_set::ContractedGaussian<primitive_type>;
    using shell_type            = basis_set::Shell<cg_type>;
    using abs_type              = basis_set::AtomicBasisSet<shell_type>;
    using center_type           = typename primitive_type::center_type;
    using coord_type            = typename primitive_type::coord_type;
    using pure_type             = typename shell_type::pure_type;
    using angular_momentum_type = typename shell_type::angular_momentum_type;
    using cg_ref_type           = typename shell_type::cg_reference;
    using size_type             = typename abs_type::size_type;
    using name_type             = typename abs_type::name_type;
    using atomic_number_type    = typename abs_type::atomic_number_type;
    using range_type            = typename abs_type::range_type;

    python_class_type<abs_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<name_type, atomic_number_type, coord_type, coord_type,
                          coord_type>())
      .def(pybind11::init<name_type, atomic_number_type, center_type>())
      .def(pybind11::init<coord_type, coord_type, coord_type>())
      .def(pybind11::init<center_type>())
      .def(pybind11::init<name_type, atomic_number_type>())
      .def(pybind11::init([](center_type r, std::vector<shell_type> ss) {
          return abs_type(r, ss.begin(), ss.end());
      }))
      .def(pybind11::init([](name_type n, atomic_number_type z, center_type r,
                             std::vector<shell_type> ss) {
          return abs_type(n, z, r, ss.begin(), ss.end());
      }))
      .def_property(
        "basis_set_name", [](abs_type& s) { return s.basis_set_name(); },
        [](abs_type& s, name_type n) { s.basis_set_name() = n; })
      .def_property(
        "atomic_number", [](abs_type& s) { return s.atomic_number(); },
        [](abs_type& s, atomic_number_type l) { s.atomic_number() = l; })
      .def_property(
        "center", [](abs_type& s) { return s.center(); },
        [](abs_type& s, center_type r) { s.center() = r; })
      .def("add_shell", [](abs_type& s, pure_type p, angular_momentum_type l,
                           cg_type cg) { s.add_shell(p, l, cg); })
      .def("n_aos", [](abs_type& s) { return s.n_aos(); })
      .def("n_primitives", [](abs_type& s) { return s.n_primitives(); })
      .def("primitive_range",
           [](abs_type& s, size_type i) { return s.primitive_range(i); })
      .def("primitive_to_shell",
           [](abs_type& s, size_type i) { return s.primitive_to_shell(i); })
      .def("primitive", [](abs_type& s, size_type i) { return s.primitive(i); })
      .def("empty", [](abs_type& s) { return s.empty(); })
      .def("at", [](abs_type& s, size_type i) { return s[i]; })
      .def("size", [](abs_type& s) { return s.size(); })
      .def("is_null", [](abs_type& s) { return s.is_null(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_atomic_basis_set(python_module_reference m) {
    detail_::export_atomic_basis_set_<float>("AtomicBasisSetF", m);
    detail_::export_atomic_basis_set_<double>("AtomicBasisSetD", m);
}

} // namespace chemist