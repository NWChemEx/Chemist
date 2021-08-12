#pragma once
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/operators/detail_/operator_impl.hpp"

namespace libchemist::operators {

/** @brief Models the kinetic energy operator of a particle
 *
 *  @tparam Particle The type of the particle.
 */
template<typename Particle>
class Kinetic : public detail_::OperatorImpl<Kinetic, Particle> {
private:
    using base_type = detail_::OperatorImpl<Kinetic, Particle>;

public:
    Kinetic()               = default;
    Kinetic(const Kinetic&) = default;
    Kinetic(Kinetic&&)      = default;
    Kinetic(Particle p) : base_type(std::move(p)) {}
    Kinetic& operator=(const Kinetic&) = default;
    Kinetic& operator=(Kinetic&&) = default;
};

/// Type of the electronic kinetic energy operator
using ElectronKinetic = Kinetic<libchemist::Electron>;

/// Type of the many electron kinetic energy operator
using NElectronKinetic = Kinetic<libchemist::ManyElectrons>;

} // namespace libchemist::operators
