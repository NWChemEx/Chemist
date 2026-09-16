/*
 * Copyright 2026 NWChemEx-Project
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
#include <chemist/experimental/point/point_view.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::experimental {

/** @brief Implements the API shared by PointSet and PointSetView.
 *
 *  This is the set-level counterpart of PointCommon, and exists for the same
 *  reason: an owning class and an aliasing class with identical APIs would
 *  otherwise declare, document, and implement that API twice.
 *
 *  Everything here is expressed in terms of "give me the coordinate array for
 *  Cartesian direction q". The derived class supplies only `buffer_(q)`; the
 *  container API (size, indexing, iteration) then follows, because the i-th
 *  point of the set is nothing more than the i-th element of each of the
 *  three arrays.
 *
 *  That last point is the whole trick. The set is stored as a structure of
 *  arrays, which is the layout integral libraries expect and the layout which
 *  vectorizes, but users want to ask for "the i-th point" and then ask that
 *  point for its x-coordinate. Indexing returns a PointView aliasing one
 *  element of each array, which behaves like a point without any of the
 *  coordinates ever being copied out of the set.
 *
 *  @tparam DerivedType The class deriving from *this. Must define `buffer_(q)`
 *                      and must declare *this a friend.
 *  @tparam PointSetType The, possibly const-qualified, PointSet type the
 *                       derived class models.
 */
template<typename DerivedType, typename PointSetType>
class PointSetCommon : public utilities::IndexableContainerBase<DerivedType> {
private:
    /// Type *this inherits from
    using base_type = utilities::IndexableContainerBase<DerivedType>;

    /// Lets the container base reach at_ and size_
    friend base_type;

    /// Struct defining the types for the set *this acts like
    using traits_type = ChemistClassTraits<PointSetType>;

    /// Struct defining the types for the points in the set
    using point_traits_type = typename traits_type::point_traits;

public:
    /// Type of a point in the set
    using value_type = typename traits_type::point_type;

    /// Type acting like a, possibly mutable, reference to a point in the set
    using reference = typename point_traits_type::view_type;

    /// Type acting like a read-only reference to a point in the set
    using const_reference = typename point_traits_type::const_view_type;

    /// Type used to own one Cartesian direction's worth of coordinates
    using buffer_type = typename traits_type::buffer_type;

    /// Type of a, possibly mutable, aliasing view of one coordinate array
    using buffer_reference = typename traits_type::buffer_reference;

    /// Type of a read-only, aliasing view of one coordinate array
    using const_buffer_reference = typename traits_type::const_buffer_reference;

    /// Type used for indexing and offsets
    using size_type = typename base_type::size_type;

    /// The number of Cartesian directions, and thus of coordinate arrays
    static constexpr size_type n_coords = 3;

    // -------------------------------------------------------------------------
    // -- Raw data access
    // -------------------------------------------------------------------------

    /** @brief Returns the coordinate array for Cartesian direction @p q.
     *
     *  This is the escape hatch for consumers which can not accept any C++
     *  abstraction and need a pointer to contiguous coordinates, e.g., an
     *  external integral library. The returned buffer is type-erased: it
     *  knows which floating-point type it holds, but that type is not part of
     *  its C++ type. A consumer which genuinely needs a pointer names the
     *  element type at that point, by asking the buffer for a span of a
     *  concrete type (`value<double>()`), or by visiting it with a callable
     *  which is instantiated for whichever type the buffer turned out to
     *  hold. The latter is preferable when the consumer can be written
     *  generically, since it can not guess wrong.
     *
     *  @warning A pointer obtained this way is only valid while the set's
     *           storage is unchanged, and only when the underlying buffer is
     *           actually contiguous --- which the buffer can be asked about
     *           with `is_contiguous()`.
     *
     *  @param[in] q The Cartesian direction whose array is wanted. Must be in
     *               the range [0, 3).
     *
     *  @return A view of the requested coordinate array.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3). Strong
     *                           throw guarantee.
     */
    ///@{
    buffer_reference get_buffer(size_type q) {
        assert_coord_in_range_(q);
        return downcast_().buffer_(q);
    }

    const_buffer_reference get_buffer(size_type q) const {
        assert_coord_in_range_(q);
        return downcast_().buffer_(q);
    }
    ///@}

    /** @brief Returns the array holding every point's x-coordinate.
     *
     *  Convenience function for calling `get_buffer(0)`.
     */
    ///@{
    buffer_reference get_x_buffer() { return get_buffer(0); }
    const_buffer_reference get_x_buffer() const { return get_buffer(0); }
    ///@}

    /** @brief Returns the array holding every point's y-coordinate.
     *
     *  Convenience function for calling `get_buffer(1)`.
     */
    ///@{
    buffer_reference get_y_buffer() { return get_buffer(1); }
    const_buffer_reference get_y_buffer() const { return get_buffer(1); }
    ///@}

    /** @brief Returns the array holding every point's z-coordinate.
     *
     *  Convenience function for calling `get_buffer(2)`.
     */
    ///@{
    buffer_reference get_z_buffer() { return get_buffer(2); }
    const_buffer_reference get_z_buffer() const { return get_buffer(2); }
    ///@}

    /** @brief Serializes *this into @p ar.
     *
     *  Writes the number of points, then each point in turn. Only save is
     *  implemented here; loading requires growing the coordinate arrays,
     *  which only a set which owns them can do.
     *
     *  @tparam Archive The type of the cereal output archive.
     *
     *  @param[in,out] ar The archive to write to.
     *
     *  @throw std::runtime_error if any coordinate is holding an
     *                            uncertainty-quantification type, which can
     *                            not be serialized. Weak throw guarantee.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        const auto n = this->size();
        ar(n);
        for(size_type i = 0; i < n; ++i) (*this)[i].save(ar);
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    PointSetCommon() noexcept                                 = default;
    PointSetCommon(const PointSetCommon&) noexcept            = default;
    PointSetCommon(PointSetCommon&&) noexcept                 = default;
    PointSetCommon& operator=(const PointSetCommon&) noexcept = default;
    PointSetCommon& operator=(PointSetCommon&&) noexcept      = default;
    ~PointSetCommon() noexcept                                = default;
    ///@}

    /// Throws std::out_of_range if @p q is not in the range [0, 3)
    static void assert_coord_in_range_(size_type q) {
        if(q < n_coords) return;
        throw std::out_of_range("chemist::experimental: Cartesian direction " +
                                std::to_string(q) +
                                " is not in the range [0, 3).");
    }

private:
    /// Wraps casting *this to the derived class
    DerivedType& downcast_() noexcept {
        return static_cast<DerivedType&>(*this);
    }

    /// Wraps casting *this to a read-only reference to the derived class
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }

    /** @brief Implements the container base's mutable element access.
     *
     *  The returned view aliases one element of each of the three coordinate
     *  arrays; nothing is copied out of the set, and writing through the
     *  result writes back into it.
     */
    reference at_(size_type i) {
        return reference(downcast_().buffer_(0).at(i),
                         downcast_().buffer_(1).at(i),
                         downcast_().buffer_(2).at(i));
    }

    /// Implements the container base's read-only element access
    const_reference at_(size_type i) const {
        return const_reference(downcast_().buffer_(0).at(i),
                               downcast_().buffer_(1).at(i),
                               downcast_().buffer_(2).at(i));
    }

    /// Implements the container base's size
    size_type size_() const noexcept { return downcast_().buffer_(0).size(); }
};

} // namespace chemist::experimental
