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
#include "chemist/density/density.hpp"
#include "chemist/electrons/electrons.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

enum class ExchangeCorrelationFunctional {
    _INVALID,
    SVWN3,
    SVWN5,
    BLYP,
    PBE,
    revPBE,
    PBE0,
    B3LYP
};

/** @brief Kohn-Sham Exchange-Correlation operator
 *
 *  @tparam Particle The type of particle.
 */
template<typename XCFunctionalType, typename Particle1, typename Particle2>
class KohnShamExchangeCorrelation
  : public detail_::OperatorImpl<KohnShamExchangeCorrelation, XCFunctionalType,
                                 Particle1, Particle2> {
private:
    /// Type of the object this class inherits from
    using base_type =
      detail_::OperatorImpl<KohnShamExchangeCorrelation, XCFunctionalType,
                            Particle1, Particle2>;

public:
    KohnShamExchangeCorrelation(const KohnShamExchangeCorrelation&) = default;
    KohnShamExchangeCorrelation(KohnShamExchangeCorrelation&&)      = default;
    KohnShamExchangeCorrelation(XCFunctionalType f, Particle1 p1,
                                Particle2 p2) :
      base_type(std::move(f), std::move(p1), std::move(p2)) {}
    KohnShamExchangeCorrelation() :
      KohnShamExchangeCorrelation(XCFunctionalType::_INVALID, Particle1{},
                                  Particle2{}) {}
    KohnShamExchangeCorrelation& operator=(const KohnShamExchangeCorrelation&) =
      default;
    KohnShamExchangeCorrelation& operator=(KohnShamExchangeCorrelation&&) =
      default;

protected:
    /// Gives a more traditional representation than the default implementation
    std::string as_string_impl() const override { return "xc"; }
};

/// Type of Kohn-Sham Exchange-Correlation operator
using ExchangeCorrelation =
  KohnShamExchangeCorrelation<ExchangeCorrelationFunctional, chemist::Electron,
                              chemist::OneElectronDensity>;

} // namespace chemist::operators
