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
#include <chemist/basis_set2/shell/shell.hpp>
#include <pybind11/stl.h>

namespace chemist {
namespace detail_ {

template<typename T>
void export_shell_(const char* name, python_module_reference m) {
    using primitive_type        = basis_set::Primitive<T>;
    using cg_type               = basis_set::ContractedGaussian<primitive_type>;
    using shell_type            = basis_set::Shell<cg_type>;
    using size_type             = typename shell_type::size_type;
    using pure_type             = typename shell_type::pure_type;
    using angular_momentum_type = typename shell_type::angular_momentum_type;
    using center_type           = typename primitive_type::center_type;
    using coord_type            = typename primitive_type::coord_type;
    using vector_type           = std::vector<T>;

    python_class_type<shell_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init([](pure_type pure, angular_momentum_type l,
                             vector_type& cs, vector_type& es, center_type r) {
          return shell_type(pure, l, cs.begin(), cs.end(), es.begin(), es.end(),
                            r);
      }))
      .def(pybind11::init([](pure_type pure, angular_momentum_type l,
                             vector_type& cs, vector_type& es, coord_type x,
                             coord_type y, coord_type z) {
          return shell_type(pure, l, cs.begin(), cs.end(), es.begin(), es.end(),
                            x, y, z);
      }))
      .def(pybind11::init<pure_type, angular_momentum_type, cg_type>())
      .def_property(
        "pure", [](shell_type& s) { return s.pure(); },
        [](shell_type& s, pure_type p) { s.pure() = p; })
      .def_property(
        "l", [](shell_type& s) { return s.l(); },
        [](shell_type& s, angular_momentum_type l) { s.l() = l; })
      .def_property(
        "contracted_gaussian",
        [](shell_type& s) { return s.contracted_gaussian(); },
        [](shell_type& s, cg_type cg) { s.contracted_gaussian() = cg; })
      .def("n_primitives", [](shell_type& s) { return s.n_primitives(); })
      .def("primitive",
           [](shell_type& s, size_type i) { return s.primitive(i); })
      .def("size", [](shell_type& s) { return s.size(); })
      .def("is_null", [](shell_type& s) { return s.is_null(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_shell(python_module_reference m) {
    detail_::export_shell_<float>("ShellF", m);
    detail_::export_shell_<double>("ShellD", m);
}

} // namespace chemist