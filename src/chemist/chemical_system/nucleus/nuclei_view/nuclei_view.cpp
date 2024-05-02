/*
 * Copyright 2023 NWChemEx-Project
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

#include "detail_/contiguous_nuclei_view.hpp"
#include "detail_/nuclei_subset.hpp"
#include "detail_/nuclei_union.hpp"
#include "detail_/nuclei_view_pimpl.hpp"
#include "detail_/nucleus_view_list.hpp"
#include <numeric>

namespace chemist {

#define TPARAMS template<typename NucleiType>
#define NUCLEI_VIEW NucleiView<NucleiType>

// -----------------------------------------------------------------------------
// -- CTors, Assignment, and Dtor
// -----------------------------------------------------------------------------

TPARAMS
NUCLEI_VIEW::NucleiView() noexcept = default;

TPARAMS
NUCLEI_VIEW::NucleiView(nuclei_reference nuclei) :
  NucleiView(nuclei.charges(), nuclei.name_data(), nuclei.atomic_number_data(),
             nuclei.mass_data()) {}

TPARAMS
NUCLEI_VIEW::NucleiView(charges_reference charges, name_pointer pnames,
                        atomic_number_pointer patomic_numbers,
                        mass_pointer pmasses) :
  NucleiView(std::make_unique<detail_::ContiguousNucleiView<NucleiType>>(
    charges, pnames, patomic_numbers, pmasses)) {}

TPARAMS
NUCLEI_VIEW::NucleiView(NucleiView supersystem, member_list_type members) :
  NucleiView(std::make_unique<detail_::NucleiSubset<NucleiType>>(
    std::move(supersystem), std::move(members))) {}

TPARAMS
NUCLEI_VIEW::NucleiView(reference_container members) :
  NucleiView(std::make_unique<detail_::NucleusViewList<NucleiType>>(
    std::move(members))) {}

TPARAMS
NUCLEI_VIEW::NucleiView(nuclei_view_container members) :
  NucleiView(
    std::make_unique<detail_::NucleiUnion<NucleiType>>(std::move(members))) {}

TPARAMS NUCLEI_VIEW::NucleiView(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

TPARAMS
NUCLEI_VIEW::NucleiView(const NucleiView& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

TPARAMS
NUCLEI_VIEW::NucleiView(NucleiView&& other) noexcept = default;

TPARAMS
NUCLEI_VIEW& NUCLEI_VIEW::operator=(const NucleiView& rhs) {
    if(this != &rhs) NUCLEI_VIEW(rhs).swap(*this);
    return *this;
}

TPARAMS
NUCLEI_VIEW& NUCLEI_VIEW::operator=(NucleiView&& rhs) noexcept = default;

TPARAMS
NUCLEI_VIEW::~NucleiView() noexcept = default;

TPARAMS
void NUCLEI_VIEW::swap(NucleiView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool NUCLEI_VIEW::operator==(const NucleiView& other) const noexcept {
    if(this->size() != other.size()) return false;
    if(this->size() == 0) return true;
    return this->m_pimpl_->are_equal(*other.m_pimpl_);
}

TPARAMS
bool NUCLEI_VIEW::operator==(const nuclei_type& rhs) const noexcept {
    if(this->size() != rhs.size()) return false;
    for(size_type i = 0; i < this->size(); ++i) {
        if((*this)[i] != rhs[i]) return false;
    }
    return true;
}

TPARAMS
bool NUCLEI_VIEW::operator!=(const nuclei_type& rhs) const noexcept {
    return !(*this == rhs);
}

// -----------------------------------------------------------------------------
// -- Protected and Private members
// -----------------------------------------------------------------------------

TPARAMS
typename NUCLEI_VIEW::reference NUCLEI_VIEW::at_(size_type i) {
    return m_pimpl_->get_nuke(i);
}

TPARAMS
typename NUCLEI_VIEW::const_reference NUCLEI_VIEW::at_(size_type i) const {
    return std::as_const(*m_pimpl_).get_nuke(i);
}

TPARAMS
typename NUCLEI_VIEW::size_type NUCLEI_VIEW::size_() const noexcept {
    if(has_pimpl_()) return m_pimpl_->size();
    return 0;
}

TPARAMS
bool NUCLEI_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

#undef NUCLEI_VIEW
#undef TPARAMS

template class NucleiView<Nuclei>;
template class NucleiView<const Nuclei>;

} // namespace chemist
