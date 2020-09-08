#pragma once
#include "libchemist/orbital_space/derived_space.hpp"

namespace libchemist::orbital_space {

template<typename Traits, typename BaseType = AOSpace_<Traits>>
class CanonicalSpace_ : public DerivedSpace_<Traits, BaseType> {
public:
    using orbital_energy_type = typename Traits::orbital_energy_type;
private:
    orbital_energy_type m_egys_;
};

} // namespace libchemist::orbital_space
