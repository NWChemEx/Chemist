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

/** @brief Models a Dirac Delta function.
 *
 *  @tparam Particle1 The type of the first particle.
 *  @tparam Particle2 The type of the second particle.
 *
 */
template<typename Particle1, typename Particle2>
class DiracDelta
  : public detail_::OperatorImpl<DiracDelta, Particle1, Particle2> {
private:
    /// Type of the object this class inherits from
    using base_type = detail_::OperatorImpl<DiracDelta, Particle1, Particle2>;

public:
    DiracDelta()                  = default;
    DiracDelta(const DiracDelta&) = default;
    DiracDelta(DiracDelta&&)      = default;
    DiracDelta(Particle1 p1, Particle2 p2) :
      base_type(std::move(p1), std::move(p2)) {}
    DiracDelta& operator=(const DiracDelta&) = default;
    DiracDelta& operator=(DiracDelta&&) = default;

protected:
    /// Gives a more traditional representation than the default implementation
    std::string as_string_impl() const override { return "δ\u0302(r₁ - r₂)"; }
};

/// Type of an electronic Dirac Delta
using ElectronElectronDelta = DiracDelta<chemist::Electron, chemist::Electron>;

} // namespace chemist::operators
