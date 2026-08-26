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
#include <chemist/quantum_mechanics/operator/coulomb.hpp>

namespace chemist::qm_operator {

void export_coulomb(python_module_reference m) {
    using namespace detail_;

    // Coulomb is the one operator registered with two of OperatorVisitor's
    // macros: TWO_PARTICLE_OVERLOADS and DENSITY_OVERLOADS.
    using table        = join_tables<two_particle_pairs, density_pairs>;
    using default_pair = type_pair<Electron, Electron>;

    auto impls    = export_instantiations<Coulomb, table>(m, "Coulomb");
    auto dispatch = make_two_particle_dispatch<Coulomb, table, default_pair>(
      "Coulomb", "takes two particles");

    export_dispatching_class(
      m, "Coulomb", impls, dispatch,
      "Describes the Coulomb interaction between two particles.\n\n"
      "Coulomb(lhs, rhs) selects the instantiation the particles imply. "
      "Coulomb() is the same as Coulomb(Electron(), Electron()).");
}

} // namespace chemist::qm_operator
