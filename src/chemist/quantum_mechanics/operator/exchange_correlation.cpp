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

#include <chemist/quantum_mechanics/operator/exchange_correlation.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

namespace chemist::qm_operator {

#define TPARAMS template<typename LHSParticleType, typename RHSParticleType>
#define XC ExchangeCorrelation<LHSParticleType, RHSParticleType>

TPARAMS
XC::ExchangeCorrelation(lhs_value_type p0, rhs_value_type p1) noexcept :
  base_type(std::move(p0), std::move(p1)) {}

#undef XC
#undef TPARAMS

template class ExchangeCorrelation<Electron, Electron>;
template class ExchangeCorrelation<ManyElectrons, ManyElectrons>;
template class ExchangeCorrelation<Electron, Nuclei>;
template class ExchangeCorrelation<ManyElectrons, Nuclei>;
template class ExchangeCorrelation<Nuclei, Nuclei>;

} // namespace chemist::qm_operator