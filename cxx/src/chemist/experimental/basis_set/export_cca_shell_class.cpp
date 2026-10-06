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
namespace {

/// Binds CCAShell<AOType> as @p name
template<typename AOType>
void export_cca_shell(python_module_reference m, const char* name) {
    using shell_type = CCAShell<AOType>;
    using size_type  = std::size_t;

    auto c = python_class_type<shell_type, AOShell<AOType>>(m, name)
               .def(py::init<>())
               .def(py::init<ContractedGaussian>())
               // Narrows the templated range ctors to double; see CartesianAO.
               .def(py::init([](std::vector<double> cs, std::vector<double> es,
                                size_type l, double x, double y, double z) {
                   return shell_type(cs.begin(), cs.end(), es.begin(), es.end(),
                                     l, x, y, z);
               }))
               .def(py::init([](std::vector<double> cs, std::vector<double> es,
                                size_type l, const Point& r0) {
                   return shell_type(cs.begin(), cs.end(), es.begin(), es.end(),
                                     l, r0);
               }))
               .def(py::self == py::self)
               .def(py::self != py::self);

    detail_::add_cca_shell_readers(c);
    detail_::add_cca_shell_writers(c);
}

} // namespace

void export_cca_shell_class(python_module_reference m) {
    export_cca_shell<CartesianAO>(m, "CartesianCCAShell");
    export_cca_shell<SphericalAO>(m, "SphericalCCAShell");
}

} // namespace chemist::experimental
