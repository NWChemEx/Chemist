#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"
#include "libchemist/operator/slater_type_geminal.hpp"

namespace libchemist {

template<typeanem Geminal, typename Particle1, typename Particle2>
class CorrelationFactor
  : public detail_::OperatorImpl<CorrelationFactor, Geminal, Particle1,
                                 Particle2> {
private:
    using base_type =
      detail_::OperatorImpl<CorrelationFactor, Geminal, Particle1, Particle2>;
    using my_type = CorrelationFactor<Particle1, Particle2>;

public:
    using base_type::OperatorImpl;

    my_type operator*(const my_type& rhs) const;
};

#define CORRELATION_FACTOR CorrelationFactor<Geminal, Particle1, Particle2>

CORRELATION_FACTOR
CORRELATION_FACTOR::operator*(const CorrelationFactor& rhs) const {
    const auto f12_squared = this > at<0>() * rhs.at<0>();
    return my_type(std::move(f12_squared), this->at<1>(), this->at<2>());
}

#undef CORRELATION_FACTOR

using ElectronElectronSTG =
  CorrelationFactor<SlaterTypeGeminal, Electron, Electron>;

} // namespace libchemist
