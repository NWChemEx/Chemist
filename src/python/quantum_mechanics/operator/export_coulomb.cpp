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
#include <chemist/quantum_mechanics/operator/coulomb.hpp>

namespace chemist::qm_operator {

namespace detail_ {

template<typename T, typename U>
void export_coulomb_(const char* name, python_module_reference m) {
    using coulomb_t = Coulomb<T, U>;
    using op_base_t = OperatorBase;

    auto get_lhs_particle = [](const coulomb_t& o) {
        return o.get_lhs_particle();
    };
    auto set_lhs_particle = [](coulomb_t& o, T& p) { o.set_lhs_particle(p); };
    auto get_rhs_particle = [](const coulomb_t& o) {
        return o.get_rhs_particle();
    };
    auto set_rhs_particle = [](coulomb_t& o, U& p) { o.set_rhs_particle(p); };

    python_class_type<coulomb_t, op_base_t, py::smart_holder>(m, name)
      .def(py::init<>())
      .def(py::init<T, U>())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_property("lhs_particle", get_lhs_particle, set_lhs_particle)
      .def_property("rhs_particle", get_rhs_particle, set_rhs_particle);
}

} // namespace detail_

void export_coulomb(python_module_reference m) {
    detail_::export_coulomb_<Electron, Electron>("CoulombElectronElectron", m);
    detail_::export_coulomb_<ManyElectrons, ManyElectrons>(
      "CoulombManyElectronsManyElectrons", m);
    detail_::export_coulomb_<Electron, chemist::Density<Electron>>(
      "CoulombElectronDensityElectron", m);
    detail_::export_coulomb_<ManyElectrons, chemist::Density<Electron>>(
      "CoulombManyElectronsDensityElectrons", m);
    detail_::export_coulomb_<Electron, DecomposableDensity<Electron>>(
      "CoulombElectronDecomposableDensityElectron", m);
    detail_::export_coulomb_<ManyElectrons, DecomposableDensity<Electron>>(
      "CoulombManyElectronsDecomposableDensityElectron", m);
    detail_::export_coulomb_<Electron, Nuclei>("CoulombElectronNuclei", m);
    detail_::export_coulomb_<ManyElectrons, Nuclei>(
      "CoulombManyElectronsNuclei", m);
    detail_::export_coulomb_<Nuclei, Nuclei>("CoulombNucleiNuclei", m);
}

} // namespace chemist::qm_operator
