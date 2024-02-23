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
#include <pybind11/operators.h>

namespace chemist::fragmenting {

using nucleus_type = Cap::value_type;
using size_type    = Cap::size_type;
using reference    = Cap::reference;
using at_fxn       = reference (Cap::*)(size_type);
void export_cap(python_module_reference m) {
    python_class_type<Cap>(m, "Cap")
      .def(pybind11::init<>())
      .def(pybind11::init(
        [](size_type anchor, size_type replaced, pybind11::args args) {
            std::vector<nucleus_type> buffer(args.size());
            for(decltype(args.size()) i = 0; i < args.size(); ++i)
                buffer[i] = args[i].cast<nucleus_type>();
            return Cap(anchor, replaced, buffer.begin(), buffer.end());
        }))
      .def("insert", &Cap::insert)
      .def("at", static_cast<at_fxn>(&Cap::at))
      .def("__getitem__", static_cast<at_fxn>(&Cap::at))
      .def("size", &Cap::size)
      .def("__len__", &Cap::size)
      .def("set_anchor_index", &Cap::set_anchor_index)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist::fragmenting
