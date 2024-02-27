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
#include <chemist/fragmenting/capping/cap_set.hpp>
#include <pybind11/operators.h>

namespace chemist::fragmenting {

using cap_type     = CapSet::value_type;
using nucleus_type = typename cap_type::value_type;
using size_type    = CapSet::size_type;
using reference    = CapSet::reference;
using at_fxn       = reference (CapSet::*)(size_type);

void export_cap_set(python_module_reference m) {
    auto rvp = pybind11::return_value_policy::reference_internal;

    python_class_type<CapSet>(m, "CapSet")
      .def(pybind11::init<>())
      .def(pybind11::init([](pybind11::args args) {
          auto buffer = args_to_buffer<cap_type>(std::move(args));
          return CapSet(buffer.begin(), buffer.end());
      }))
      .def("push_back", &CapSet::push_back)
      .def("emplace_back",
           [](CapSet& self, size_type anchor, size_type replaced,
              pybind11::args args) {
               auto buffer = args_to_buffer<nucleus_type>(std::move(args));
               self.push_back(
                 cap_type(anchor, replaced, buffer.begin(), buffer.end()));
           })
      .def("at", static_cast<at_fxn>(&CapSet::at), rvp)
      .def("__getitem__", static_cast<at_fxn>(&CapSet::at), rvp)
      .def("__setitem__",
           [](CapSet& self, size_type i, cap_type cap) { self.at(i) = cap; })
      .def("size", [](CapSet& self) { return self.size(); })
      .def("__len__", [](CapSet& self) { return self.size(); })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist::fragmenting
