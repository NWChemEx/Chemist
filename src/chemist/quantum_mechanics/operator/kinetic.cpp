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

#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

namespace chemist::qm_operator {

#define TPARAMS template<typename ParticleType>
#define KINETIC Kinetic<ParticleType>

TPARAMS
KINETIC::Kinetic(value_type particle) noexcept :
  base_type(std::move(particle)) {}

#undef KINETIC
#undef TPARAMS

template class Kinetic<Electron>;
template class Kinetic<ManyElectrons>;
template class Kinetic<Nucleus>;
template class Kinetic<Nuclei>;

} // namespace chemist::qm_operator