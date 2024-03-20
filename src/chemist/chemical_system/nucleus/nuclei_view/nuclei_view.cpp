// /*
//  * Copyright 2023 NWChemEx-Project
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  * http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// #include "detail_/nuclei_subset.hpp"
// #include "detail_/nuclei_view_pimpl.hpp"
// #include <numeric>

// namespace chemist {
// namespace detail_ {

// // TODO: This PIMPL is inefficient for wrapping an entire Nuclei object.
// template<typename NucleiType>
// auto wrap_nuclei(NucleiType& nuclei) {
//     std::vector<std::size_t> buffer(nuclei.size());
//     std::iota(buffer.begin(), buffer.end(), 0);
//     auto pnuclei     = std::make_shared<NucleiType>(nuclei);
//     using pimpl_type = NucleiSubset<NucleiType>;
//     return std::make_unique<pimpl_type>(pnuclei, buffer.begin(),
//     buffer.end());
// }

// } // namespace detail_
// #define TPARAMS template<typename NucleiType>
// #define NUCLEI_VIEW NucleiView<NucleiType>

// //
// -----------------------------------------------------------------------------
// // -- CTors, Assignment, and Dtor
// //
// -----------------------------------------------------------------------------

// TPARAMS
// NUCLEI_VIEW::NucleiView() noexcept = default;

// TPARAMS
// NUCLEI_VIEW::NucleiView(apply_const_ref<nuclei_type> nuclei) :
//   NucleiView(detail_::wrap_nuclei(nuclei)) {}

// TPARAMS NUCLEI_VIEW::NucleiView(pimpl_pointer pimpl) noexcept :
//   m_pimpl_(std::move(pimpl)) {}

// TPARAMS
// NUCLEI_VIEW::NucleiView(const NucleiView& other) :
//   m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

// TPARAMS
// NUCLEI_VIEW::NucleiView(NucleiView&& other) noexcept = default;

// TPARAMS
// NUCLEI_VIEW& NUCLEI_VIEW::operator=(const NucleiView& rhs) {
//     if(this != &rhs) NUCLEI_VIEW(rhs).swap(*this);
//     return *this;
// }

// TPARAMS
// NUCLEI_VIEW& NUCLEI_VIEW::operator=(NucleiView&& rhs) noexcept = default;

// TPARAMS
// NUCLEI_VIEW::~NucleiView() noexcept = default;

// TPARAMS
// void NUCLEI_VIEW::swap(NucleiView& other) noexcept {
//     m_pimpl_.swap(other.m_pimpl_);
// }

// TPARAMS
// bool NUCLEI_VIEW::operator==(const nuclei_type& rhs) const noexcept {
//     if(this->size() != rhs.size()) return false;
//     for(size_type i = 0; i < this->size(); ++i) {
//         if((*this)[i] != rhs[i]) return false;
//     }
//     return true;
// }

// TPARAMS
// bool NUCLEI_VIEW::operator!=(const nuclei_type& rhs) const noexcept {
//     return !(*this == rhs);
// }

// //
// -----------------------------------------------------------------------------
// // -- Protected and Private members
// //
// -----------------------------------------------------------------------------

// TPARAMS
// typename NUCLEI_VIEW::reference NUCLEI_VIEW::at_(size_type i) {
//     return m_pimpl_->get_nuke(i);
// }

// TPARAMS
// typename NUCLEI_VIEW::const_reference NUCLEI_VIEW::at_(size_type i) const {
//     return std::as_const(*m_pimpl_).get_nuke(i);
// }

// TPARAMS
// typename NUCLEI_VIEW::size_type NUCLEI_VIEW::size_() const noexcept {
//     if(has_pimpl_()) return m_pimpl_->size();
//     return 0;
// }

// TPARAMS
// bool NUCLEI_VIEW::has_pimpl_() const noexcept {
//     return static_cast<bool>(m_pimpl_);
// }

// #undef NUCLEI_VIEW
// #undef TPARAMS

// template class NucleiView<Nuclei>;
// template class NucleiView<const Nuclei>;

// } // namespace chemist
