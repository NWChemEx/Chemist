#include <chemist/quantum_mechanics/operator/kinetic.hpp>

namespace chemist::qm_operator {

#define TPARAMS template<typename Particle>
#define KINETIC

TPARAMS
void KINETIC::visit_(visitor_reference visitor) const { visitor.run(*this); }

#undef KINETIC
#undef TPARAMS

template class Kinetic<Electron>

}