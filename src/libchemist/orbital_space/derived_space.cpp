#include "detail_/make_indices.hpp"
#include "libchemist/orbital_space/derived_space.hpp"
#include "libchemist/ta_helpers/ta_hashers.hpp"

namespace libchemist::orbital_space {

#define DERIVED_SPACE DerivedSpace<TransformType, FromSpace, BaseType>

template<typename TransformType, typename FromSpace, typename BaseType>
typename DERIVED_SPACE::size_type DERIVED_SPACE::size_() const noexcept {
    if constexpr(std::is_same_v<BaseType, DependentSpace>) {
        return BaseType::size_();
    } else {
        const auto extents = C().extents();
        if(extents.empty())
            return 0;
        else if(extents.size() != 2)
            throw std::runtime_error("Assuming column vectors");
        // Assumes columns are the orbitals
        return extents[1];
    }
}

// template<typename TransformType, typename FromSpace, typename BaseType>
// type::tensor_wrapper DERIVED_SPACE::transform_(
//   const type::tensor_wrapper& t, const mode_container& modes) const {
//     using traits_type     = libchemist::tensor::TensorTraits<TransformType>;
//     constexpr bool is_tot = traits_type::is_tot;
//     type::tensor_wrapper rv(t);
//     if constexpr(is_tot) {
//         throw std::runtime_error("Can't transform a tensor by a ToT");
//     } else {
//         type::tensor_wrapper c(m_C_);
//         auto c_idx = c.make_annotation();
//         auto t_idx = t.make_annotation("j");

//         for(const auto& modei : modes) {
//             auto [r_idx, rhs_idx] = detail_::make_indices(c_idx, t_idx,
//             modei); rv(r_idx)             = c(c_idx) * t(rhs_idx);
//         }
//     }
//     return rv;
// }

// template<typename TransformType, typename FromSpace, typename BaseType>
// type::tot_wrapper DERIVED_SPACE::transform_(const type::tot_wrapper& t,
//                                             const mode_container& modes)
//                                             const {
//     type::tot_wrapper rv(t);
//     using traits_type     = libchemist::tensor::TensorTraits<TransformType>;
//     constexpr bool is_tot = traits_type::is_tot;
//     using wrapper_t =
//       std::conditional_t<is_tot, type::tot_wrapper, type::tensor_wrapper>;
//     wrapper_t c(m_C_);
//     auto c_idx = c.make_annotation();
//     auto t_idx = t.make_annotation("j");

//     for(const auto& modei : modes) {
//         auto [r_idx, rhs_idx] = detail_::make_indices(c_idx, t_idx, modei);
//         rv(r_idx)             = c(c_idx) * t(rhs_idx);
//     }
//     return rv;
// }

template<typename TransformType, typename FromSpace, typename BaseType>
void DERIVED_SPACE::hash_(sde::Hasher& h) const {
    BaseType::hash_(h);
    h(m_C_, m_pbase_);
}

template<typename TransformType, typename FromSpace, typename BaseType>
bool DERIVED_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef DERIVED_SPACE

using TensorD = type::tensor;
using ToTD    = type::tensor_of_tensors;
template class DerivedSpace<TensorD, AOSpaceD, BaseSpace>;
template class DerivedSpace<TensorD, DepAOSpaceD, BaseSpace>;
template class DerivedSpace<ToTD, DepAOSpaceD, DependentSpace>;

} // namespace libchemist::orbital_space
