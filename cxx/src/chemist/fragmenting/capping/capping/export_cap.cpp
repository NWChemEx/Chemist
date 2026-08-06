/*
 * Copyright 2024 NWChemEx-Project
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

#include "export_capping.hpp"
#include <chemist/fragmenting/capping/cap.hpp>

namespace chemist::fragmenting {

using nucleus_type = Cap::value_type;
using size_type    = Cap::size_type;
using reference    = Cap::reference;

void export_cap(python_module_reference m) {
    auto rvp = py::return_value_policy::reference_internal;
    // A lambda, rather than static_cast<reference
    // (Cap::*)(size_type)>(&Cap::at), to pick the non-const overload: GCC (at
    // least through 10.2.1, the compiler manylinux2014 ships) can't resolve
    // that cast when the underlying method's return type comes from
    // decltype(auto) (as IndexableContainerBase::at's does), erroring "no
    // matches converting function 'at'".
    auto at_fxn = [](Cap& self, size_type i) -> reference {
        return self.at(i);
    };

    python_class_type<Cap>(m, "Cap")
      .def(py::init<>())
      .def(py::init([](size_type anchor, size_type replaced, py::args args) {
          auto buffer = args_to_buffer<nucleus_type>(std::move(args));
          return Cap(anchor, replaced, buffer.begin(), buffer.end());
      }))
      .def("insert", &Cap::insert)
      .def("at", at_fxn, rvp)
      .def("__getitem__", at_fxn, rvp)
      .def("__setitem__",
           [](Cap& self, size_type i, nucleus_type atom) { self.at(i) = atom; })
      .def("size", &Cap::size)
      .def("__len__", &Cap::size)
      .def("set_anchor_index", &Cap::set_anchor_index)
      .def("get_anchor_index",
           [](Cap& self) {
               try {
                   return self.get_anchor_index();
               } catch(const std::bad_optional_access& e) {
                   throw std::runtime_error("No Anchor Set");
               }
           })
      .def("set_replaced_index", &Cap::set_replaced_index)
      .def("get_replaced_index",
           [](Cap& self) {
               try {
                   return self.get_replaced_index();
               } catch(const std::bad_optional_access& e) {
                   throw std::runtime_error("No Replacement Set");
               }
           })
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace chemist::fragmenting
