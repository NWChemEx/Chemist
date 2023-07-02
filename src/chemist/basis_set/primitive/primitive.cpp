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

#include <chemist/basis_set/primitive/primitive.hpp>
#include <stdexcept>

namespace chemist {
namespace detail_ {

template<typename T>
struct PrimitivePIMPL {
    /// Type *this implements
    using parent_type = Primitive<T>;

    /// Pull in parent's types
    using center_type      = typename parent_type::center_type;
    using coefficient_type = typename parent_type::coefficient_type;
    using exponent_type    = typename parent_type::exponent_type;

    bool operator==(const PrimitivePIMPL& rhs) const noexcept {
        auto lhs_state = std::tie(m_center, m_coefficient, m_exponent);
        auto rhs_state =
          std::tie(rhs.m_center, rhs.m_coefficient, rhs.m_exponent);
        return lhs_state == rhs_state;
    }

    center_type m_center{};
    coefficient_type m_coefficient{};
    exponent_type m_exponent{};
};

} // namespace detail_

#define PRIM_TYPE Primitive<T>

// -----------------------------------------------------------------------------
// -- Ctors/Dtor
// -----------------------------------------------------------------------------

template<typename T>
PRIM_TYPE::Primitive() noexcept = default;

template<typename T>
PRIM_TYPE::Primitive(T coef, T exp, T x, T y, T z) :
  Primitive(std::make_unique<pimpl_type>(center_type(x, y, z), coef, exp)) {}

template<typename T>
PRIM_TYPE::Primitive(const PRIM_TYPE& rhs) :
  Primitive(has_pimpl_() ? std::make_unique<pimpl_type>(*rhs.m_pimpl_) :
                           nullptr) {}

template<typename T>
PRIM_TYPE::Primitive(PRIM_TYPE&& rhs) noexcept = default;

template<typename T>
PRIM_TYPE& PRIM_TYPE::operator=(const PRIM_TYPE& rhs) noexcept {
    if(&rhs != this) Primitive(rhs).swap(*this);
    return *this;
}

template<typename T>
PRIM_TYPE& PRIM_TYPE::operator=(PRIM_TYPE&& rhs) noexcept = default;

template<typename T>
PRIM_TYPE::~Primitive() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters/Setters
// -----------------------------------------------------------------------------

template<typename T>
typename PRIM_TYPE::center_reference PRIM_TYPE::center() {
    if(!has_pimpl_()) m_pimpl_ = make_pimpl_();
    return m_pimpl_->m_center;
}

template<typename T>
typename PRIM_TYPE::coefficient_reference PRIM_TYPE::coefficient() {
    if(!has_pimpl_()) m_pimpl_ = make_pimpl_();
    return m_pimpl_->m_coefficient;
}

template<typename T>
typename PRIM_TYPE::const_coefficient_reference PRIM_TYPE::coefficient() const {
    assert_pimpl_();
    return m_pimpl_->m_coefficient;
}

template<typename T>
typename PRIM_TYPE::exponent_reference PRIM_TYPE::exponent() {
    if(!has_pimpl_()) m_pimpl_ = make_pimpl_();
    return m_pimpl_->m_exponent;
}

template<typename T>
typename PRIM_TYPE::const_exponent_reference PRIM_TYPE::exponent() const {
    assert_pimpl_();
    return m_pimpl_->m_exponent;
}

// -----------------------------------------------------------------------------
// -- Utility Functions
// -----------------------------------------------------------------------------

template<typename T>
void PRIM_TYPE::swap(PRIM_TYPE& rhs) noexcept {
    m_pimpl_.swap(rhs.m_pimpl_);
}

template<typename T>
bool PRIM_TYPE::operator==(const PRIM_TYPE& rhs) const noexcept {
    if(has_pimpl_() != rhs.has_pimpl_()) return false;
    if(!has_pimpl_()) return true; // Both are null
    return *m_pimpl_ == *rhs.m_pimpl_;
}

// -----------------------------------------------------------------------------
// -- Protected/Private
// -----------------------------------------------------------------------------

template<typename T>
PRIM_TYPE::Primitive(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

template<typename T>
void PRIM_TYPE::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("Primitive<T> does not have a PIMPL.");
}

template<typename T>
typename PRIM_TYPE::pimpl_pointer PRIM_TYPE::make_pimpl_() const {
    return std::make_unique<pimpl_type>();
}

template<typename T>
bool PRIM_TYPE::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

#undef PRIM_TYPE

template class Primitive<double>;
template class Primitive<float>;

} // namespace chemist
