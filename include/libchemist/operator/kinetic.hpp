#pragma once
#include "libchemist/operator/operator_impl.hpp"
#include "libchemist/oprator/particles.hpp"

namespace libchemist {

template<typename Particle>
class Kinetic : public detail_::OperatorImpl<Kinetic, Particle> {};

using ElectronKinetic = Kinetic<Electron>;

} // namespace libchemist
