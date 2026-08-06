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
#include <chemist/grid/grid_class.hpp>

namespace chemist {

void export_grid_class(python_module_reference m) {
    using grid_type   = Grid;
    using value_type  = typename grid_type::value_type;
    using size_type   = typename grid_type::size_type;
    using vector_type = std::vector<value_type>;

    python_class_type<grid_type>(m, "Grid")
      .def(py::init<>())
      .def(py::init([](vector_type& grid_points) {
          return grid_type(grid_points.begin(), grid_points.end());
      }))
      .def("empty", [](grid_type& self) { return self.empty(); })
      .def("at", [](grid_type& self, size_type i) { return self[i]; })
      .def("size", [](grid_type& self) { return self.size(); })
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace chemist
