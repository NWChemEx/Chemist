#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist::operators {

/** @brief Kohn-Sham Exchange-Correlation operator
 *
 *  @tparam Particle The type of particle.
 *
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

using ExchangeCorrelation = KohnShamExchangeCorrelation<Electron>;

} // namespace libchemist::operators
