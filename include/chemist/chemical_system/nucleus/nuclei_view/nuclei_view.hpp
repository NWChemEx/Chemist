/*
 * Copyright 2023 NWChemEx-Project
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
#include <chemist/chemical_system/nucleus/nuclei.hpp>
#include <chemist/chemical_system/point_charge/charges_view/charges_view.hpp>
#include <chemist/traits/nucleus_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename NucleiType>
class NucleiViewPIMPL;

} // namespace detail_

/** @brief Allows existing state to be used as if it were a Nuclei object.
 *
 *  Nuclei objects own their state. The NucleiView class aliases existing state,
 *  but allows the user to interact with the aliased state as if it were in a
 *  Nuclei object.
 *
 *  @tparam NucleiType
 */
template<typename NucleiType>
class NucleiView
  : public utilities::IndexableContainerBase<NucleiView<NucleiType>> {
private:
    /// Type of *this
    using my_type = NucleiView<NucleiType>;

    /// Type of the base
    using base_type = utilities::IndexableContainerBase<my_type>;

    template<typename T>
    static constexpr bool is_const_v = !std::is_same_v<std::remove_cv_t<T>, T>;

    /// Disables a function unless *this aliases a const Nuclei and T doesn't
    template<typename T>
    using enable_if_i_const_other_not =
      std::enable_if_t<is_const_v<NucleiType> && !is_const_v<T>>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::NucleiViewPIMPL<NucleiType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type used for indexing and offsets
    using size_type = typename base_type::size_type;

    /// Type for specifying members by offset
    using member_list_type = std::vector<size_type>;

    /// Type of a container filled with NucleiView objects
    using nuclei_view_container = std::vector<my_type>;

    // -- Nuclei/Nucleus types -------------------------------------------------

    /// Class containing the types for the aliased Nuclei object
    using nuclei_traits = ChemistClassTraits<NucleiType>;

    /// Type *this is behaving like
    using nuclei_type = typename nuclei_traits::value_type;

    /// Type of a reference to a nuclei object
    using nuclei_reference = typename nuclei_traits::reference;

    /// Class containing the types of the Nucleus objects in the aliased Nuclei
    using nucleus_traits = typename nuclei_traits::nucleus_traits;

    /// Type of an element in *this
    using value_type = typename nucleus_traits::value_type;

    /// Mutable reference to an element in *this
    using reference = typename nucleus_traits::view_type;

    /// Read-only reference to an element in *this
    using const_reference = typename nucleus_traits::const_view_type;

    /// Type of a container filled with references to Nucleus objects
    using reference_container = std::vector<reference>;

    /// Type of a pointer to a nucleus's name
    using name_pointer = typename nucleus_traits::name_pointer;

    /// Type of a pointer to an atomic number
    using atomic_number_pointer =
      typename nucleus_traits::atomic_number_pointer;

    /// Type of a pointer to a nucleus's mass
    using mass_pointer = typename nucleus_traits::mass_pointer;

    // -- PointCharge types----------------------------------------------------

    /// Class providing types for the Charges object *this aliases
    using charges_traits = typename nuclei_traits::charges_traits;

    /// Type of a mutable reference to a Charges object
    using charges_reference = typename charges_traits::view_type;

    /// Type used to store the charge
    using charge_type = typename value_type::charge_type;

    // -- Ctors, Assignment, and dtor ------------------------------------------

    /** @brief Creates a view to an empty Nuclei object.
     *
     *  @throw None No throw guarantee.
     */
    NucleiView() noexcept;

    /** @brief Creates a view which aliases @p nuclei
     *
     *  This ctor allows Nuclei objects to be implicitly converted to
     *  NucleiView objects.
     *
     *  @param[in] nuclei The Nuclei object to alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the internal
     *                        state of *this. Strong throw guarantee.
     */
    NucleiView(nuclei_reference nuclei);

    /** @brief Enables conversions from aliasing mutable Nuclei to aliasing
     *         read-only nuclei.
     *
     *  @tparam NucleiType2 The type aliased by @p other. In order for this
     *                      ctor to participate in overload resolution
     *                      @p NucleiType2 must be non-cv-qualified.
     *  @tparam <anonymous> Used to disable this ctor via SFINAE in the event
     *                      that *this aliases a mutable object and/or if
     *                      @p other aliases a read-only object.
     *
     *  @note This is NOT a copy constructor, but a conversion.
     *
     *  This ctor is used to implicitly convert a view of a mutable Nuclei
     *  object to a view of a read-only Nuclei object.
     *
     *  @param[in] other The view we want to convert
     *
     *  @throw std::bad_alloc if there is a problem allocating the state for the
     *                        new view. Strong throw guarantee.
     */
    template<typename NucleiType2,
             typename = enable_if_i_const_other_not<NucleiType2>>
    NucleiView(const NucleiView<NucleiType2>& other) :
      NucleiView(reference_container(other.begin(), other.end())) {}

    /** @brief Creates a NucleiView from contiguous data.
     *
     *  This ctor allows the user to use existing contiguous arrays of data as
     *  if it were a Nuclei object. The number of Nucleus objects will be taken
     *  from @p charges and each pointer `p` is assumed to be laid out so that
     *  the property for the `i`-th nucleus is `p[i]`.
     *
     *  @param[in] charges A ChargesView to use as the "Charges" part of *this
     *  @param[in] pnames A pointer to the first nucleus's name.
     *  @param[in] patomic_numbers A pointer to the first nucleus's atomic
     *                             number.
     *  @param[in] pmasses A pointer to the first nucleus's mass.
     */
    NucleiView(charges_reference charges, name_pointer pnames,
               atomic_number_pointer patomic_numbers, mass_pointer pmasses);

    /** @brief Holds a subset of the provided Nuclei object
     *
     *  @tparam BeginItr The type of the iterator pointing to the index of the
     *                   first nucleus.
     *  @tparam EndItr The type of the iterator pointing to just past the index
     *                 of the last nucleus.
     *
     *  This ctor fills in a container of type `member_list_type` and dispatches
     *  to the NucleiView(NucleiView, member_list_type) ctor. See that ctor
     *  for more information.
     *
     *  @param[in] supersystem The Nuclei object *this will be a subset of.
     *  @param[in] begin An iterator which points to the index of the first
     *                   nucleus.
     *  @param[in] end An iterator which points to just past the index of the
     *                 last nucleus.
     *
     * @throw std::bad_alloc if there is a problem allocating the memory. Strong
     *                       throw guarantee.
     *
     */
    template<typename BeginItr, typename EndItr>
    NucleiView(NucleiView supersystem, BeginItr&& begin, EndItr&& end) :
      NucleiView(std::move(supersystem),
                 member_list_type(std::forward<BeginItr>(begin),
                                  std::forward<EndItr>(end))) {}

    /** @brief Holds a subset of the provided Nuclei object
     *
     *  This ctor will create a view of @p supersystem which only includes a
     *  subset of the nuclei in @p supersystem. More specifically the resulting
     *  view will alias a Nuclei object which contains `members.size()` nuclei.
     *  The i-th nuclei in the aliased Nuclei object will be
     *  supersystem[members[i]]. For example, say @p supersystem has at least
     *  four nuclei. Then:
     *  ```
     *  NucleiView supersystem = get_a_nuclei_object_from_somewhere();
     *  member_list_type members{0, 2, 3};
     *  NucleiView subset(supersystem, members);
     *  ```
     * `subset` will be a view of a three element Nuclei object, such that
     *  `subset[0] == supersystem[0]`, `subset[1] = supersystem[2]`, and
     *  `subset[2] == supersystem[3]`.
     *
     *  @param[in] supersystem An alias of the supersystem. The NucleusView
     *                         objects in *this will be aliases of the Nucleus
     *                         objects in @p supersystem.
     *  @param[in] members The elements of the Nuclei object aliased by
     *                     @p supersystem which will be in the Nuclei object
     *                     aliased by *this.
     *
     *  @note If the data @p supersystem aliases is invalidated it will also
     *        invalidate *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    NucleiView(NucleiView supersystem, member_list_type members);

    /** @brief Creates a new NucleiView object containing @p members
     *
     *  Given a container of reference objects, @p members, this ctor will
     *  initialize *this to use the elements of @p members as aliases for the
     *  Nucleus objects in the Nuclei object aliased by *this.
     *
     *  @param[in] members References to the elements of *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    explicit NucleiView(reference_container members);

    /** @brief Initializes *this to the union of @p members.
     *
     *  This ctor is used to create a NucleiView which aliases a Nuclei object
     *  formed from the unions of the Nuclei objects aliased in @p members.
     *
     *  @param[in] members A container with aliases to the Nuclei objects to
     *                     take the union of.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    explicit NucleiView(nuclei_view_container members);

    /** @brief Makes *this a view of the same Nuclei as @p other.
     *
     *  This ctor will make a new NucleiView which is a deep copy of @p other.
     *  In the context of views, a deep copy entails aliasing the same Nuclei
     *  object, but doing so through new state. In other words, after this
     *  constructor is called the resulting object will behave like a reference
     *  to the same Nuclei object as @p other, but *this can be made to
     *  reference a different Nuclei object independent of @p other.
     *
     *  @param[in] other The view we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state for
     *         *this. Strong throw guarantee.
     *
     */
    NucleiView(const NucleiView& other);

    /** @brief Transfers ownership of @p other to *this.
     *
     *  This ctor will move the state in @p other into a newly created object.
     *
     *  @param[in,out] other The object whose state is being transferred to
     *                       make *this. After this operation @p other will be
     *                       a view of an empty Nuclei object.
     *
     * @throw None No throw guarantee.
     */
    NucleiView(NucleiView&& other) noexcept;

    /** @brief Overwrites the state in *this with a copy of @p rhs.
     *
     *  This function makes a deep copy of @p rhs via the copy ctor and then
     *  overwrites the state of *this with that deep copy. See the copy ctor's
     *  documentation for more details.
     *
     *  @param[in] rhs The view we are copying.
     *
     *  @return *this after overwriting its state with a copy of @p rhs.
     *
     *  @throw std::bad_alloc if the copy ctor throws. Strong throw guarantee.
     */
    NucleiView& operator=(const NucleiView& rhs);

    /** @brief Overwrites the state in *this with the state in @p rhs.
     *
     *  This function will release the state contained in *this and transfer it
     *  to *this.
     *
     *  @param[in,out] rhs The view we are taking the state from. After this
     *                     function @p rhs will be a view of an empty Nuclei
     *                     object.
     *
     *  @throw None No throw guarantee.
     */
    NucleiView& operator=(NucleiView&& rhs) noexcept;

    /// Default no throw dtor
    ~NucleiView() noexcept;

    /** @brief Exchanges the state of *this with @p other.
     *
     *  This function will "swap" the state of *this with the state in @p other.
     *  Thus, after this call *this will contain the state which was previously
     *  in @p other and @p other will contain the state which previously was in
     *  *this.
     *
     *  @param[in,out] other The view to swap state with. After this call
     *                       @p other will contain the state which was in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(NucleiView& other) noexcept;

    /** @brief Determines if the Nuclei aliased by *this and @p rhs are equal.
     *
     *  This method compares the Nuclei object aliased by *this to the Nuclei
     *  object aliased by @p other. The aliased Nuclei objects are compared by
     *  value. Notably, this means that *this and @p other do not need to alias
     *  the same Nuclei object for this method to be true.
     *
     *  @param[in] rhs The NucleiView to compare against.
     *
     *  @return True if *this and @p rhs alias Nuclei objects that are value
     *          equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const NucleiView& rhs) const noexcept;

    /** @brief Determines if *this is value equal to an existing Nuclei
     * object.
     *
     *   This method compares the Nuclei object *this is a view of to the
     *   provided Nuclei object. The Nuclei objects are compared for value
     *   equality as defined by Nuclei::operator==.
     *
     *   @param[in] rhs The Nuclei object to compare to.
     *
     *   @return True if *this compares value equal to @p rhs and false
     *           otherwise.
     *
     *   @throw None No throw guarantee.
     */
    bool operator==(const nuclei_type& rhs) const noexcept;

    /** @brief Determines if *this differs from an existing Nuclei object.
     *
     *  The NucleiView class defines different as not value equal. Value equal
     *  is defined by the corresponding operator==. This method simply negates
     *  the result of calling operator==.
     *
     *  @param[in] rhs The Nuclei object to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const nuclei_type& rhs) const noexcept;

private:
    /** @brief Creates a new view powered by the provided PIMPL.
     *
     *  @param[in] pimpl A pointer to the istance's state. If @p pimpl is null
     *             then the resulting view is the same as if it had been
     *             default initialized.
     *
     *  @throw None No throw guarantee.
     */
    explicit NucleiView(pimpl_pointer pimpl) noexcept;

    /// Allows the base class to access at_ and size_
    friend base_type;

    /// Implements retrieving read/write references
    reference at_(size_type i);

    /// Implements retrieving read-only references
    const_reference at_(size_type i) const;

    /// Used by base to determine the number of elements in *this
    size_type size_() const noexcept;

    /// Wraps determining if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// The object implementing *this
    pimpl_pointer m_pimpl_;
};

/** @brief Determines if the Nuclei aliased by @p lhs and @p rhs are different.
 *
 *  @relates NucleiView
 *
 *  @tparam NucleiType1 The cv-qualified type that @p lhs aliases.
 *  @tparam NucleiType2 The cv-qualified type that @p rhs aliases.
 *
 *  For the purposes of this function, "different" is defined as "not value
 *  equal" and this function simply negates the result of
 *  NucleiView::operator==(NucleiView). See the documentation for more detail.
 *
 *  @param[in] lhs The NucleiView on the left side of the operator.
 *  @param[in] rhs The NucleiView on the right side of the operator.
 *
 *  @return False if @p lhs and @p rhs alias Nuclei objects which are value
 *          equal and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename NucleiType1, typename NucleiType2>
bool operator!=(const NucleiView<NucleiType1>& lhs,
                const NucleiView<NucleiType2>& rhs) noexcept {
    return !(lhs == rhs);
}

/** @brief Compares a Nuclei object to a NucleiView object.
 *
 *  @relates NucleiView
 *
 *  This function simply calls NucleiView::operator==() on lhs. See the
 *  Nuclei overload of NucleiView::operator==() for more information.
 *
 *  @param[in] lhs The Nuclei object being compared to @p rhs.
 *  @param[in] rhs The NucleiView object being compared @p lhs.
 *
 *  @return True if @p lhs is equal to @p rhs and false otherwise.
 *
 *  @throw None No throw guarantee
 */
template<typename NucleiType>
inline bool operator==(const typename NucleiView<NucleiType>::nuclei_type& lhs,
                       const NucleiView<NucleiType>& rhs) noexcept {
    return rhs == lhs;
}

/** @brief Determines if a Nuclei object is different than the the Nuclei
 *         object @p rhs is a view of.
 *
 *  @relates NucleiView
 *
 *  This function is defined so that comparisons between a Nuclei and a
 *  NucleiView object can be done symmetrically. In practice this function
 *  simply calls NucleiView::operator!= on @p lhs. See the documentation of the
 *  Nuclei overload of NucleiView::operator!= for more details.
 *
 *  @param[in] lhs The Nuclei object being compared to @p rhs.
 *  @param[in] rhs The NucleiView object being compared to @p lhs.
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename NucleiType>
inline bool operator!=(const typename NucleiView<NucleiType>::nuclei_type& lhs,
                       const NucleiView<NucleiType>& rhs) noexcept {
    return rhs != lhs;
}

/** @brief Allows a NucleiView object to be printed akin to a Nuclei object.
 *
 *  @relates NucleiView
 *
 *  This method allows NucleiView objects to be printed.
 *
 *  @param[in,out] os The stream we are printing to. After the operation @p os
 *                    will contain a textual representation of @p nuclei.
 *  @param[in] nuclei The NucleiView to print.
 *  @return Returns @p os, after adding @p nuclei to it.
 *  @throw std::ios_base::failure if anything goes wrong while writing. Weak
 *                                throw guarantee.
 */
template<typename NucleiType>
std::ostream& operator<<(std::ostream& os,
                         const NucleiView<NucleiType>& nuclei) {
    for(const auto& nuke_i : nuclei) os << nuke_i << std::endl;
    return os;
}

extern template class NucleiView<Nuclei>;
extern template class NucleiView<const Nuclei>;

} // namespace chemist
