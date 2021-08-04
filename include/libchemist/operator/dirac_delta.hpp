#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist {

template<typename Particle1, typename Particle2>
class DiracDelta
  : public detail_::OperatorImpl<DiracDelta, Particle1, Particle2> {
private:
    using base_type = detail_::OperatorImpl<DiracDelta, Particle1, Particle2>;

public:
    using base_type::base_type;
};

using ElectronElectronDelta = DiracDelta<Electron, Electron>;

} // namespace libchemist
