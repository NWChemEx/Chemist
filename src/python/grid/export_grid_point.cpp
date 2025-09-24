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
#include <pybind11/operators.h>

namespace chemist {

void export_grid_point(python_module_reference m) {
    using gridp_type       = GridPoint;
    using weight_type      = typename gridp_type::weight_type;
    using weight_reference = typename gridp_type::weight_reference;
    using coord_type       = typename gridp_type::coord_type;
    using point_type       = typename gridp_type::point_type;
    using point_reference  = typename gridp_type::point_reference;

    python_class_type<gridp_type>(m, "GridPoint")
      .def(pybind11::init<>())
      .def(pybind11::init<weight_type, coord_type, coord_type, coord_type>())
      .def(pybind11::init<weight_type, point_type>())
      .def_property(
        "weight", [](gridp_type& g) -> weight_reference { return g.weight(); },
        [](gridp_type& g, weight_type w) { g.weight() = w; },
        pybind11::return_value_policy::reference_internal)
      .def_property(
        "point", [](gridp_type& g) -> point_reference { return g.point(); },
        [](gridp_type& g, point_type p) { g.point() = p; },
        pybind11::return_value_policy::reference_internal)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
