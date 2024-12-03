/*
 * Copyright 2024 NWChemEx-Project
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

#include <chemist/quantum_mechanics/operator/coulomb.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

namespace chemist::qm_operator {

#define TPARAMS template<typename LHSParticleType, typename RHSParticleType>
#define COULOMB Coulomb<LHSParticleType, RHSParticleType>

TPARAMS
COULOMB::Coulomb(lhs_value_type p0, rhs_value_type p1) noexcept :
  base_type(std::move(p0), std::move(p1)) {}

#undef COULOMB
#undef TPARAMS

template class Coulomb<Electron, Electron>;
template class Coulomb<ManyElectrons, ManyElectrons>;
template class Coulomb<Electron, chemist::Density<Electron>>;
template class Coulomb<ManyElectrons, chemist::Density<Electron>>;
template class Coulomb<Electron, DecomposableDensity<Electron>>;
template class Coulomb<ManyElectrons, DecomposableDensity<Electron>>;
template class Coulomb<Electron, Nuclei>;
template class Coulomb<ManyElectrons, Nuclei>;
template class Coulomb<Nuclei, Nuclei>;

} // namespace chemist::qm_operator