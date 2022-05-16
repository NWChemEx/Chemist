#include "chemist/orbital_space/canonical_space.hpp"

namespace chemist::orbital_space {

#define CANONICAL_SPACE CanonicalSpace<OrbitalEnergyType, BaseType>

template<typename OrbitalEnergyType, typename BaseType>
typename CANONICAL_SPACE::const_energy_reference
CANONICAL_SPACE::orbital_energies() const {
    if(m_pegys_) return *m_pegys_;
    throw std::runtime_error("Orbital energies have not been set. Was this "
                             "instance default constructed or moved from?");
}

template<typename OrbitalEnergyType, typename BaseType>
bool CANONICAL_SPACE::operator==(const CanonicalSpace& rhs) const noexcept {
    if(m_pegys_) {
        if(!rhs.m_pegys_) return false; // We have, they dont'
        if(orbital_energies() != rhs.orbital_energies()) return false;
    } else if(rhs.m_pegys_) {
        return false;
    } // We don't have, they have

    // Both have or both don't have energies
    const BaseType& lhs_base = *this;
    const BaseType& rhs_base = rhs;
    return lhs_base == rhs_base;
}

// -----------------------------------------------------------------------------
// ----------------------- Virtual Fxn Overrides -------------------------------
// -----------------------------------------------------------------------------

template<typename OrbitalEnergyType, typename BaseType>
void CANONICAL_SPACE::hash_(chemist::detail_::Hasher& h) const {
    BaseType::hash_(h);
    h(m_pegys_);
}

template<typename OrbitalEnergyType, typename BaseType>
bool CANONICAL_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef CANONICAL_SPACE

template class CanonicalSpace<type::tensor, DerivedSpaceD>;
template class CanonicalSpace<type::tensor_of_tensors, ToTDerivedSpace>;

} // namespace chemist::orbital_space
