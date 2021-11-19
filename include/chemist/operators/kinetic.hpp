#pragma once
#include "chemist/electrons/electrons.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"

namespace chemist::operators {

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
using ElectronKinetic = Kinetic<chemist::Electron>;

/// Type of the many electron kinetic energy operator
using NElectronKinetic = Kinetic<chemist::ManyElectrons>;

} // namespace chemist::operators
