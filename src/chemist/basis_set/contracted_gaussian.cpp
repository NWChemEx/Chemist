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
#include "detail_/contracted_gaussian_pimpl.hpp"
#include <chemist/basis_set/contracted_gaussian.hpp>

namespace chemist::basis_set {

#define CG ContractedGaussian<PrimitiveType>

// -----------------------------------------------------------------------------
// -- Ctors and Dtor
// -----------------------------------------------------------------------------

template<typename PrimitiveType>
CG::ContractedGaussian() noexcept = default;

template<typename PrimitiveType>
CG::ContractedGaussian(const my_type& rhs) :
  m_pimpl_(rhs.has_pimpl_() ? std::make_unique<pimpl_type>(*rhs.m_pimpl_) :
                              nullptr) {}

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
typename CG::primitive_traits::center_reference CG::center() {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>();
    return m_pimpl_->center();
}

template<typename PrimitiveType>
typename CG::primitive_traits::const_center_reference CG::center() const {
    assert_pimpl_();
    return m_pimpl_->center();
}

template<typename PrimitiveType>
typename CG::numerical_value CG::evaluate(const_point_view r) const {
    numerical_value rv = 0.0;
    for(const auto& prim : *this) { rv += prim.evaluate(r); }
    return rv;
}

template<typename PrimitiveType>
typename CG::numerical_vector CG::evaluate(const_point_set_view points) const {
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
CG::ContractedGaussian(coefficient_vector cs, exponent_vector es,
                       typename primitive_traits::center_type center) :
  ContractedGaussian(std::make_unique<pimpl_type>(std::move(cs), std::move(es),
                                                  std::move(center))) {}

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

} // namespace chemist::basis_set
