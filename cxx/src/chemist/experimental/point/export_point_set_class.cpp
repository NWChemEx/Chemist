/*
 * Copyright 2026 NWChemEx-Project
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

#include "../export_experimental.hpp"

namespace chemist::experimental {

void export_point_set_class(python_module_reference m) {
    auto c =
      python_class_type<PointSet>(m, "PointSet")
        .def(py::init<>())
        .def(py::init([](py::iterable points) {
            PointSet rv;
            for(auto p : points) rv.push_back(p.cast<const Point&>());
            return rv;
        }))
        .def("push_back", [](PointSet& s, const Point& p) { s.push_back(p); })
        .def("push_back",
             [](PointSet& s, const const_point_view& p) { s.push_back(p); })
        // The returned view aliases the set, so it must not outlive it.
        .def(
          "__getitem__", [](PointSet& s, std::size_t i) { return s.at(i); },
          py::keep_alive<0, 1>())
        .def("__setitem__",
             [](PointSet& s, std::size_t i, const Point& p) { s.at(i) = p; })
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_set_readers(c);
}

} // namespace chemist::experimental
