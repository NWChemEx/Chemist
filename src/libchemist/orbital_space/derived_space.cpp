#include "detail_/make_indices.hpp"
#include "libchemist/orbital_space/derived_space.hpp"
#include "libchemist/ta_helpers/ta_hashers.hpp"
#include "libchemist/tensor/creation.hpp"
namespace libchemist::orbital_space {

#define DERIVED_SPACE DerivedSpace<TransformType, FromSpace>

template<typename TransformType, typename FromSpace>
typename DERIVED_SPACE::size_type DERIVED_SPACE::size_() const noexcept {
    const auto extents = C().extents();
    if(extents.size() != 2) return 0;
    // Assumes columns are the orbitals
    return extents[1];
}

template<typename TransformType, typename FromSpace>
DERIVED_SPACE DERIVED_SPACE::operator+(const DERIVED_SPACE& rhs) const {
    if(from_space() != rhs.from_space())
        throw std::runtime_error("Must have same from space");

    auto C_new = tensor::concatenate(C(), rhs.C(), 1);
    return DerivedSpace(C_new, from_space_data());
}

template<typename TransformType, typename FromSpace>
void DERIVED_SPACE::hash_(pluginplay::Hasher& h) const {
    h(m_C_, m_pbase_);
}

template<typename TransformType, typename FromSpace>
bool DERIVED_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef DERIVED_SPACE

using Tensor = type::tensor;
using ToT    = type::tensor_of_tensors;
template class DerivedSpace<Tensor, AOSpaceD>;
template class DerivedSpace<ToT, AOSpaceD>;

} // namespace libchemist::orbital_space
