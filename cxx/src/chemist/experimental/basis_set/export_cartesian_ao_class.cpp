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
#include <vector>

namespace chemist::experimental {

void export_cartesian_ao_class(python_module_reference m) {
    using size_type = std::size_t;

    auto c = python_class_type<CartesianAO, AO>(m, "CartesianAO")
               .def(py::init<>())
               // The templated range ctors are not directly bindable; these
               // overloads narrow them to double, the same way
               // ContractedGaussian's binding does.
               .def(py::init([](std::vector<double> cs, std::vector<double> es,
                                size_type i, size_type j, size_type k, double x,
                                double y, double z) {
                   return CartesianAO(cs.begin(), cs.end(), es.begin(),
                                      es.end(), i, j, k, x, y, z);
               }))
               .def(py::init([](std::vector<double> cs, std::vector<double> es,
                                size_type i, size_type j, size_type k,
                                const Point& r0) {
                   return CartesianAO(cs.begin(), cs.end(), es.begin(),
                                      es.end(), i, j, k, r0);
               }))
               .def(py::self == py::self)
               .def(py::self != py::self);

    detail_::add_cartesian_ao_readers(c);
    detail_::add_cartesian_ao_writers(c);
}

} // namespace chemist::experimental
