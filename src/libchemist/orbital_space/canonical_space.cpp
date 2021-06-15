#include "libchemist/orbital_space/canonical_space.hpp"
#include "libchemist/ta_helpers/ta_hashers.hpp"

namespace libchemist::orbital_space {

#define CANONICAL_SPACE CanonicalSpace<OrbitalEnergyType, BaseType>

template<typename OrbitalEnergyType, typename BaseType>
template<typename... Args>
CANONICAL_SPACE::CanonicalSpace(OrbitalEnergyType egys, Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_egys_(std::move(egys)) {}

template<typename OrbitalEnergyType, typename BaseType>
void CANONICAL_SPACE::hash_(sde::Hasher& h) const {
    BaseType::hash_(h);
    h(m_egys_);
}

template<typename OrbitalEnergyType, typename BaseType>
bool CANONICAL_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef CANONICAL_SPACE

template class CanonicalSpace<type::tensor<double>, DerivedSpaceD>;
template class CanonicalSpace<type::tensor<double>, IndDerivedSpaceD>;
template class CanonicalSpace<type::tensor_of_tensors<double>,
                              DepDerivedSpaceD>;

} // namespace libchemist::orbital_space