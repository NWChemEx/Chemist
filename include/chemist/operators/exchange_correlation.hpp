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
};

/// Type of Kohn-Sham Exchange-Correlation operator
using ExchangeCorrelation =
  KohnShamExchangeCorrelation<ExchangeCorrelationFunctional, chemist::Electron,
                              chemist::OneElectronDensity>;

} // namespace chemist::operators
