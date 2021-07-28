#pragma once
#include "libchemist/operator/density.hpp"
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist {

/** @brief Exchange operator
 *
 *  The exchange operator is the product of the particle permutation operator
 *  and the Coulomb operator, i.e. @f$\frac{\widehat{P}_{12}}}{r_{12}}@f$
 */
template<typename Particle1, typename Particle2>
class Exchange : public detail_::OperatorImpl<Exchange, Particle1, Particle2> {
};

using ElectronEDensityExchange = Exchange<Electron, OneElectronDensity>;

} // namespace libchemist
