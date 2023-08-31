/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chemist/basis_set/primitive/primitive_view.hpp>

namespace chemist::basis_set {

#define PRIMITIVE_TPARAMS template<typename PrimitiveType>
#define PRIMITIVE_VIEW PrimitiveView<PrimitiveType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

PRIMITIVE_TPARAMS
PRIMITIVE_VIEW::PrimitiveView(apply_const_ref<primitive_type> prim) noexcept :
  PrimitiveView(
    !prim.is_null() ?
      PrimitiveView(prim.coefficient(), prim.exponent(), prim.center()) :
      PrimitiveView()) {}

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

PRIMITIVE_TPARAMS
typename PRIMITIVE_VIEW::center_reference PRIMITIVE_VIEW::center() {
    assert_non_null_();
    return m_center_.value();
}

PRIMITIVE_TPARAMS
typename PRIMITIVE_VIEW::const_center_reference PRIMITIVE_VIEW::center() const {
    assert_non_null_();
    return m_center_.value();
}

PRIMITIVE_TPARAMS
typename PRIMITIVE_VIEW::coefficient_reference PRIMITIVE_VIEW::coefficient() {
    assert_non_null_();
    return *m_coef_;
}

PRIMITIVE_TPARAMS
typename PRIMITIVE_VIEW::const_coefficient_reference
PRIMITIVE_VIEW::coefficient() const {
    assert_non_null_();
    return *m_coef_;
}

PRIMITIVE_TPARAMS
typename PRIMITIVE_VIEW::exponent_reference PRIMITIVE_VIEW::exponent() {
    assert_non_null_();
    return *m_exp_;
}

PRIMITIVE_TPARAMS
typename PRIMITIVE_VIEW::const_exponent_reference PRIMITIVE_VIEW::exponent()
  const {
    assert_non_null_();
    return *m_exp_;
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

PRIMITIVE_TPARAMS
void PRIMITIVE_VIEW::swap(PrimitiveView& other) noexcept {
    m_center_.swap(other.m_center_);
    std::swap(m_coef_, other.m_coef_);
    std::swap(m_exp_, other.m_exp_);
}

PRIMITIVE_TPARAMS
bool PRIMITIVE_VIEW::is_null() const noexcept { return !m_center_.has_value(); }

PRIMITIVE_TPARAMS
bool PRIMITIVE_VIEW::operator==(const PrimitiveView& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are null
    if(m_center_ != rhs.m_center_) return false;
    if(coefficient() != rhs.coefficient()) return false;
    if(exponent() != rhs.exponent()) return false;
    return true;
}

PRIMITIVE_TPARAMS
bool PRIMITIVE_VIEW::operator==(const primitive_type& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are null
    if(m_center_ != rhs.center()) return false;
    if(coefficient() != rhs.coefficient()) return false;
    if(exponent() != rhs.exponent()) return false;
    return true;
}

// -----------------------------------------------------------------------------
// -- Protected/Private functions
// -----------------------------------------------------------------------------

PRIMITIVE_TPARAMS
void PRIMITIVE_VIEW::assert_non_null_() const {
    if(!is_null()) return;
    throw std::runtime_error("This is a view of a null primitive.");
}

#undef PRIMITIVE_VIEW
#undef PRIMITIVE_TPARAMS

template class PrimitiveView<PrimitiveD>;
template class PrimitiveView<const PrimitiveD>;
template class PrimitiveView<PrimitiveF>;
template class PrimitiveView<const PrimitiveF>;

} // namespace chemist::basis_set
