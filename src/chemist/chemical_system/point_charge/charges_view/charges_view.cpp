/*
 * Copyright 2024 NWChemEx-Project
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

#include "detail_/charges_contiguous.hpp"

namespace chemist {

#define TPARAMS template<typename ChargesType>
#define CHARGES_VIEW ChargesView<ChargesType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

TPARAMS
CHARGES_VIEW::ChargesView() noexcept = default;

TPARAMS
CHARGES_VIEW::ChargesView(charges_reference charges) :
  m_pimpl_(std::make_unique<detail_::ChargesContiguous<ChargesType>>(
    charges.point_set(), charges.charge_data())) {}

TPARAMS
CHARGES_VIEW::ChargesView(const ChargesView& other) :
  m_pimpl_(other.clone_pimpl_()) {}

TPARAMS
CHARGES_VIEW& CHARGES_VIEW::operator=(const ChargesView& rhs) {
    if(this != &rhs) rhs.clone_pimpl_().swap(m_pimpl_);
    return *this;
}

TPARAMS
CHARGES_VIEW::ChargesView(ChargesView&& other) noexcept = default;

TPARAMS
CHARGES_VIEW& CHARGES_VIEW::operator=(ChargesView&& rhs) noexcept = default;

TPARAMS
CHARGES_VIEW::~ChargesView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool CHARGES_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

TPARAMS
typename CHARGES_VIEW::pimpl_pointer CHARGES_VIEW::clone_pimpl_() const {
    return has_pimpl_() ? m_pimpl_->clone() : pimpl_pointer{};
}

TPARAMS
typename CHARGES_VIEW::reference CHARGES_VIEW::at_(size_type i) {
    return (*m_pimpl_)[i];
}

TPARAMS
typename CHARGES_VIEW::const_reference CHARGES_VIEW::at_(size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

TPARAMS
typename CHARGES_VIEW::size_type CHARGES_VIEW::size_() const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

#undef CHARGES_VIEW
#undef TPARAMS

template class ChargesView<Charges<float>>;
template class ChargesView<const Charges<float>>;
template class ChargesView<Charges<double>>;
template class ChargesView<const Charges<double>>;

} // namespace chemist
