#include "libchemist/orbital_space/canonical_space.hpp"
#include "libchemist/ta_helpers/ta_hashers.hpp"

namespace libchemist::orbital_space {

#define CANONICAL_SPACE CanonicalSpace<OrbitalEnergyType, BaseType>

template<typename OrbitalEnergyType, typename BaseType>
void CANONICAL_SPACE::hash_(pluginplay::Hasher& h) const {
    BaseType::hash_(h);
    h(m_egys_);
}

template<typename OrbitalEnergyType, typename BaseType>
bool CANONICAL_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef CANONICAL_SPACE

template class CanonicalSpace<type::tensor, DerivedSpaceD>;
template class CanonicalSpace<type::tensor_of_tensors, ToTDerivedSpace>;

} // namespace libchemist::orbital_space
