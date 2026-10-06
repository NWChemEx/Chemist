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

void export_spherical_ao_class(python_module_reference m) {
    using size_type             = std::size_t;
    using magnetic_index_type   = typename SphericalAO::magnetic_index_type;
    using const_shell_reference = typename SphericalAO::const_shell_reference;

    auto c = python_class_type<SphericalAO, AO>(m, "SphericalAO")
               .def(py::init<>())
               .def(py::init<ContractedGaussian, magnetic_index_type>())
               .def(py::init<const_shell_reference, magnetic_index_type>())
               // Narrows the templated range ctors to double; see CartesianAO.
               .def(py::init([](std::vector<double> cs, std::vector<double> es,
                                size_type l, magnetic_index_type m_l, double x,
                                double y, double z) {
                   return SphericalAO(cs.begin(), cs.end(), es.begin(),
                                      es.end(), l, m_l, x, y, z);
               }))
               .def(py::init([](std::vector<double> cs, std::vector<double> es,
                                size_type l, magnetic_index_type m_l,
                                const Point& r0) {
                   return SphericalAO(cs.begin(), cs.end(), es.begin(),
                                      es.end(), l, m_l, r0);
               }))
               // In C++ this is a read-only reference to the shell *this owns.
               // Binding that reference directly would hand Python a mutable
               // shell (pybind11 can not express const), so it is returned as a
               // read-only view of the shell instead.
               .def(
                 "get_cartesian_shell",
                 [](const SphericalAO& ao) {
                     return ao.get_cartesian_shell().as_view();
                 },
                 py::keep_alive<0, 1>())
               .def(py::self == py::self)
               .def(py::self != py::self);

    detail_::add_spherical_ao_readers(c);
    detail_::add_spherical_ao_writers(c);
}

} // namespace chemist::experimental
