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

#include "../pychemist.hpp"
#include "chemist/orbital_space/ao_space.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include "export_ao_space.hpp"
#include <pybind11/operators.h>

namespace chemist {
namespace detail_ {

template<typename T>
void export_ao_space_(const char* name, python_module_reference m) {
    using aos_type   = chemist::orbital_space::AOSpace<T>;
    using basis_type = typename aos_type::basis_type;
    using size_type  = typename aos_type::size_type;

    python_class_type<aos_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<basis_type>()) // Constructor with basis_type argument
      .def("size", &aos_type::size)
      .def_property(
        "basis_set",
        [](aos_type& self) -> basis_type& { return self.basis_set(); },
        [](aos_type& self, basis_type& basis) {
            self.basis_set() = std::move(basis);
        })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_
void export_ao_space(python_module_reference m) {
    detail_::export_ao_space_<basis_set::AOBasisSetD>("AOSpaceD", m);
    detail_::export_ao_space_<basis_set::AOBasisSetF>("AOSpaceF", m);
}

} // namespace chemist
