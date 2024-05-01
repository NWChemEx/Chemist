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
#include <chemist/point/point_set.hpp>
#include <chemist/traits/point_traits.hpp>
#include <memory>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename PointSetType>
class PointSetViewPIMPL;
}

/** @brief Acts like a reference to a PointSet object.
 *
 *  This class is used when you have existing state which is not actually a
 *  PointSet object, but you want to use it like it is a reference to the
 *  PointSet object.
 *
 *  @tparam PointSetType The cv-qualified type that *this acts lik an alias of.
 */
template<typename PointSetType>
class PointSetView
  : public utilities::IndexableContainerBase<PointSetView<PointSetType>> {
private:
    /// Type of *this
    using my_type = PointSetView<PointSetType>;

    /// Type this class derives from
    using base_type =
      utilities::IndexableContainerBase<PointSetView<PointSetType>>;

public:
    /// Traits associated with PointSetType
    using traits_type = ChemistClassTraits<PointSetType>;

    /// Traits associated with the Points in *this
    using point_traits_type = typename traits_type::point_traits;

    /// The non-cv qualified type *this is a view of
    using point_set_type = typename traits_type::value_type;

    /// reference to a point_set_type with cv-qualifiers matching PointSetType
    using point_set_reference = typename traits_type::reference;

    /// Type of a point stored in *this
    using value_type = typename point_traits_type::value_type;

    /// Type of a reference to a point in *this
    using reference = typename point_traits_type::view_type;

    /// Type of a read-only reference
    using const_reference = typename point_traits_type::const_view_type;

    /// Type of a PIMPL
    using pimpl_type = detail_::PointSetViewPIMPL<PointSetType>;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type used for indexing and offsets
    using typename base_type::size_type;

    // -------------------------------------------------------------------------
    // -- Ctors, Assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Makes a view of an empty PointSet object.
     *
     *  The object resulting from this ctor will act like it aliases an empty
     *  PointSet object.
     *
     *  @throw None No throw guarantee
     */
    PointSetView() noexcept;

    /** @brief Creates an object which aliases an existing PointSet.
     *
     *  This ctor is used to create a PointSetView from an existing PointSet.
     *  The resulting PointSetView will alias the state of the provided
     *  PointSet.
     *
     *  @param[in] ps The PointSet object to alias. Note this method takes a
     *                traditional reference to a PointSet object, NOT a
     *                PointSetView.
     *
     *  @throw std::bad_alloc if there is a problem allocating the internal
     *                        state. Strong throw guarantee.
     */
    explicit PointSetView(point_set_reference ps);

    /** @brief Base ctor for stateful ctors.
     *
     *  Other stateful ctors create an appropriate PIMPL object based on the
     *  state provided. Those ctors then dispatch to this ctor which is
     *  responsible for hooking up the PIMPL.
     *
     *  @param[in] pimpl The object which will actually power *this.
     *
     *  @throw None No throw guarantee.
     */
    explicit PointSetView(pimpl_pointer pimpl) noexcept;

    /** @brief Creates a new PointSetView which aliases the same PointSet as
     *         @p other.
     *
     *  This ctor is a shallow copy ctor in the sense that the new PointSetView
     *  object aliases the same PointSet as @p other. It is a deep copy ctor in
     *  the sense that the resulting PointSetView can be converted into an
     *  alias of another PointSet without affecting the PointSet @p other
     *  aliases.
     *
     *  @param[in] other The PointSetView to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    PointSetView(const PointSetView& other);

    /** @brief Causes *this to alias the same PointSet as @p other.
     *
     *  This operator will cause *this to alias the same PointSet as @p other.
     *  The internal state of *this after the operation will be shallow/deep
     *  copies in the same manner as the copy ctor.
     *
     *  @param[in] rhs The view to copy.
     *
     *  @return *this after making it alias the same PointSet as @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    my_type& operator=(const PointSetView& rhs);

    /** @brief Creates *this by taking ownership of @p other's state.
     *
     *  This ctor will transfer the internal state of @p other to *this. After
     *  the operation *this will alias the same PointSet that @p other did and
     *  @p other will act as if it was an alias of an empty PointSet. Note that
     *  after this operation *this does not own the state of the aliased
     *  PointSet, rather it owns the state used to alias the PointSet.
     *
     *  @param[in,out] other The view to take the state from. After calling this
     *                       method @p other will be in a state consistent with
     *                       aliasing an empty PointSet.
     *
     *  @throw None No throw guarantee.
     */
    PointSetView(PointSetView&& other) noexcept;

    /** @brief Transfers ownership of @p rhs's state to *this.
     *
     *  This operator works analogous to the move ctor except that it overwrites
     *  the state already in *this with that from @p rhs.
     *
     *  @param[in,out] rhs The PointSetView to take the state from. After
     *                     calling this method @p rhs will be in a state
     *                     consistent with aliasing an empty PointSet.
     *
     *  @return *this after taking ownership of @p rhs's state.
     *
     *  @throw None No throw guarantee.
     */
    my_type& operator=(PointSetView&& rhs) noexcept;

    /// No throw default dtor
    ~PointSetView() noexcept;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  This method swaps the implementation of @p other with that in *this.
     *  After the operation *this will alias the PointSet which @p other
     *  previously aliased and @p other will alias the PointSet *this
     *  previously aliased.
     *
     *  @param[in,out] other The object to exchange state with. After calling
     *                       this method @p other will have the state which
     *                       previously resided in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(PointSetView& other) noexcept;

    /** @brief Used to determine if the aliased PointSet is empty.
     *
     *  This method is used to determine if the aliased PointSet is empty. This
     *  is not the same as checking whether or not *this has a PIMPL (which
     *  users should not have to do).
     *
     *  @return True if *this is empty and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool empty() const noexcept;

    /** @brief Compares two views for equality.
     *
     *  This operator compares the aliased PointSets not whether *this and
     *  @p rhs alias the same PointSet. In other words, think of this method
     *  as comparing the objects two pointers point to, not the pointers
     *  themselves.
     *
     *  @param[in] rhs The view to compare to.
     *
     *  @return True if *this aliases a PointSet equal to the one aliased by
     *               @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const PointSetView& rhs) const noexcept;

    /** @brief Determines if two views are different.
     *
     *  This operator simply negates the result of calling operator==.
     *
     *  @param[in] rhs The view to compare to.
     *
     *  @return False if *this and @p other alias PointSet objects which compare
     *          equal and true otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool operator!=(const PointSetView& rhs) const noexcept;

private:
    /// Allow base class to access implementations
    friend base_type;

    /// Used internally to determine if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// Wraps cloning the PIMPL, accounting for when there isn't one
    pimpl_pointer clone_pimpl_() const;

    /// Implements at/operator[] for the base class
    reference at_(size_type i);

    /// Implements at/operator[] for the base class
    const_reference at_(size_type i) const;

    /// Implements size for the base class
    size_type size_() const noexcept;

    /// The actual object holding the state of *this
    pimpl_pointer m_pimpl_;
};

extern template class PointSetView<PointSet<float>>;
extern template class PointSetView<const PointSet<float>>;
extern template class PointSetView<PointSet<double>>;
extern template class PointSetView<const PointSet<double>>;

} // namespace chemist
