#pragma once
#include "libchemist/operator/density.hpp"
#include "libchemist/operator/detail_/operator_impl.hpp"
#include "libchemist/operator/electron.hpp"

namespace libchemist::operators {

/** @brief Exchange operator
 *
 *  The exchange operator is the product of the particle permutation operator
 *  and the Coulomb operator, i.e. @f$\frac{\widehat{P}_{12}}}{r_{12}}@f$
 */
template<typename Particle1, typename Particle2>
class Exchange : public detail_::OperatorImpl<Exchange, Particle1, Particle2> {
private:
    using base_type = detail_::OperatorImpl<Exchange, Particle1, Particle2>;

public:
    Exchange()                = default;
    Exchange(const Exchange&) = default;
    Exchange(Exchange&&)      = default;
    Exchange(Particle1 p1, Particle2 p2);
    Exchange& operator=(const Exchange&) = default;
    Exchange& operator=(Exchange&&) = default;

protected:
    std::string as_string_impl() const override { return "k\u0302"; }
};

using ElectronEDensityExchange = Exchange<Electron, OneElectronDensity>;

template<typename Particle1, typename Particle2>
Exchange<Particle1, Particle2>::Exchange(Particle1 p1, Particle2 p2) :
  base_type(std::move(p1), std::move(p2)) {}

} // namespace libchemist::operators
