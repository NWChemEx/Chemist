#include "libchemist/orbital_space/ao_space.hpp"

namespace libchemist::orbital_space {

#define AOSPACE AOSpace<BasisType>

template<typename BasisType>
bool AOSPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

template class AOSpace<AOBasisSetD>;
template class AOSpace<AOBasisSetF>;

#undef AOSPACE
} // namespace libchemist::orbital_space
