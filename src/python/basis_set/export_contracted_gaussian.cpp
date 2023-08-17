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
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian.hpp>
#include <pybind11/stl.h>

namespace chemist {
namespace detail_ {

template<typename T>
void export_contracted_gaussian_(const char* name, python_module_reference m) {
    using primitive_type = basis_set::Primitive<T>;
    using center_type    = typename primitive_type::center_type;
    using coord_type     = typename primitive_type::coord_type;
    using cg_type        = basis_set::ContractedGaussian<primitive_type>;
    using size_type      = typename cg_type::size_type;
    using vector_type    = std::vector<T>;

    python_class_type<cg_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init([](vector_type& cs, vector_type& es, center_type r) {
          return cg_type(cs.begin(), cs.end(), es.begin(), es.end(), r);
      }))
      .def(pybind11::init([](vector_type& cs, vector_type& es, coord_type x,
                             coord_type y, coord_type z) {
          return cg_type(cs.begin(), cs.end(), es.begin(), es.end(), x, y, z);
      }))
      .def("empty", [](cg_type& cg) { return cg.empty(); })
      .def("at", [](cg_type& cg, size_type i) { return cg[i]; })
      .def("size", [](cg_type& cg) { return cg.size(); })
      .def_property(
        "center", [](cg_type& cg) { return cg.center(); },
        [](cg_type& cg, center_type c) { cg.center() = c; })
      .def("is_null", [](cg_type& cg) { return cg.is_null(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_contracted_gaussian(python_module_reference m) {
    detail_::export_contracted_gaussian_<float>("ContractedGaussianF", m);
    detail_::export_contracted_gaussian_<double>("ContractedGaussianD", m);
}

} // namespace chemist