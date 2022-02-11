#pragma once
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/operators/detail_/operator_impl.hpp"

namespace libchemist::operators {

/** @brief Models the kinetic energy operator of a particle
 *
 *  @tparam Particle The type of the particle.
 */
template<typename Particle>
class Kinetic_Nuclear : public detail_::OperatorImpl<Kinetic_Nuclear, Particle> {
private:
    using base_type = detail_::OperatorImpl<Kinetic_Nuclear, Particle>;

public:
    Kinetic_Nuclear()                                  = default;
    Kinetic_Nuclear(const Kinetic_Nuclear&)            = default;
    Kinetic_Nuclear(Kinetic_Nuclear&&)                 = default;
    Kinetic_Nuclear(Particle p) : base_type(std::move(p)) {}
    Kinetic_Nuclear& operator=(const Kinetic_Nuclear&) = default;
    Kinetic_Nuclear& operator=(Kinetic_Nuclear&&)      = default;
};

/// Type of the electronic kinetic energy operator
using ElectronKinetic_Nuclear = Kinetic_Nuclear<libchemist::Electron>;

/// Type of the many electron kinetic energy operator
using NElectronKinetic_Nuclear = Kinetic_Nuclear<libchemist::ManyElectrons>;

} // namespace libchemist::operators
