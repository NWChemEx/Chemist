#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"
#include "libchemist/operator/slater_type_geminal.hpp"

namespace libchemist::operators {

template<typename Geminal, typename Particle1, typename Particle2>
class F12Commutator
  : public detail_::OperatorImpl<F12Commutator, Geminal, Particle1, Particle2> {
private:
    using base_type =
      detail_::OperatorImpl<F12Commutator, Geminal, Particle1, Particle2>;

public:
    F12Commutator()                     = default;
    F12Commutator(const F12Commutator&) = default;
    F12Commutator(F12Commutator&&)      = default;
    F12Commutator(Geminal g, Particle1 p1, Particle2 p2);
    F12Commutator& operator=(const F12Commutator&) = default;
    F12Commutator& operator=(F12Commutator&&) = default;
};

using ElectronElectronF12Commutator = F12Commutator<STG, Electron, Electron>;

template<typename Geminal, typename Particle1, typename Particle2>
F12Commutator<Geminal, Particle1, Particle2>::F12Commutator(Geminal g,
                                                            Particle1 p1,
                                                            Particle2 p2) :
  base_type(std::move(g), std::move(p1), std::move(p2)) {}

} // namespace libchemist::operators
