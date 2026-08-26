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
#include "py_operator_dispatch.hpp"
#include <chemist/quantum_mechanics/operator/exchange_correlation.hpp>

namespace chemist::qm_operator {

namespace detail_ {
namespace {

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

/** @brief Adds what ExchangeCorrelation has and the other operators do not.
 *
 *  Its value ctor takes the functional before the particles, and it has a
 *  property for the functional. Everything else is the common part
 *  export_instantiations already did.
 */
struct customize_xc {
    template<typename Class, typename T, typename U>
    void operator()(Class& cls, type_pair<T, U>) const {
        using xc_t = ExchangeCorrelation<T, U>;

        auto get_functional_name = [](const xc_t& o) {
            return o.get_functional_name();
        };
        auto set_functional_name = [](xc_t& o, xc_functional& p) {
            o.set_functional_name(p);
        };

        cls.def(py::init<xc_functional, T, U>())
          .def_property("functional_name", get_functional_name,
                        set_functional_name);
    }
};

} // namespace
} // namespace detail_

void export_exchange_correlation(python_module_reference m) {
    using namespace detail_;

    using table        = density_pairs;
    using default_pair = type_pair<Electron, chemist::Density<Electron>>;

    // The functional the ctor takes ahead of the two particles.
    using leading_args = std::tuple<xc_functional>;

    export_xc_functionals_(m);

    auto impls = export_instantiations<ExchangeCorrelation, table>(
      m, "ExchangeCorrelation", customize_xc{});

    auto dispatch = make_two_particle_dispatch<ExchangeCorrelation, table,
                                               default_pair, leading_args>(
      "ExchangeCorrelation", "takes a functional and two particles");

    export_dispatching_class(
      m, "ExchangeCorrelation", impls, dispatch,
      "Describes the exchange-correlation interaction between a particle and "
      "a density.\n\n"
      "ExchangeCorrelation(functional, lhs, rhs) selects the instantiation the "
      "particles imply. ExchangeCorrelation() is the same as "
      "ExchangeCorrelation(xc_functional.NONE, Electron(), Density()).");
}

} // namespace chemist::qm_operator
