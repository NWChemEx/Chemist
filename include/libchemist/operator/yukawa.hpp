#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"
#include "libchemist/operator/slater_type_geminal.hpp"

namespace libchemist {

template<typename Geminal, typename Particle1, typename Particle2>
class Yukawa
  : public detail_::OperatorImpl<Yukawa, Geminal, Particle1, Particle2> {
private:
    using base_type =
      detail_::OperatorImpl<Yukawa, Geminal, Particle1, Particle2>;

public:
    using base_type::base_type;
};

using ElectronElectronYukawa = Yukawa<STG, Electron, Electron>;

} // namespace libchemist
