#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"
#include "libchemist/operator/slater_type_geminal.hpp"

namespace libchemist {

template<typename Geminal, typename Particle1, typename Particle2>
class CorrelationFactor
  : public detail_::OperatorImpl<CorrelationFactor, Geminal, Particle1,
                                 Particle2> {
private:
    using base_type =
      detail_::OperatorImpl<CorrelationFactor, Geminal, Particle1, Particle2>;
    using my_type = CorrelationFactor<Geminal, Particle1, Particle2>;

public:
    using base_type::base_type;

    my_type operator*(const my_type& rhs) const;
};

#define CORRELATION_FACTOR CorrelationFactor<Geminal, Particle1, Particle2>

template<typename Geminal, typename Particle1, typename Particle2>
CORRELATION_FACTOR CORRELATION_FACTOR::operator*(
  const CorrelationFactor& rhs) const {
    const auto& lhs_geminal = this->template at<0>();
    const auto& rhs_geminal = rhs.template at<0>();
    const auto f12_squared  = lhs_geminal * rhs_geminal;
    const auto& p0          = this->template at<1>();
    const auto& p1          = this->template at<2>();
    return my_type(std::move(f12_squared), p0, p1);
}

#undef CORRELATION_FACTOR

using ElectronElectronSTG =
  CorrelationFactor<SlaterTypeGeminal, Electron, Electron>;

} // namespace libchemist
