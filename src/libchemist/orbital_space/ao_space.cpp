#include "libchemist/orbital_space/ao_space.hpp"

namespace libchemist::orbital_space {

#define AOSPACE AOSpace<BasisType, BaseType>

template<typename BasisType, typename BaseType>
typename AOSPACE::size_type AOSPACE::size_() const noexcept {
    if constexpr(std::is_same_v<BaseType, BaseSpace>) {
        return m_bs_.size();
    } else {
        return BaseType::size_();
    }
}

template<typename BasisType, typename BaseType>
type::tensor_wrapper AOSPACE::transform_(const type::tensor_wrapper& t,
                                         const mode_container&) const {
    return t;
}

template<typename BasisType, typename BaseType>
void AOSPACE::hash_(sde::Hasher& h) const {
    BaseType::hash_(h);
    h(m_bs_);
}

template<typename BasisType, typename BaseType>
bool AOSPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

template class AOSpace<AOBasisSetD, BaseSpace>;
template class AOSpace<AOBasisSetF, BaseSpace>;
template class AOSpace<AOBasisSetD, DependentSpace>;
template class AOSpace<AOBasisSetF, DependentSpace>;

#undef AOSPACE
} // namespace libchemist::orbital_space