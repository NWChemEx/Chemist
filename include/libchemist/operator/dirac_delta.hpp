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
    DiracDelta()                  = default;
    DiracDelta(const DiracDelta&) = default;
    DiracDelta(DiracDelta&&)      = default;
    DiracDelta(Particle1 p1, Particle2 p2);
    DiracDelta& operator=(const DiracDelta&) = default;
    DiracDelta& operator=(DiracDelta&&) = default;

protected:
    std::string as_string_impl() const override { return "δ\u0302(r₁ - r₂)"; }
};

template<typename Particle1, typename Particle2>
DiracDelta<Particle1, Particle2>::DiracDelta(Particle1 p1, Particle2 p2) :
  base_type(std::move(p1), std::move(p2)) {}

using ElectronElectronDelta = DiracDelta<Electron, Electron>;

} // namespace libchemist
