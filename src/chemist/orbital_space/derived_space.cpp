#include "chemist/orbital_space/derived_space.hpp"
#include "detail_/make_indices.hpp"
#include <tensorwrapper/tensor/creation.hpp>

namespace chemist::orbital_space {

#define DERIVED_SPACE DerivedSpace<TransformType, FromSpace>

// -----------------------------------------------------------------------------
// ---------------------------- CTORS ------------------------------------------
// -----------------------------------------------------------------------------

template<typename TransformType, typename FromSpace>
DERIVED_SPACE::DerivedSpace(transform_type C, from_space_type base) :
  DerivedSpace(std::move(C),
               std::make_shared<from_space_type>(std::move(base))) {}

template<typename TransformType, typename FromSpace>
DERIVED_SPACE::DerivedSpace(transform_type C, from_space_ptr pbase) :
  DerivedSpace(std::make_shared<transform_type>(std::move(C)), pbase) {}

template<typename TransformType, typename FromSpace>
DERIVED_SPACE::DerivedSpace(transform_ptr pC, from_space_ptr pbase) :
  m_pC_(pC), m_pbase_(pbase) {}

// -----------------------------------------------------------------------------
// --------------------------------- Accessors ---------------------------------
// -----------------------------------------------------------------------------

template<typename TransformType, typename FromSpace>
typename DERIVED_SPACE::const_transform_reference DERIVED_SPACE::C() const {
    if(m_pC_) return *m_pC_;
    throw std::runtime_error("No transformation set. Was this instance default "
                             "initialized or moved from?");
}

template<typename TransformType, typename FromSpace>
typename DERIVED_SPACE::const_from_space_reference DERIVED_SPACE::from_space()
  const {
    if(m_pbase_) return *m_pbase_;
    throw std::runtime_error("No from space set. Was this instance default "
                             "initialized or moved from?");
}

// -----------------------------------------------------------------------------
// ------------------------- Operators -----------------------------------------
// -----------------------------------------------------------------------------

template<typename TransformType, typename FromSpace>
DERIVED_SPACE DERIVED_SPACE::operator+(const DERIVED_SPACE& rhs) const {
    if(from_space() != rhs.from_space())
        throw std::runtime_error("Must have same from space");

    auto C_new = tensorwrapper::tensor::concatenate(C(), rhs.C(), 1);
    return DerivedSpace(C_new, from_space_data());
}

template<typename TransformType, typename FromSpace>
bool DERIVED_SPACE::operator==(const DerivedSpace& rhs) const noexcept {
    auto plhs = from_space_data();
    auto prhs = rhs.from_space_data();
    if(plhs && prhs) { // Both have from_spaces
        if(from_space() != rhs.from_space()) return false;
    } else if(plhs && !prhs)
        return false; // LHS has from, RHS doesn't
    else if(!plhs && prhs)
        return false; // RHS has from, LHS doesn't
    // else is both don't have from-space

    if(size() != rhs.size()) return false;
    // We know they either both have orbitals or both don't
    // else case here covers they both don't
    return size() ? C() == rhs.C() : true;
}

// -----------------------------------------------------------------------------
// ------------------------- Virtual Fxn Overrides -----------------------------
// -----------------------------------------------------------------------------

template<typename TransformType, typename FromSpace>
typename DERIVED_SPACE::size_type DERIVED_SPACE::size_() const noexcept {
    if(!m_pC_) return 0;
    // Tensor case
    const auto extents = C().extents();
    return extents.size() == 2 ? extents[1] : 0;
}

template<typename TransformType, typename FromSpace>
void DERIVED_SPACE::hash_(chemist::detail_::Hasher& h) const {
    h(m_pC_, m_pbase_);
}

template<typename TransformType, typename FromSpace>
bool DERIVED_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef DERIVED_SPACE

// -----------------------------------------------------------------------------
// ----------------------------- Template Instantiations -----------------------
// -----------------------------------------------------------------------------

using Tensor = type::tensor;
using ToT    = type::tensor_of_tensors;
template class DerivedSpace<Tensor, AOSpaceD>;
template class DerivedSpace<ToT, AOSpaceD>;

} // namespace chemist::orbital_space
