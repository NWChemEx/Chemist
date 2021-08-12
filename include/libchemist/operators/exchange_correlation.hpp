#pragma once
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/operators/detail_/operator_impl.hpp"

namespace libchemist::operators {

/** @brief Kohn-Sham Exchange-Correlation operator
 *
 *  @tparam Particle The type of particle.
 */
template<typename Particle>
class KohnShamExchangeCorrelation
  : public detail_::OperatorImpl<KohnShamExchangeCorrelation, Particle> {
private:
    /// Type of the object this class inherits from
    using base_type =
      detail_::OperatorImpl<KohnShamExchangeCorrelation, Particle>;

public:
    KohnShamExchangeCorrelation()                                   = default;
    KohnShamExchangeCorrelation(const KohnShamExchangeCorrelation&) = default;
    KohnShamExchangeCorrelation(KohnShamExchangeCorrelation&&)      = default;
    KohnShamExchangeCorrelation(Particle p) : base_type(std::move(p)) {}
    KohnShamExchangeCorrelation& operator=(const KohnShamExchangeCorrelation&) =
      default;
    KohnShamExchangeCorrelation& operator=(KohnShamExchangeCorrelation&&) =
      default;
};

/// Type of Kohn-Sham Exchange-Correlation operator
using ExchangeCorrelation = KohnShamExchangeCorrelation<libchemist::Electron>;

} // namespace libchemist::operators
