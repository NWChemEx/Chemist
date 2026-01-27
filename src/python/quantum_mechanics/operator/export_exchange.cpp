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
#include <chemist/quantum_mechanics/operator/exchange.hpp>
#include <pybind11/operators.h>

namespace chemist::qm_operator {

template<typename T, typename U>
void export_exchange_(const char* name, python_module_reference m) {
    using exchange_t = Exchange<T, U>;

    auto get_lhs_particle = [](const exchange_t& o) {
        return o.lhs_particle();
    };
    auto set_lhs_particle = [](exchange_t& o, T& p) { o.lhs_particle() = p; };
    auto get_rhs_particle = [](const exchange_t& o) {
        return o.rhs_particle();
    };
    auto set_rhs_particle = [](exchange_t& o, U& p) { o.rhs_particle() = p; };

    python_class_type<exchange_t>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<T, U>())
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def_property("lhs_particle", get_lhs_particle, set_lhs_particle)
      .def_property("rhs_particle", get_rhs_particle, set_rhs_particle);
}

void export_exchange(python_module_reference m) {
    export_exchange_<Electron, chemist::Density<Electron>>(
      "ExchangeElectronDensityElectron", m);
    export_exchange_<ManyElectrons, chemist::Density<Electron>>(
      "ExchangeManyElectronsDensityElectron", m);
    export_exchange_<Electron, DecomposableDensity<Electron>>(
      "ExchangeElectronDecomposableDensityElectron", m);
    export_exchange_<ManyElectrons, DecomposableDensity<Electron>>(
      "ExchangeManyElectronsDecomposableDensityElectron", m);
}

} // namespace chemist::qm_operator
