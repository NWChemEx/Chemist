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
#include <chemist/quantum_mechanics/operator/exchange.hpp>

namespace chemist::qm_operator {

void export_exchange(python_module_reference m) {
    using namespace detail_;

    using table        = density_pairs;
    using default_pair = type_pair<Electron, chemist::Density<Electron>>;

    auto impls    = export_instantiations<Exchange, table>(m, "Exchange");
    auto dispatch = make_two_particle_dispatch<Exchange, table, default_pair>(
      "Exchange", "takes two particles");

    export_dispatching_class(
      m, "Exchange", impls, dispatch,
      "Describes the exchange interaction between a particle and a "
      "density.\n\n"
      "Exchange(lhs, rhs) selects the instantiation the particles imply. "
      "Exchange() is the same as Exchange(Electron(), Density()).");
}

} // namespace chemist::qm_operator
