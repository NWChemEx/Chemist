#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist {

template<typename Particle>
class KohnShamExchangeCorrelation
  : public detail_::OperatorImpl<KohnShamExchangeCorrelation, Particle> {};

using ExchangeCorrelation = KohnShamExchangeCorrelation<Electron>;

} // namespace libchemist
