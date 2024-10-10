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
#include "point_set_view_pimpl.hpp"
#include <memory>

namespace chemist::detail_ {

/** @brief Implements a PointSetView by holding pointers to contiguous memory.
 *
 *  @tparam PointSetType Type *this is a view of.
 *
 */
template<typename PointSetType>
class PointSetContiguous : public PointSetViewPIMPL<PointSetType> {
private:
    /// Type *this derives from
    using base_type = PointSetViewPIMPL<PointSetType>;

    /// Type of *this
    using my_type = PointSetContiguous<PointSetType>;

public:
    /// Type of a pointer to a coordinate
    using coord_pointer = typename base_type::point_traits_type::coord_pointer;

    /// Type used for indexing and offsets
    using typename base_type::size_type;

    /// Type used for a mutable reference to a Point
    using typename base_type::reference;

    /// Type used for a read-only reference to a Point
    using typename base_type::const_reference;

    /// Type of a pointer to PIMPL's API
    using typename base_type::pimpl_pointer;

    /** @brief Creates an empty PointSet.
     *
     *  The object resulting from this ctor is capable of implementing an
     *  empty PointSet.
     *
     *  @throw None No throw guarantee.
     */
    PointSetContiguous() = default;

    /** @brief Creates a PointSet which aliases @p n_points contiguous points.
     *
     *  @param[in] n_points The number of points to be managed by *this.
     *  @param[in] px A pointer to the x-coordinate of the 0-th point. The x-
     *                coordinate of the i-th point is assumed to be *(px + i).
     *  @param[in] py A pointer to the y-coordinate of the 0-th point. The y-
     *                coordinate of the i-th point is assumed to be *(py + i).
     *  @param[in] pz A pointer to the z-coordinate of the 0-th point. The z-
     *                coordinate of the i-th point is assumed to be *(pz + i).
     */
    PointSetContiguous(size_type n_points, coord_pointer px, coord_pointer py,
                       coord_pointer pz) :
      m_n_points_(n_points), m_px_(px), m_py_(py), m_pz_(pz) {}

    /** @brief Makes a shallow copy of another PointSetContiguous
     *
     *  PointSetView objects alias PointSet objects. Copying a PointSetView
     *  makes a new PointSetView which still aliases the same object. This
     *  ctor powers the copy and thus needs to be a shallow copy.
     *
     *  @param[in] other The instance to copy.
     *
     *  @throw None No throw guarantee.
     */
    PointSetContiguous(const PointSetContiguous& other) = default;

    /// Defaulted no throw dtor
    ~PointSetContiguous() noexcept = default;

    /** @brief Compares for equality.
     *
     *  This method is intended to parallel PointSet::operator== which means
     *  it compares the aliased PointSet objects, not that the views alias the
     *  same PointSet object.
     */
    bool operator==(const PointSetContiguous& rhs) const noexcept;

protected:
    bool are_equal_(const base_type& other) const noexcept override {
        return base_type::template are_equal_impl_<my_type>(other);
    }

    pimpl_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    size_type size_() const noexcept override { return m_n_points_; }

    reference at_(size_type i) override {
        return reference(m_px_[i], m_py_[i], m_pz_[i]);
    }

    const_reference at_(size_type i) const override {
        return const_reference(m_px_[i], m_py_[i], m_pz_[i]);
    }

private:
    /// The number of points in *this
    size_type m_n_points_ = 0;

    /// The address of the first point's x-coordinate
    coord_pointer m_px_ = nullptr;

    /// The address of the first point's y-coordinate
    coord_pointer m_py_ = nullptr;

    /// The address of the second point's z-coordinate
    coord_pointer m_pz_ = nullptr;
};

// -----------------------------------------------------------------------------
// -- Out of line implementations
// -----------------------------------------------------------------------------

template<typename PointSetType>
bool PointSetContiguous<PointSetType>::operator==(
  const PointSetContiguous& rhs) const noexcept {
    // Must have the same number of points to be equal
    if(m_n_points_ != rhs.m_n_points_) return false;

    // N.b. From here on we know both have the same number of points

    // If there's zero points the addresses may be junk and we don't want to
    // compare them, so short circuit if empty
    if(m_n_points_ == 0) return true;

    // Try to short circuit if the addresses are the same
    const auto plhs = std::tie(m_px_, m_py_, m_pz_);
    const auto prhs = std::tie(rhs.m_px_, rhs.m_py_, rhs.m_pz_);
    if(plhs == prhs) return true;

    // Now we have to actually compare the values
    return std::equal(m_px_, m_px_ + m_n_points_, rhs.m_px_) &&
           std::equal(m_py_, m_py_ + m_n_points_, rhs.m_py_) &&
           std::equal(m_pz_, m_pz_ + m_n_points_, rhs.m_pz_);
}

} // namespace chemist::detail_
