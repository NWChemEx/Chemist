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

#include "export_operator.hpp"
#include <chemist/quantum_mechanics/operator/exchange_correlation.hpp>
#include <pybind11/native_enum.h>
#include <pybind11/operators.h>

namespace chemist::qm_operator {

void export_xc_functionals_(python_module_reference m) {
    python_enum_type<xc_functional>(m, "xc_functional", "enum.Enum")
      .value("NONE", xc_functional::NONE)
      .value("CUSTOM", xc_functional::CUSTOM)
      .value("SVWN3", xc_functional::SVWN3)
      .value("SVWN5", xc_functional::SVWN5)
      .value("BLYP", xc_functional::BLYP)
      .value("B3LYP", xc_functional::B3LYP)
      .value("PBE", xc_functional::PBE)
      .value("revPBE", xc_functional::revPBE)
      .value("PBE0", xc_functional::PBE0)
      .finalize();
}

template<typename T, typename U>
void export_exchange_correlation_(const char* name, python_module_reference m) {
    using xc_t = ExchangeCorrelation<T, U>;

    auto get_lhs_particle    = [](const xc_t& o) { return o.lhs_particle(); };
    auto set_lhs_particle    = [](xc_t& o, T& p) { o.lhs_particle() = p; };
    auto get_rhs_particle    = [](const xc_t& o) { return o.rhs_particle(); };
    auto set_rhs_particle    = [](xc_t& o, U& p) { o.rhs_particle() = p; };
    auto get_functional_name = [](const xc_t& o) {
        return o.functional_name();
    };
    auto set_functional_name = [](xc_t& o, xc_functional& p) {
        o.functional_name() = p;
    };

    python_class_type<xc_t>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<xc_functional, T, U>())
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def_property("lhs_particle", get_lhs_particle, set_lhs_particle)
      .def_property("rhs_particle", get_rhs_particle, set_rhs_particle)
      .def_property("functional_name", get_functional_name,
                    set_functional_name);
}

void export_exchange_correlation(python_module_reference m) {
    export_xc_functionals_(m);

    export_exchange_correlation_<Electron, chemist::Density<Electron>>(
      "ExchangeCorrelationElectronDensityElectron", m);
    export_exchange_correlation_<ManyElectrons, chemist::Density<Electron>>(
      "ExchangeCorrelationManyElectronsDensityElectrons", m);
    export_exchange_correlation_<Electron, DecomposableDensity<Electron>>(
      "ExchangeCorrelationElectronDecomposableDensityElectron", m);
    export_exchange_correlation_<ManyElectrons, DecomposableDensity<Electron>>(
      "ExchangeCorrelationManyElectronsDecomposableDensityElectron", m);
}

} // namespace chemist::qm_operator
