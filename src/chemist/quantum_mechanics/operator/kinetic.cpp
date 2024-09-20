#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

namespace chemist::qm_operator {

#define TPARAMS template<typename ParticleType>
#define KINETIC Kinetic<ParticleType>

TPARAMS
KINETIC::Kinetic(value_type particle) : base_type(std::move(particle)) {}

TPARAMS void KINETIC::visit_(visitor_reference visitor) const {
    visitor.run(*this);
}

TPARAMS
typename KINETIC::base_pointer KINETIC::clone_() const {
    return std::make_unique<my_type>(*this);
}

#undef KINETIC
#undef TPARAMS

template class Kinetic<Electron>;
template class Kinetic<ManyElectrons>;

} // namespace chemist::qm_operator