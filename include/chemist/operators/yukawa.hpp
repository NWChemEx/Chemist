#pragma once
#include "chemist/electrons/electrons.hpp"
#include "chemist/operators/detail_/operator_impl.hpp"
#include "chemist/operators/slater_type_geminal.hpp"

namespace chemist::operators {

/** @brief Models the Yukawa operator
 *
 *  @tparam Geminal The type of the geminal relating the two particles.
 *  @tparam Particle1 The type of the first particle.
 *  @tparam Particle2 The type of the second particle.
 */
template<typename Geminal, typename Particle1, typename Particle2>
class Yukawa
  : public detail_::OperatorImpl<Yukawa, Geminal, Particle1, Particle2> {
private:
    using base_type =
      detail_::OperatorImpl<Yukawa, Geminal, Particle1, Particle2>;

public:
    Yukawa()              = default;
    Yukawa(const Yukawa&) = default;
    Yukawa(Yukawa&&)      = default;
    Yukawa(Geminal g, Particle1 p1, Particle2 p2) :
      base_type(std::move(g), std::move(p1), std::move(p2)) {}
    Yukawa& operator=(const Yukawa&) = default;
    Yukawa& operator=(Yukawa&&) = default;
};

/// Type of the Electronic Yukawa operator
using ElectronElectronYukawa =
  Yukawa<STG, chemist::Electron, chemist::Electron>;

} // namespace chemist::operators
