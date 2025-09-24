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
#include <chemist/grid/grid_point_view.hpp>
#include <pybind11/operators.h>

namespace chemist {

void export_grid_point_view(python_module_reference m) {
    using gridp_type       = GridPoint;
    using gridp_view       = GridPointView<GridPoint>;
    using gridp_reference  = typename gridp_view::grid_point_reference;
    using weight_reference = typename gridp_view::weight_reference;
    using point_view       = typename gridp_view::point_view;

    python_class_type<gridp_view>(m, "GridPointView")
      .def(pybind11::init<gridp_reference>())
      .def_property(
        "weight", [](gridp_view& g) -> weight_reference { return g.weight(); },
        [](gridp_view& g, weight_reference w) { g.weight() = w; },
        pybind11::return_value_policy::reference_internal)
      .def_property(
        "point", [](gridp_view& g) -> point_view& { return g.point(); },
        [](gridp_view& g, point_view p) { g.point() = p; },
        pybind11::return_value_policy::reference_internal)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self == gridp_type())
      .def(gridp_type() == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def(pybind11::self != gridp_type())
      .def(gridp_type() != pybind11::self);
}

} // namespace chemist
