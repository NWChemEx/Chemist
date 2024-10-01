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
 *  This class stores the PointSet part of the Charges object as a
 *  PointSetView and the literal charge values in a contiguous array.
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
    /// Type of a mutable view of a point charge
    using typename base_type::reference;

    /// Type of a read-only view of a point charge
    using typename base_type::const_reference;

    /// Traits class defining types associated with a PointCharge object
    using typename base_type::point_charge_traits;

    /// Type of a pointer to the base of *this
    using typename base_type::pimpl_pointer;

    /// Traits class defining types associated with a Charges object
    using charges_traits = typename base_type::charges_traits;

    /// Traits class defining types associated with a PointSet object
    using point_set_traits = typename charges_traits::point_set_traits;

    /// Type of a mutable reference to the PointSet piece of *this
    using point_set_reference = typename point_set_traits::view_type;

    /// Type of a read-only reference to the PointSet piece of *this
    using const_point_set_reference =
      typename point_set_traits::const_view_type;

    /// Type of a mutable pointer to a charge
    using charge_pointer = typename point_charge_traits::charge_pointer;

    /// Type used for indexing and offsets
    using typename base_type::size_type;

    /** @brief Creates an empty ChargesContiguous object.
     *
     *  The object created with this ctor acts like it aliases an empty Charges
     *  object.
     *
     *  @throw None No throw guarantee
     */
    ChargesContiguous() = default;

    /** @brief Creates a copy of *this which aliases the same state.
     *
     *  @param[in] other The object we want to copy.
     *
     *  @throw std::bad_alloc if there is an issue copying the reference to
     *         other's PointSet. Strong throw guarantee.
     */
    ChargesContiguous(const ChargesContiguous& other) = default;

    /** @brief Creates *this from existing state.
     *
     *  @param[in] points A reference to what will be the PointSet part of
     *                    *this.
     *  @param[in] pcharges A pointer to the first point charge's charge. It is
     *                      assumed that the charge of the i-th point charge
     *                      can be obtained as pcharges[i].
     *
     *  @throw None No throw guarantee.
     */
    ChargesContiguous(point_set_reference points, charge_pointer pcharges) :
      m_points_(std::move(points)), m_pcharges_(pcharges) {}

    /** @brief Compares two ChargesContiguous objects for equality.
     *
     *  This method compares the Charges object aliased by *this to that being
     *  aliased by @p rhs. The method will return true if the aliased Charges
     *  objects compare equal. In particular, note that this does NOT require
     *  the aliased Charged objects to use the same memory (as comparing the
     *  ChargesContiguous objects would).
     *
     *  @param[in] rhs The object we compare to.
     *
     *  @return True if *this compares equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ChargesContiguous& rhs) const noexcept;

protected:
    /// Simply calls the copy ctor
    pimpl_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    /// Creates a mutable reference on the fly
    reference at_(size_type i) noexcept override {
        return reference(m_pcharges_[i], m_points_[i]);
    }

    /// Creates a read-only reference on the fly
    const_reference at_(size_type i) const noexcept override {
        return const_reference(m_pcharges_[i], m_points_[i]);
    }

    /// Returns the PointSetView used to implement *this
    point_set_reference point_set_() override { return m_points_; }

    /// Returns a read-only view of the PointSetView used to implement *this
    const_point_set_reference point_set() const override { return m_points_; }

    /// Defers to the PointSet piece of *this for the number of point charges
    size_type size_() const noexcept override { return m_points_.size(); }

    /// Calls the base class's are_equal_impl_ to implement are_equal
    bool are_equal_(const base_type& rhs) const noexcept override {
        return base_type::template are_equal_impl_<my_type>(rhs);
    }

private:
    /// Mutable reference to the PointSet part of *this
    point_set_reference m_points_;

    /// Pointer to the first point charge's charge
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
