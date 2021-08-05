#pragma once
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/many_electrons.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist {

template<typename Particle>
class Kinetic : public detail_::OperatorImpl<Kinetic, Particle> {
private:
    using base_type = detail_::OperatorImpl<Kinetic, Particle>;

public:
    using base_type::base_type;
};

using ElectronKinetic = Kinetic<Electron>;
using NElectronKinetic = Kinetic<ManyElectrons>;

} // namespace libchemist
