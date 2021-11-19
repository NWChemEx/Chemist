#include "chemist/orbital_space/ao_space.hpp"

namespace chemist::orbital_space {

#define AOSPACE AOSpace<BasisType>

template<typename BasisType>
bool AOSPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

template class AOSpace<AOBasisSetD>;
template class AOSpace<AOBasisSetF>;

#undef AOSPACE
} // namespace chemist::orbital_space
