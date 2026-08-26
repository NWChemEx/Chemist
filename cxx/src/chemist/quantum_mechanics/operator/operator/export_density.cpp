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

#include "export_operator.hpp"
#include <chemist/electron/electron.hpp>
#include <chemist/quantum_mechanics/operator/density.hpp>
#include <chemist/quantum_mechanics/wavefunction/cmos.hpp>
#include <chemist/quantum_mechanics/wavefunction/mos.hpp>

namespace chemist::qm_operator {

namespace detail_ {

template<typename OrbitalsType, typename ParticleType>
void export_density_(const char* name, python_module_reference m) {
    using density_t  = Density<OrbitalsType, ParticleType>;
    using weights_t  = typename density_t::weight_vector_type;
    using orbitals_t = typename density_t::orbitals_type;
    using wf_t       = wavefunction::Determinant<OrbitalsType>;

    auto get_orbitals = [](const density_t& self) { return self.orbitals(); };
    auto set_orbitals = [](density_t& self, orbitals_t& o) {
        self.orbitals() = o;
    };
    auto get_weights = [](const density_t& self) { return self.weights(); };
    auto set_weights = [](density_t& self, weights_t& w) {
        self.weights() = w;
    };
    auto size_fn = [](const density_t& self) { return self.size(); };

    python_class_type<density_t, OperatorBase, py::smart_holder>(m, name)
      .def(py::init<>())
      .def(py::init<orbitals_t, weights_t>())
      .def(py::init<const wf_t&>())
      .def_property("orbitals", get_orbitals, set_orbitals)
      .def_property("weights", get_weights, set_weights)
      .def("size", size_fn)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace detail_

void export_density(python_module_reference m) {
    using wavefunction::CMOs;
    using wavefunction::MOs;

    // N.B. These are exactly the instantiations OperatorVisitor dispatches on
    // (see operator_visitor.hpp).
    detail_::export_density_<MOs, Electron>("DensityMOsElectron", m);
    detail_::export_density_<CMOs, Electron>("DensityCMOsElectron", m);
}

} // namespace chemist::qm_operator
