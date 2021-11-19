#pragma once
#include "chemist/electrons/electrons.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

/** @brief Models a Dirac Delta function.
 *
 *  @tparam Particle1 The type of the first particle.
 *  @tparam Particle2 The type of the second particle.
 *
 */
template<typename Particle1, typename Particle2>
class DiracDelta
  : public detail_::OperatorImpl<DiracDelta, Particle1, Particle2> {
private:
    /// Type of the object this class inherits from
    using base_type = detail_::OperatorImpl<DiracDelta, Particle1, Particle2>;

public:
    DiracDelta()                  = default;
    DiracDelta(const DiracDelta&) = default;
    DiracDelta(DiracDelta&&)      = default;
    DiracDelta(Particle1 p1, Particle2 p2) :
      base_type(std::move(p1), std::move(p2)) {}
    DiracDelta& operator=(const DiracDelta&) = default;
    DiracDelta& operator=(DiracDelta&&) = default;

protected:
    /// Gives a more traditional representation than the default implementation
    std::string as_string_impl() const override { return "δ\u0302(r₁ - r₂)"; }
};

/// Type of an electronic Dirac Delta
using ElectronElectronDelta = DiracDelta<chemist::Electron, chemist::Electron>;

} // namespace chemist::operators
