#pragma once
#include "libchemist/operator/Electron.hpp"
#include "libchemist/operator/detail_/operator_impl.hpp"

namespace libchemist {

template<typename Particle>
class KohnShamExchangeCorrelation
  : public detail_::OperatorImpl<KohnShamExchangeCorrelation, Particle> {};

using MeanFieldKohnShamExchangeCorrelation =
  KohnShamExchangeCorrelation<Electron>;

} // namespace libchemist
