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
#include <chemist/experimental/point/point_set_class.hpp>
#include <utility>

namespace chemist::experimental {

/** @brief An object which behaves like a PointSet, but aliases its storage.
 *
 *  *this is to PointSet what PointView is to Point: it has the same API, but
 *  it does not own the coordinates it hands out. Indexing *this yields a
 *  PointView aliasing the underlying storage, so writing through it writes
 *  into whatever *this aliases.
 *
 *  @note The current implementation aliases  three coordinate arrays. Other
 *        aliasing strategies are possible. If we need to alias different
 *        memory layouts, we can add a PIMPL to this class and implement.
 *
 *  @warning *this aliases storage it does not own. Anything which causes the
 *           aliased set to reallocate --- notably PointSet::push_back ---
 *           invalidates *this.
 *
 *  @tparam PointSetType A, possibly const-qualified, PointSet.
 *                       PointSetView<PointSet> aliases mutable storage;
 *                       PointSetView<const PointSet> aliases read-only
 *                       storage and hands out read-only PointView objects.
 */
template<typename PointSetType>
class PointSetView
  : public PointSetCommon<PointSetView<PointSetType>, PointSetType> {
private:
    /// Type *this inherits from
    using base_type = PointSetCommon<PointSetView<PointSetType>, PointSetType>;

    /// Lets the CRTP base reach buffer_
    friend base_type;

    /// Lets the container base reach the CRTP base's at_ and size_
    friend utilities::IndexableContainerBase<PointSetView<PointSetType>>;

    /// Lets the other const-qualification of *this reach m_x_ and friends
    template<typename OtherPointSetType>
    friend class PointSetView;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<std::is_const_v<PointSetType> && !std::is_const_v<T>>;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::buffer_reference;
    using typename base_type::buffer_type;
    using typename base_type::const_buffer_reference;
    using typename base_type::const_reference;
    using typename base_type::reference;
    using typename base_type::size_type;
    using typename base_type::value_type;
    ///@}

    /// Type of a non-const-qualified PointSet object
    using point_set_type =
      typename ChemistClassTraits<PointSetType>::value_type;

    /// Type of a, possibly read-only, reference to the aliased PointSet
    using point_set_reference =
      typename ChemistClassTraits<PointSetType>::reference;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view which aliases nothing.
     *
     *  The resulting view behaves like an empty set.
     *
     *  @throw None No throw guarantee.
     */
    PointSetView() noexcept = default;

    /** @brief Creates a view of @p ps.
     *
     *  This ctor is deliberately implicit so that a PointSet can be passed
     *  wherever a PointSetView is expected. The result aliases @p ps; it does
     *  not copy it.
     *
     *  @param[in] ps The set *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    PointSetView(point_set_reference ps) :
      PointSetView(ps.m_x_.as_view(), ps.m_y_.as_view(), ps.m_z_.as_view()) {}

    /** @brief Creates a view of three already-aliased coordinate arrays.
     *
     *  This ctor is what lets *this alias storage which is not owned by a
     *  PointSet at all.
     *
     *  @param[in] x A view of the array holding every x-coordinate.
     *  @param[in] y A view of the array holding every y-coordinate.
     *  @param[in] z A view of the array holding every z-coordinate.
     *
     *  @throw std::invalid_argument if @p x, @p y, and @p z are not all the
     *                               same length. Strong throw guarantee.
     */
    PointSetView(buffer_reference x, buffer_reference y, buffer_reference z) :
      m_x_(std::move(x)), m_y_(std::move(y)), m_z_(std::move(z)) {
        if(m_x_.size() == m_y_.size() && m_y_.size() == m_z_.size()) return;
        throw std::invalid_argument(
          "chemist::experimental::PointSetView: the x-, y-, and z-coordinate "
          "arrays must all be the same length.");
    }

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided.
     *
     *  @tparam OtherPointSetType The set type @p other models. This ctor only
     *                            participates in overload resolution when
     *                            @p other is mutable and *this is not.
     *
     *  @param[in] other The mutable view to convert.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename OtherPointSetType,
             typename = enable_mutable_to_const_t<OtherPointSetType>>
    PointSetView(const PointSetView<OtherPointSetType>& other) :
      m_x_(other.m_x_), m_y_(other.m_y_), m_z_(other.m_z_) {}

    /** @brief Creates a view aliasing the same storage as @p other.
     *
     *  The copy is shallow.
     *
     *  @param[in] other The view to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    PointSetView(const PointSetView& other) = default;

    /** @brief Takes the aliased state from @p other.
     *
     *  @param[in,out] other The view to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined, state.
     *
     *  @throw None No throw guarantee.
     */
    PointSetView(PointSetView&& other) noexcept = default;

    /** @brief Overwrites the points *this aliases with @p rhs's.
     *
     *  @note Like PointView's, this is a deep assignment: it writes through
     *        to the aliased storage rather than rebinding *this. Use swap to
     *        rebind. This method only exists when *this aliases mutable
     *        storage.
     *
     *  @param[in] rhs The view whose points should be copied into the storage
     *                 *this aliases.
     *
     *  @return *this, after overwriting the aliased points.
     *
     *  @throw std::runtime_error if @p rhs does not hold the same number of
     *                            points as *this, or if the concrete
     *                            floating-point types do not match. Weak
     *                            throw guarantee.
     */
    PointSetView& operator=(const PointSetView& rhs) {
        static_assert(!std::is_const_v<PointSetType>,
                      "Can not assign through a read-only PointSetView.");
        if(this != &rhs) assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the points *this aliases with @p rhs's.
     *
     *  This is the move assignment operator. It is identical to the copy
     *  assignment operator; there is nothing to steal, because *this does not
     *  own what it aliases.
     *
     *  @param[in] rhs The view whose points should be copied into the storage
     *                 *this aliases.
     *
     *  @return *this, after overwriting the aliased points.
     *
     *  @throw std::runtime_error under the same conditions as the copy
     *                            assignment operator. Weak throw guarantee.
     */
    PointSetView& operator=(PointSetView&& rhs) {
        static_assert(!std::is_const_v<PointSetType>,
                      "Can not assign through a read-only PointSetView.");
        if(this != &rhs) assign_(rhs);
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased storage.
    ~PointSetView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a PointSet holding a copy of the aliased points.
     *
     *  Use this when a copy really is wanted. The resulting set owns its
     *  coordinates and is completely decoupled from what *this aliases.
     *
     *  @return A PointSet value equal to *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    point_set_type as_point_set() const {
        point_set_type rv;
        const auto n = this->size();
        for(size_type i = 0; i < n; ++i) rv.push_back((*this)[i]);
        return rv;
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased points.
     *
     *  @param[in,out] other The view to exchange aliases with.
     *
     *  @throw None No throw guarantee.
     */
    void swap(PointSetView& other) noexcept {
        // wtf::buffer::BufferView has no swap of its own, but it is movable
        // and moving it rebinds, which is exactly what is wanted here.
        std::swap(m_x_, other.m_x_);
        std::swap(m_y_, other.m_y_);
        std::swap(m_z_, other.m_z_);
    }

private:
    /// Implements both assignment operators
    void assign_(const PointSetView& rhs) {
        const auto n = this->size();
        if(n != rhs.size())
            throw std::runtime_error(
              "chemist::experimental::PointSetView: can not assign a set of a "
              "different size through a view.");
        for(size_type i = 0; i < n; ++i) (*this)[i] = rhs[i];
    }

    /// Implements the CRTP base's coordinate array access
    buffer_reference buffer_(size_type q) const {
        if(q == 0) return m_x_;
        if(q == 1) return m_y_;
        return m_z_;
    }

    /// Aliases the array holding every point's x-coordinate
    buffer_reference m_x_;

    /// Aliases the array holding every point's y-coordinate
    buffer_reference m_y_;

    /// Aliases the array holding every point's z-coordinate
    buffer_reference m_z_;
};

/// Type of a view of a mutable PointSet
using point_set_view = PointSetView<PointSet>;

/// Type of a view of a read-only PointSet
using const_point_set_view = PointSetView<const PointSet>;

/** @note Only the mutable instantiation is declared here. Explicitly
 *        instantiating PointSetView<const PointSet> would force the compiler
 *        to instantiate its assignment operators, which deliberately do not
 *        compile for a read-only view. The read-only instantiation is covered
 *        by the unit tests instead.
 */
extern template class PointSetView<PointSet>;

} // namespace chemist::experimental
