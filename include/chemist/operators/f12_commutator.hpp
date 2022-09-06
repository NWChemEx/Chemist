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
#include "chemist/operators/slater_type_geminal.hpp"

namespace chemist::operators {

/** @brief Models an F12 Commutator
 *
 *  @tparam Geminal The type of the geminal relating the two particles.
 *  @tparam Particle1 The type of the first particle.
 *  @tparam Particle2 The type of the second particle.
 */
template<typename Geminal, typename Particle1, typename Particle2>
class F12Commutator
  : public detail_::OperatorImpl<F12Commutator, Geminal, Particle1, Particle2> {
private:
    /// Type of the object this class inherits from
    using base_type =
      detail_::OperatorImpl<F12Commutator, Geminal, Particle1, Particle2>;

public:
    F12Commutator()                     = default;
    F12Commutator(const F12Commutator&) = default;
    F12Commutator(F12Commutator&&)      = default;
    F12Commutator(Geminal g, Particle1 p1, Particle2 p2) :
      base_type(std::move(g), std::move(p1), std::move(p2)) {}
    F12Commutator& operator=(const F12Commutator&) = default;
    F12Commutator& operator=(F12Commutator&&) = default;
};

/// Type of a Slater Type Geminal Commutator
using ElectronElectronF12Commutator =
  F12Commutator<STG, chemist::Electron, chemist::Electron>;

} // namespace chemist::operators
