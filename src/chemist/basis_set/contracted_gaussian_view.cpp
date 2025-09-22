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
#include "detail_/contracted_gaussian_view_pimpl.hpp"
#include <chemist/basis_set/contracted_gaussian_view.hpp>
#include <utility>

namespace chemist::basis_set {

#define CG_VIEW ContractedGaussianView<CGType>

/// ----------------------------------------------------------------------------
/// -- Ctors/Dtor
/// ----------------------------------------------------------------------------

template<typename CGType>
CG_VIEW::ContractedGaussianView() noexcept = default;

template<typename CGType>
CG_VIEW::ContractedGaussianView(
  size_type n_prims,
  typename primitive_traits::coefficient_reference coef_begin,
  typename primitive_traits::exponent_reference exp_begin,
  typename primitive_traits::center_reference center) :
  ContractedGaussianView(std::make_unique<pimpl_type>(
    n_prims, coef_begin, exp_begin, std::move(center))) {}

template<typename CGType>
CG_VIEW::ContractedGaussianView(apply_const_ref<contracted_gaussian_type> cg) :
  ContractedGaussianView(
    !cg.is_null() ? ContractedGaussianView(cg.size(), cg.at(0).coefficient(),
                                           cg.at(0).exponent(), cg.center()) :
                    ContractedGaussianView()) {}

template<typename CGType>
CG_VIEW::ContractedGaussianView(const ContractedGaussianView& other) :
  m_pimpl_(other.has_pimpl_() ? std::make_unique<pimpl_type>(*other.m_pimpl_) :
                                nullptr) {}

template<typename CGType>
CG_VIEW::ContractedGaussianView(ContractedGaussianView&& other) noexcept =
  default;

template<typename CGType>
CG_VIEW& CG_VIEW::operator=(const ContractedGaussianView& rhs) {
    if(&rhs != this) ContractedGaussianView(rhs).swap(*this);
    return *this;
}

template<typename CGType>
CG_VIEW& CG_VIEW::operator=(ContractedGaussianView&& rhs) noexcept = default;

template<typename CGType>
CG_VIEW::~ContractedGaussianView() noexcept = default;

/// ----------------------------------------------------------------------------
/// -- Getters/Setters
/// ----------------------------------------------------------------------------

template<typename CGType>
typename CG_VIEW::primitive_traits::center_reference CG_VIEW::center() {
    assert_pimpl_();
    return m_pimpl_->center();
}

template<typename CGType>
typename CG_VIEW::primitive_traits::const_center_reference CG_VIEW::center()
  const {
    assert_pimpl_();
    return m_pimpl_->center();
}

template<typename CGType>
typename CG_VIEW::numerical_value CG_VIEW::evaluate(const_point_view r) const {
    numerical_value rv = 0.0;
    for(const auto& prim : *this) { rv += prim.evaluate(r); }
    return rv;
}

template<typename CGType>
std::vector<typename CG_VIEW::numerical_value> CG_VIEW::evaluate(
  const_point_set_view points) const {
    std::vector<numerical_value> results(points.size(), 0.0);
    for(const auto& prim : *this) {
        auto prim_results = prim.evaluate(points);
        for(std::size_t i = 0; i < points.size(); ++i) {
            results[i] += prim_results[i];
        }
    }
    return results;
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

template<typename CGType>
void CG_VIEW::swap(ContractedGaussianView& rhs) noexcept {
    m_pimpl_.swap(rhs.m_pimpl_);
}

template<typename CGType>
bool CG_VIEW::is_null() const noexcept {
    return !has_pimpl_();
}

template<typename CGType>
bool CG_VIEW::operator==(const ContractedGaussianView& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are references to null objects
    return (*m_pimpl_) == *rhs.m_pimpl_;
}

template<typename CGType>
bool CG_VIEW::operator==(const contracted_gaussian_type& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are null objects
    if(this->size() != rhs.size()) return false;
    for(size_type i = 0; i < this->size(); ++i) {
        if((*this)[i] != rhs[i]) return false;
    }
    return true;
}

template<typename CGType>
bool CG_VIEW::operator!=(const ContractedGaussianView& rhs) const noexcept {
    return !(*this == rhs);
}

template<typename CGType>
bool CG_VIEW::operator!=(const contracted_gaussian_type& rhs) const noexcept {
    return !(*this == rhs);
}

// -----------------------------------------------------------------------------
// -- Protected/Private
// -----------------------------------------------------------------------------

template<typename CGType>
CG_VIEW::ContractedGaussianView(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

template<typename CGType>
bool CG_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

template<typename CGType>
void CG_VIEW::assert_non_null_() const {
    if(!is_null()) return;
    throw std::runtime_error(
      "ContractedGaussianView is a view of a null Contracted Gaussian");
}

template<typename CGType>
void CG_VIEW::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("ContractedGaussianView does not have a PIMPL. Did"
                             " you move from it?");
}

template<typename CGType>
typename CG_VIEW::size_type CG_VIEW::size_() const noexcept {
    if(is_null()) return 0;
    return m_pimpl_->size();
}

template<typename CGType>
typename CG_VIEW::reference CG_VIEW::at_(size_type i) noexcept {
    return (*m_pimpl_)[i];
}

template<typename CGType>
typename CG_VIEW::const_reference CG_VIEW::at_(size_type i) const noexcept {
    return std::as_const(*m_pimpl_)[i];
}

#undef CG_VIEW

template class ContractedGaussianView<ContractedGaussianD>;
template class ContractedGaussianView<const ContractedGaussianD>;
template class ContractedGaussianView<ContractedGaussianF>;
template class ContractedGaussianView<const ContractedGaussianF>;

} // namespace chemist::basis_set
