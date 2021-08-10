#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist::operators {

template<typename Particle>
class KohnShamExchangeCorrelation
  : public detail_::OperatorImpl<KohnShamExchangeCorrelation, Particle> {
private:
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
