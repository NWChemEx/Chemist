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

#include <chemist/basis_set/contracted_gaussian/contracted_gaussian.hpp>

namespace chemist {
namespace detail_ {

template<typename PrimitiveType>
class ContractedGaussianPIMPL {
    /// Type *this is implementing
    using parent_type = ContractedGaussian<PrimitiveType>;

    /// Pull in the types the parent class uses
    using center_type            = typename parent_type::center_type;
    using center_reference       = typename parent_type::center_reference;
    using const_center_reference = typename parent_type::const_center_reference;
    using coefficient_type       = typename parent_type::coefficient_type;
    using exponent_type          = typename parent_type::exponent_type;
    using reference              = typename parent_type::reference;
    using const_reference        = typename parent_type::const_reference;
    using size_type              = typename parent_type::size_type;

    /// How we have chosen to store the coefficients
    using coefficient_vector = std::vector<coefficient_type>;

    /// How we have chosen to store the exponents
    using exponent_vector = std::vector<exponent_type>;

    ContractedGaussianPIMPL(coefficient_vector cs, exponent_vector exps,
                            center_type r0) :
      m_center_(std::move(r0)),
      m_coefs_(std::move(cs)),
      m_exp_(std::move(exp)) {}

    center_reference center() noexcept { return m_center_; }

    const_center_reference center() const noexcept { return m_center_; }

    reference operator[](size_type i) noexcept {
        return reference(m_coefs_[i], m_exp_[i], m_center_);
    }

    const_reference operator[](size_type i) const noexcept {
        return const_reference(m_coefs_[i], m_exp_[i], m_center_);
    }

    bool operator==(const ContractedGaussian& rhs) const noexcept {
        auto lhs_state = std::tie(m_center_, m_coefs_, m_exp_);
        auto rhs_state = std::tie(rhs.m_center_, rhs.m_coefs_, rhs.m_exp_);
        return lhs_state == rhs_state;
    }

private:
    /// Where the contracted Gaussian is centered
    center_type m_center_;

    /// The contraction coefficients
    coefficient_vector m_coefs_;

    /// The exponents
    exponent_vector m_exp_;
};

} // namespace detail_

#define CG ContractedGaussian<PrimitiveType>

// -----------------------------------------------------------------------------
// -- Ctors and Dtor
// -----------------------------------------------------------------------------

template<typename PrimitiveType>
CG::ContractedGaussian() noexcept = default;

template<typename PrimitiveType>
CG::ContractedGaussian(std::vector<coefficient_type> coefs,
                       std::vector<exponent_type> exps, coord_type x,
                       coord_type y, coord_type z) :
  ContractedGaussian(std::move(coefs), std::move(exps), center_type(x, y, z)) {}

template<typename PrimitiveType>
CG::ContractedGaussian(std::vector<coefficient_type> coefs,
                       std::vector<exponent_type> exps, center_type center) :
  ContractedGaussian(m_pimpl_(std::make_unique<pimpl_type>(
    std::move(coefs), std::move(exps), std::move(center)))) {}

template<typename PrimitiveType>
CG::ContractedGaussian(const my_type& rhs) :
  m_pimpl_(std::make_unique<pimpl_type>(*rhs.m_pimpl_)) {}

template<typename PrimitiveType>
CG::ContractedGaussian(my_type&& rhs) noexcept = default;

template<typename PrimitiveType>
CG& CG::operator=(const my_type& rhs) {
    if(&rhs != this) ContractedGaussian(rhs).swap(*this);
    return *this;
}

template<typename PrimitiveType>
CG& CG::operator=(my_type&& rhs) noexcept = default;

template<typename PrimitiveType>
CG::~ContractedGaussian() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters/Setters
// -----------------------------------------------------------------------------

template<typename PrimitiveType>
typename CG::center_reference CG::center() {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>();
    return m_pimpl_->center();
}

template<typename PrimitiveType>
typename CG::const_center_reference CG::center() const {
    assert_pimpl_();
    return m_pimpl_->center();
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

template<typename PrimitiveType>
void CG::swap(my_type& rhs) noexcept {
    m_pimpl_.swap(rhs.m_pimpl_);
}

template<typename PrimitiveType>
bool CG::operator==(const my_type& rhs) const noexcept {
    // N.B. The base class's operator== is inefficient because it will compare
    //      the centers of each primitive.

    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are null
    return (*m_pimpl_) == (*rhs.m_pimpl_);
}

template<typename PrimitiveType>
bool CG::operator!=(const my_type& rhs) const noexcept {
    return !(*this == rhs);
}

// -----------------------------------------------------------------------------
// -- Protected/Private functions
// -----------------------------------------------------------------------------

template<typename PrimitiveType>
CG::ContractedGaussian(pimpl_ptr my_pimpl) noexcept :
  utilities::IndexableContainerBase<CG>(), m_pimpl_(std::move(my_pimpl)) {}

template<typename PrimitiveType>
bool CG::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

template<typename PrimitiveType>
void CG::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error(
      "ContractedGaussian does not have a PIMPL. This is"
      " usually the case if it was default constructed or moved from.");
}

template<typename PrimitiveType>
typename CG::size_type CG::size_() const noexcept {
    if(is_null()) return 0;
    return m_pimpl_->size();
}

template<typename PrimitiveType>
typename CG::reference CG::at_(size_type i) noexcept {
    return (*m_pimpl_)[i];
}

template<typename PrimitiveType>
typename CG::const_reference CG::at_(size_type i) const noexcept {
    return (*m_pimpl_)[i];
}

#undef CG

template class ContractedGaussian<PrimitiveD>;
template class ContractedGaussian<PrimitiveF>;

} // namespace chemist
