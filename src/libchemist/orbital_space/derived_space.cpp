#include "libchemist/orbital_space/derived_space.hpp"
#include "libchemist/ta_helpers/ta_hashers.hpp"
#include "detail_/make_indices.hpp"

namespace libchemist::orbital_space {

#define DERIVED_SPACE DerivedSpace<TransformType, FromSpace, BaseType>

template<typename TransformType, typename FromSpace, typename BaseType>
template<typename... Args>
DERIVED_SPACE::DerivedSpace(transform_type C, from_space_type base,
                            Args&&... args) :
  DerivedSpace(std::move(C), std::make_shared<from_space_type>(std::move(base)),
               std::forward<Args>(args)...) {}

template<typename TransformType, typename FromSpace, typename BaseType>
template<typename... Args>
DERIVED_SPACE::DerivedSpace(transform_type C, from_space_ptr pbase,
                            Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_C_(std::move(C)), m_pbase_(pbase) {}

template<typename TransformType, typename FromSpace, typename BaseType>
typename DERIVED_SPACE::size_type DERIVED_SPACE::size_() const noexcept {
    if constexpr(std::is_same_v<BaseType, DependentSpace>) {
        return BaseType::size_();
    } else {
        const bool init = m_C_.is_initialized();
        // Assumes columns are the orbitals
        return init ? m_C_.trange().dim(1).extent() : 0;
    }
}

template<typename TransformType, typename FromSpace, typename BaseType>
type::tensor_wrapper DERIVED_SPACE::transform_(
  const type::tensor_wrapper& t, const mode_container& modes) const {
    type::tensor_wrapper rv(t);
    type::tensor_wrapper c(m_C_);
    auto c_idx = c.make_annotation();
    auto t_idx = t.make_annotation("j");

    // for(const auto& modei : modes) {
    //     auto [r_idx, rhs_idx] = detail_::make_indices(c_idx, t_idx, modei);
    //     rv(r_idx)             = c(c_idx) * t(rhs_idx);
    // }
    return rv;
}

template<typename TransformType, typename FromSpace, typename BaseType>
void DERIVED_SPACE::hash_(sde::Hasher& h) const {
    BaseType::hash_(h);
    h(m_C_, *m_pbase_);
}

template<typename TransformType, typename FromSpace, typename BaseType>
bool DERIVED_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef DERIVED_SPACE

using TensorD = type::tensor<double>;
using ToTD    = type::tensor_of_tensors<double>;
template class DerivedSpace<TensorD, AOSpaceD, BaseSpace>;
template class DerivedSpace<TensorD, DepAOSpaceD, BaseSpace>;
template class DerivedSpace<ToTD, DepAOSpaceD, DependentSpace>;

} // namespace libchemist::orbital_space
