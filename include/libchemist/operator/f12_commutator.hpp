#pragma once
#include "libchemist/operator/Electron.hpp"
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/slater_type_geminal.hpp"

namespace libchemist {

template<typename Geminal, typename Particle1, typename Particle2>
class F12Commutator
  : public detail_::OperatorImpl<F12Commutator, Geminal, Particle1, Particle2> {
private:
    using base_type =
      detail_::OperatorIimpl<F12Commutator, Geminal, Particle1, Particle2>;

public:
    using base_type::OperatorImpl;
}

using ElectronElectronF12Commutator = F12Commutator<STG, Electron, Electron>;

} // namespace libchemist
