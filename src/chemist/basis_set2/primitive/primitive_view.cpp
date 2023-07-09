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

#include <chemist/basis_set2/primitive/primitive_view.hpp>

namespace chemist::basis_set {

#define PRIMITIVE_TPARAMS template<typename PrimitiveType>
#define PRIMITIVE_VIEW PrimitiveView<PrimitiveType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

PRIMITIVE_TPARAMS
PRIMITIVE_VIEW::PrimitiveView(
  apply_const_ref<typename traits_type::type> prim) noexcept :
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
void PRIMITIVE_VIEW::assert_non_null_() const {
    if(!is_null()) return;
    throw std::runtime_error("This is a view of a null primitive.");
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

PRIMITIVE_TPARAMS
bool PRIMITIVE_VIEW::is_null() const noexcept { return !m_center_.has_value(); }

#undef PRIMITIVE_VIEW
#undef PRIMITIVE_TPARAMS

template class PrimitiveView<PrimitiveD>;
template class PrimitiveView<const PrimitiveD>;
template class PrimitiveView<PrimitiveF>;
template class PrimitiveView<const PrimitiveF>;

} // namespace chemist::basis_set
