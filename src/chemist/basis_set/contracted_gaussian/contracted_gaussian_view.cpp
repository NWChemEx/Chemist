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

#include <chemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp>

namespace chemist {
namespace detail_ {

// Design note: we went with PIMPL because depending on the guarantees on the
//              data we can get better performance
template<typename CGType>
class ContractedGaussianViewPIMPL {
public:
    /// Type of *this
    using my_type = ContractedGaussianViewPIMPL<CGType>;

    /// Traits type used to do the TMP
    using traits_type = ViewTraits<CGType>;

    /// Avoid needing "typename" and "template" below
    template<typename T>
    using ptr_type = typename traits_type::template apply_const_ptr<T>;

    /// Type *this is implementing
    using parent_type = ContractedGaussianView<CGType>;

    /// Type used to hold a contraction coefficient
    using coefficient_type = typename parent_type::coefficient_type;

    /// Type of a possibly mutable pointer to a coefficient
    using coefficient_pointer = ptr_type<coefficient_point>;

    /// Type used to hold an exponent
    using exponent_type = typename parent_type::exponent_type;

    /// Type of a possibly mutable pointer to an exponent
    using exponent_pointer = ptr_type<exponent_type>;

    /// Type of a pointer to an object of type *this
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    /// Makes a deep copy of *this, on the heap
    pimpl_pointer clone() const { return std::make_unique<my_type>(*this); }

    bool operator==(const ContractedGaussianViewPIMPL& rhs) const noexcept {
        if(m_center != rhs.m_center) return false;
        if(size() != rhs.size()) return false;
        for(decltype(m_coefs.size()) i = 0; i < m_coefs.size(); ++i) {
            if(*m_coefs[i] != *rhs.m_coefs[i]) return false;
            if(*m_exps[i] != *rhs.m_exps[i]) return false;
        }
    }

    /// Pointers to the coefficients
    std::vector<coefficient_pointer> m_coefs;

    /// Pointers to the exponents
    std::vector<exponent_pointer> m_exps;

    /// The center common to all contracted gaussians
    center_reference m_center;
};

} // namespace detail_

#define CG_VIEW ContractedGaussianView<CGType>

/// ----------------------------------------------------------------------------
/// -- Ctors/Dtor
/// ----------------------------------------------------------------------------

template<typename CGType>
CG_VIEW::ContractedGaussianView(size_type n_prims,
                                coefficient_reference coef_begin,
                                exponent_reference exp_begin,
                                center_reference center) {
    auto pimpl = std::make_unique<pimpl_type>();
    for(size_type i = 0; i < n_prims; ++i) {
        pimpl->m_coef.push_back(&coef_begin + i);
        pimpl->m_exp.push_back(&exp_begin + i);
    }
    pimpl->m_center(center);
    m_pimpl_.swap(pimpl);
}

template<typename CGType>
CG_VIEW::ContractedGaussianView(const ContractedGaussianView& other) :
  m_pimpl_(rhs.has_pimpl_ ? other.m_pimpl_->clone() : nullptr) {}

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

CG_VIEW::~ContractedGaussianView() noexcept = default;

/// ----------------------------------------------------------------------------
/// -- Getters/Setters
/// ----------------------------------------------------------------------------

template<typename CGType>
typename CG_VIEW::center_reference CG_VIEW::center() {
    assert_pimpl_();
    return m_pimpl_->m_center;
}

template<typename CGType>
typename CG_VIEW::const_center_reference CG_VIEW::center() const {
    assert_pimpl_();
    return m_pimpl_->m_center;
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

template<typename CGType>
void CG_VIEW::swap(ContractedGaussianView& rhs) noexcept {
    m_pimpl_.swap(rhs.m_pimpl_);
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
    if(size() != rhs.size()) return false;
    for(decltype(size()) i = 0; i < size(); ++i) {
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
CG_VIEW::ContractedGaussianView(pimpl_pointer pimpl) :
  m_pimpl_(std::move(pimpl)) {}

template<typename CGType>
bool CG_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

template<typename CGType>
void CG_VIEW::assert_pimpl_() const {
    if(has_pimpl_()) return;
  throw std::runtime_error("ContractedGaussianView does not have a PIMPL. Did
                            you move from it?");
}

template<typename CGType>
typename CG_VIEW::size_type CG_VIEW::size_() const noexcept {
    if(!is_null()) return 0;
    return m_pimpl_->m_coefs_.size();
}

reference at_(size_type i) noexcept {
    auto c_i   = m_pimpl_->m_coefs_[i];
    auto exp_i = m_pimpl_->m_exps_[i];
    return reference(*c_i, *exp_i, m_pimpl_->m_center_);
}

const_reference at_(size_type i) const noexcept {
    auto c_i   = m_pimpl_->m_coefs_[i];
    auto exp_i = m_pimpl_->m_exps_[i];
    return const_reference(*m_coefs_[i], *m_exps_[i], m_center_);
}

#undef CG_VIEW

template class ContractedGaussianView<ContractedGaussianD>;
template class ContractedGaussianView<const ContractedGaussianD>;
template class ContractedGaussianView<ContractedGaussianF>;
template class ContractedGaussianView<const ContractedGaussianF>;

} // namespace chemist
