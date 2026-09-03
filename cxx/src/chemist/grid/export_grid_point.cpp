/*
 * Copyright 2025 NWChemEx-Project
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

#include "export_grid.hpp"
#include <chemist/grid/grid_point.hpp>
#include <wtf/wtf.hpp>

namespace chemist {

void export_grid_point(python_module_reference m) {
    using gridp_type = GridPoint;
    using py_fp_type = double;

    python_class_type<gridp_type>(m, "GridPoint")
      .def(py::init<>())
      .def(py::init<py_fp_type, py_fp_type, py_fp_type, py_fp_type>())
      .def("get_weight",
           [](gridp_type& g) {
               return wtf::fp::float_cast<py_fp_type>(g.get_weight());
           })
      .def("set_weight", [](gridp_type& g, py_fp_type w) { g.set_weight(w); })
      .def("get_x",
           [](gridp_type& g) {
               return wtf::fp::float_cast<py_fp_type>(g.get_x());
           })
      .def("set_x", [](gridp_type& g, py_fp_type x) { g.set_x(x); })
      .def("get_y",
           [](gridp_type& g) {
               return wtf::fp::float_cast<py_fp_type>(g.get_y());
           })
      .def("set_y", [](gridp_type& g, py_fp_type y) { g.set_y(y); })
      .def("get_z",
           [](gridp_type& g) {
               return wtf::fp::float_cast<py_fp_type>(g.get_z());
           })
      .def("set_z", [](gridp_type& g, py_fp_type z) { g.set_z(z); })
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace chemist
