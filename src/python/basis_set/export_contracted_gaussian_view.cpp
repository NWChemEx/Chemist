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
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp>

namespace chemist {
namespace detail_ {

template<typename T>
void export_cg_view_(const char* name, python_module_reference m) {
    using primitive_type = basis_set::Primitive<T>;
    using center_type    = typename primitive_type::center_type;
    using cg_type        = basis_set::ContractedGaussian<primitive_type>;
    using cg_view_type   = basis_set::ContractedGaussianView<cg_type>;
    using size_type      = typename cg_type::size_type;

    python_class_type<cg_view_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<cg_type&>())
      .def("empty", [](cg_view_type& cg) { return cg.empty(); })
      .def("at", [](cg_view_type& cg, size_type i) { return cg[i]; })
      .def("size", [](cg_view_type& cg) { return cg.size(); })
      .def_property(
        "center", [](cg_view_type& cg) { return cg.center(); },
        [](cg_view_type& cg, center_type c) { cg.center() = c; })
      .def("is_null", [](cg_view_type& cg) { return cg.is_null(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self == cg_type())
      .def(cg_type() == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def(pybind11::self != cg_type())
      .def(cg_type() != pybind11::self);
}

} // namespace detail_

void export_contracted_gaussian_view(python_module_reference m) {
    detail_::export_cg_view_<float>("ContractedGaussianViewF", m);
    detail_::export_cg_view_<double>("ContractedGaussianViewD", m);
}

} // namespace chemist