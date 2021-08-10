#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"
#include "libchemist/operator/slater_type_geminal.hpp"

namespace libchemist::operators {

template<typename Geminal, typename Particle1, typename Particle2>
class Yukawa
  : public detail_::OperatorImpl<Yukawa, Geminal, Particle1, Particle2> {
private:
    using base_type =
      detail_::OperatorImpl<Yukawa, Geminal, Particle1, Particle2>;

public:
    Yukawa()              = default;
    Yukawa(const Yukawa&) = default;
    Yukawa(Yukawa&&)      = default;
    Yukawa(Geminal g, Particle1 p1, Particle2 p2);
    Yukawa& operator=(const Yukawa&) = default;
    Yukawa& operator=(Yukawa&&) = default;
};

using ElectronElectronYukawa = Yukawa<STG, Electron, Electron>;

template<typename Geminal, typename Particle1, typename Particle2>
Yukawa<Geminal, Particle1, Particle2>::Yukawa(Geminal g, Particle1 p1,
                                              Particle2 p2) :
  base_type(std::move(g), std::move(p1), std::move(p2)) {}

} // namespace libchemist::operators
