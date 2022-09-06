/*
 * Copyright 2022 NWChemEx-Project
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

#pragma once
#include "chemist/electrons/electrons.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

/** @brief Models the kinetic energy operator of a particle
 *
 *  @tparam Particle The type of the particle.
 */
template<typename Particle>
class Kinetic : public detail_::OperatorImpl<Kinetic, Particle> {
private:
    using base_type = detail_::OperatorImpl<Kinetic, Particle>;

public:
    Kinetic()               = default;
    Kinetic(const Kinetic&) = default;
    Kinetic(Kinetic&&)      = default;
    Kinetic(Particle p) : base_type(std::move(p)) {}
    Kinetic& operator=(const Kinetic&) = default;
    Kinetic& operator=(Kinetic&&) = default;
};

/// Type of the electronic kinetic energy operator
using ElectronKinetic = Kinetic<chemist::Electron>;

/// Type of the many electron kinetic energy operator
using NElectronKinetic = Kinetic<chemist::ManyElectrons>;

} // namespace chemist::operators
