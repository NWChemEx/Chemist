#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

namespace chemist::qm_operator {

#define TPARAMS template<typename ParticleType>
#define KINETIC Kinetic<ParticleType>

TPARAMS
KINETIC::Kinetic(value_type particle) noexcept :
  base_type(std::move(particle)) {}

#undef KINETIC
#undef TPARAMS

template class Kinetic<Electron>;
template class Kinetic<ManyElectrons>;
template class Kinetic<Nucleus>;
template class Kinetic<Nuclei>;

} // namespace chemist::qm_operator