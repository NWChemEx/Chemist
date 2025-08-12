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
#include <chemist/basis_set/ao_basis_set.hpp>
#include <pybind11/stl.h>

namespace chemist {
namespace detail_ {

template<typename T>
void export_ao_basis_set_(const char* name, python_module_reference m) {
    using primitive_type = basis_set::Primitive<T>;
    using cg_type        = basis_set::ContractedGaussian<primitive_type>;
    using shell_type     = basis_set::Shell<cg_type>;
    using abs_type       = basis_set::AtomicBasisSet<shell_type>;
    using aobs_type      = basis_set::AOBasisSet<abs_type>;
    using size_type      = typename abs_type::size_type;

    python_class_type<aobs_type>(m, name)
      .def(pybind11::init<>())
      .def("add_center", [](aobs_type& s, abs_type b) { s.add_center(b); })
      .def("shell_range",
           [](aobs_type& s, size_type i) { return s.shell_range(i); })
      .def("max_l", [](aobs_type& s) { return s.max_l(); })
      .def("n_shells", [](aobs_type& s) { return s.n_shells(); })
      .def("shell", [](aobs_type& s, size_type i) { return s.shell(i); })
      .def("primitive_range",
           [](aobs_type& s, size_type i) { return s.primitive_range(i); })
      .def("n_aos", [](aobs_type& s) { return s.n_aos(); })
      .def("n_primitives", [](aobs_type& s) { return s.n_primitives(); })
      .def("primitive",
           [](aobs_type& s, size_type i) { return s.primitive(i); })
      .def("empty", [](aobs_type& s) { return s.empty(); })
      .def("at", [](aobs_type& s, size_type i) { return s[i]; })
      .def("size", [](aobs_type& s) { return s.size(); })
      .def(pybind11::self + pybind11::self)
      .def(pybind11::self += pybind11::self)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_ao_basis_set(python_module_reference m) {
    detail_::export_ao_basis_set_<float>("AOBasisSetF", m);
    detail_::export_ao_basis_set_<double>("AOBasisSetD", m);
}

} // namespace chemist
