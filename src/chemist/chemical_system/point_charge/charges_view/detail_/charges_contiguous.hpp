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

#pragma once
#include "charges_view_pimpl.hpp"

namespace chemist::detail_ {

/** @brief Used to alias the charges for a Charges object when the charges
 *         are in a contiguous array.
 *
 *  @tparam ChargesType the type *this is acting like it aliases.
 */
template<typename ChargesType>
class ChargesContiguous : public ChargesViewPIMPL<ChargesType> {
private:
    /// Type of *this
    using my_type = ChargesContiguous<ChargesType>;

    /// Type of the base class
    using base_type = ChargesViewPIMPL<ChargesType>;

public:
    using typename base_type::reference;

    using typename base_type::const_reference;

    using typename base_type::point_charge_traits;

    using typename base_type::pimpl_pointer;

    using charges_traits = typename base_type::charges_traits;

    using point_set_traits = typename charges_traits::point_set_traits;

    using point_set_reference = typename point_set_traits::view_type;

    using charge_pointer = typename point_charge_traits::charge_pointer;

    using typename base_type::size_type;

    ChargesContiguous() = default;

    ChargesContiguous(const ChargesContiguous& other) = default;

    ChargesContiguous(point_set_reference points, charge_pointer pcharges) :
      m_points_(std::move(points)), m_pcharges_(pcharges) {}

    bool operator==(const ChargesContiguous& rhs) const noexcept;

protected:
    pimpl_pointer clone_() const { return std::make_unique<my_type>(*this); }

    reference at_(size_type i) noexcept override {
        return reference(m_pcharges_[i], m_points_[i]);
    }

    const_reference at_(size_type i) const noexcept override {
        return const_reference(m_pcharges_[i], m_points_[i]);
    }

    size_type size_() const noexcept override { return m_points_.size(); }

private:
    point_set_reference m_points_;

    charge_pointer m_pcharges_;
};

// -----------------------------------------------------------------------------
// -- Out of line definitions
// -----------------------------------------------------------------------------

template<typename ChargesType>
bool ChargesContiguous<ChargesType>::operator==(
  const ChargesContiguous& rhs) const noexcept {
    // Must have same size
    if(size_() != rhs.size()) return false;

    // If they're both empty, then they are both the same
    if(size_() == 0) return true;

    // Now we now they both have the same non-zero size

    // Start by comparing the points
    if(m_points_ != rhs.m_points_) return false;

    // If aliasing the same memory for the charges then they're the same
    if(m_pcharges_ == rhs.m_pcharges_) return true;

    // Have to manually compare the charges
    return std::equal(m_pcharges_, m_pcharges_ + size_(), rhs.m_pcharges_);
}

} // namespace chemist::detail_
