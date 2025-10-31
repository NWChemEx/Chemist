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
#include <chemist/fragmenting/capping/capping.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/nucleus/nucleus.hpp>

namespace chemist::fragmenting {
namespace detail_ {
template<typename NucleiType>
class FragmentedNucleiPIMPL;
}

/** @brief Represents a set of fragments derived from a Nuclei object.
 *
 *  @tparam NucleiType The cv-qualified type that is being fragmented. Expected
 *                     to be either `Nuclei` or `const Nuclei`.
 *
 *  See https://tinyurl.com/mtba7n3w for design notes.
 *
 *  When we want to divide a ChemicalSystem into subsets we usually start by
 *  dividing the nuclei into subsets. The FragmentedNuclei class describes how
 *  the `Nuclei` part of the `ChemicalSystem` has been divided. It includes not
 *  only the subsets of the nuclei, but also any nuclei needed to account for
 *  broken bonds (i.e., caps).
 */
template<typename NucleiType>
class FragmentedNuclei : public FragmentedBase<FragmentedNuclei<NucleiType>> {
private:
    /// Type of *this
    using my_type = FragmentedNuclei<NucleiType>;

    /// Type *this derives from
    using base_type = FragmentedBase<my_type>;

public:
    /// Type of the object defining types associated with *this
    using typename base_type::traits_type;

    /// Type of the object implementing *this
    using pimpl_type = detail_::FragmentedNucleiPIMPL<NucleiType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of object *this is fragmenting
    using typename base_type::supersystem_type;

    /// Type of a mutable reference to the supersystem
    using typename base_type::supersystem_reference;

    /// Type of a read-only reference to the supersystem
    using typename base_type::const_supersystem_reference;

    /// Type of the fragments
    using value_type = typename traits_type::fragment_type;

    /// Type of a mutable reference to a fragment
    using reference = typename traits_type::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename traits_type::const_fragment_reference;

    /// Type of the object holding the caps for the fragments
    using cap_set_type = typename traits_type::cap_set_type;

    /// Type of a mutable reference to an object of type cap_set_type
    using cap_set_reference = typename traits_type::cap_set_reference;

    /// Type of a read-only reference to an object of type cap_set_type
    using const_cap_set_reference =
      typename traits_type::const_cap_set_reference;

    /// Type of a cap
    using cap_type = typename cap_set_type::value_type;

    /// Type supersystem uses for indexing and offsets
    using size_type = typename supersystem_type::size_type;

    /// Type used to specify a set of nucleus indices
    using nucleus_index_set = std::vector<size_type>;

    /// Type of a map from fragment index to nuclei offset
    using nucleus_map_type = std::vector<nucleus_index_set>;

    // -- CTors, assignment, and dtor

    /** @brief Fragments an empty set that fragments the empty set of nuclei.
     *
     *  This ctor will create an object with no fragments that is set up to
     *  fragment the empty set of nuclei.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei() noexcept;

    /** @brief Creates an empty object.
     *
     *  Empty FragmentedNuclei objects have a supersystem, but no fragments.
     *  Fragments can be added by calling emplace_back. This ctor is most
     *  useful when *this will be filled in algorithmically.
     *
     *  @param[in] supersystem The object *this will hold fragments of.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     */
    explicit FragmentedNuclei(supersystem_type supersystem);

    /** @brief Creates an empty object.
     *
     *  This ctor works the same as the other ctor for creating an empty object,
     *  except that it takes a view of the supersystem. See the description for
     *  the by-value overload for more details.
     *
     *  @param[in] supersystem The object *this will hold fragments of.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     */
    explicit FragmentedNuclei(const_supersystem_reference supersystem);

    /** @brief Initializes *this with the provided set of fragments.
     *
     *  @param[in] supersystem The object that is being fragmented.
     *  @param[in] frags A container-of-containers object such that
     *                   `frags[i][j]` is the offset (with respect to
     *                   @p supersystem) of the `j`-th nucleus in the `i`-th
     *                   fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    FragmentedNuclei(supersystem_type supersystem, nucleus_map_type frags);

    /** @brief Initializes *this with the provided set of fragments.
     *
     *  This overload dispatches to previous overload after copying
     *  @p supersystem. See the description for the other overload for more
     *  details.
     *
     *  @param[in] supersystem The object that is being fragmented.
     *  @param[in] frags A container-of-containers object such that
     *                   `frags[i][j]` is the offset (with respect to
     *                   @p supersystem) of the `j`-th nucleus in the `i`-th
     *                   fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    FragmentedNuclei(const_supersystem_reference supersystem,
                     nucleus_map_type frags);

    /** @brief Initializes *this from fragments and a set of caps.
     *
     *  @param[in] supersystem The object being fragmented.
     *  @param[in] frags A container-of-containers object such that
     *                   `frags[i][j]` is the offset (with respect to
     *                   @p supersystem) of the `j`-th nucleus in the `i`-th
     *                   fragment.
     *  @param[in] caps The caps for fixing severed bonds.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    FragmentedNuclei(supersystem_type supersystem, nucleus_map_type frags,
                     cap_set_type caps);

    /** @brief Initializes *this from fragments and a set of caps.
     *
     *  This overload dispatches to previous overload after copying
     *  @p supersystem. See the description for the other overload for more
     *  details.
     *
     *  @param[in] supersystem The object being fragmented.
     *  @param[in] frags A container-of-containers object such that
     *                   `frags[i][j]` is the offset (with respect to
     *                   @p supersystem) of the `j`-th nucleus in the `i`-th
     *                   fragment.
     *  @param[in] caps The caps for fixing severed bonds.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    FragmentedNuclei(const_supersystem_reference supersystem,
                     nucleus_map_type frags, cap_set_type caps);

    /** @brief Initializes *this to a copy of @p other.
     *
     *  This ctor will deep copy the supersystem of @p other, as well as the
     *  mapping of nuclei to fragments and the caps.
     *
     *  @param[in] other The object whose state is being copied.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    FragmentedNuclei(const FragmentedNuclei& other);

    /** @brief Takes ownership of the state in @p other.
     *
     *  This ctor will transfer the state in @p other to the newly constructed
     *  object.
     *
     *  @param[in,out] other The object whose state is being taken. After this
     *                       call @p other will be in a valid, but otherwise
     *                       unspecified state.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei(FragmentedNuclei&& other) noexcept;

    /** @brief Replaces the state in *this with a copy of that in @p rhs.
     *
     *  This method will release the state in *this and replace it with a
     *  copy of the state in @p rhs.
     *
     *  @param[in] rhs The object whose state is being taken.
     *
     *  @return *this with its state set to a deep copy of the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    FragmentedNuclei& operator=(const FragmentedNuclei& rhs);

    /** @brief Replaces the state in *this with that in @p rhs.
     *
     *  This method will replace the state which currently resides in *this with
     *  the state which is in @p rhs.
     *
     *  @param[in,out] rhs The object to take the state from. After this method
     *                     is called @p rhs will be in a valid, but otherwise
     *                     undefined state.
     *
     *  @return *this after taking the state of @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei& operator=(FragmentedNuclei&& rhs) noexcept;

    /// Defaulted no throw dtor
    ~FragmentedNuclei() noexcept;

    /** @brief Creates a fragment with the provided members.
     *
     *  @tparam BeginItr Type of the iterator pointing to the first member of
     *                   the fragment.
     *  @tparam EndItr Type of the iterator pointing to just past the last
     *                 member of the fragment.
     *
     *  This method will add a SINGLE fragment whose members are given by
     *  [begin, end). Of note this method can NOT be used to add multiple
     *  fragments. The iterators may point to either the indices of the nuclei
     *  to add or the actual Nucleus objects.
     *
     *  @param[in] begin The iterator pointing to the first member of the
     *                       fragment.
     *  @param[in] end The iterator pointint to just past the last member
     *                     of the fragment.
     *
     *  @throw std::runtime_error if *this does not have a supersystem of if any
     *                            of the members in the range are not members of
     *                            the supersystem. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new fragment.
     *                        Strong throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    void insert(BeginItr begin, EndItr end);

    /** @brief Creates a fragment with the specified elements.
     *
     *  @tparam ElementType Type of the members of @p il. Expected to be either
     *                      an integral type or convertible to `NucleusView`.
     *
     *  This method allows the user to manually specify the elements in a
     *  fragment.
     *
     *  @param[in] il An initializer list containing the elements for the
     *                fragment.
     *
     *  @throw std::runtime_error if *this does not have a supersystem or if
     *                            any member of @p il can not be mapped to a
     *                            nucleus in *this. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new fragment.
     *                        Strong throw guarantee.
     */
    template<typename ElementType>
    void insert(std::initializer_list<ElementType> il) {
        insert(il.begin(), il.end());
    }

    /** @brief Adds a fragment with the provided nuclei.
     *
     *  This method will create a new fragment which contains the elements in
     *  @p nuclei. To map the elements in @p nuclei to the supersystem in *this,
     *  this method relies on value equality. N.b., that such comparisons
     *  involve floating point comparisons and thus users are strongly suggested
     *  to use the elements of `supersystem()` directly.
     *
     *  @param[in] nuclei The objects to associate with the fragment.
     *
     *  @throw std::runtime_error if *this does not have a supersystem or if
     *                            any member of @p nuclei can not be mapped to a
     *                            nucleus in *this. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new fragment.
     *                        Strong throw guarantee.
     */
    void insert(const_reference nuclei);

    /** @brief Adds a fragment whose members are given by supersystem offset.
     *
     *  Assuming the supersystem contains `N` nuclei, users can specify a
     *  particular nucleus by giving an offset in the range [0, N). This method
     *  is used to specify a fragment by giving nuclear offsets.
     *
     *  @param[in] nuclei The offsets of the fragment's members.
     *
     *  @throw std::runtime_error if *this does not have a supersystem or if
     *                            any index in @p nuclei can not be mapped to a
     *                            nucleus in *this. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new fragment.
     *                        Strong throw guarantee.
     */
    void insert(nucleus_index_set nuclei);

    /** @brief Returns the indices for the nuclei in the @p i -th fragment.
     *
     *  The `operator[]` and `at` methods return fragments as objects. Sometimes
     *  we want to just know the offsets of the elements in the fragments. This
     *  method can be used to retrieve the set of offsets for the @p i -th
     *  fragment.
     *
     *  @param[in] i The offset of the fragment whose indices we want. @p i
     *               must be in the range [0, size()).
     *
     *  @throw std::bad_alloc if there is a problem allocating the result.
     *                        Strong throw guarantee.
     *  @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *                           throw guarantee.
     */
    nucleus_index_set nuclear_indices(size_type i) const;

    /** @brief Provides access to the caps in *this.
     *
     *  When fragmenting large, covalently bonded systems we often must sever
     *  bonds. To rectify the resulting electronic structure of the fragments,
     *  one typically adds caps. This method is used to retrieve the internal
     *  `CapSet` object managing the caps for *this.
     *
     *  @return A mutable reference to the internal `CapSet` object.
     *
     *  @throw std::runtime_error if *this does not have a supersystem. Strong
     *                        throw guarantee.
     *  @throw std::bad_alloc if *this has a supersystem, but no PIMPL, and
     *                        allocating the PIMPL fails. Strong throw
     *                        guarantee.
     */
    cap_set_reference cap_set();

    /** @brief The caps *this is currently managing.
     *
     *  Same as the non-const version, except that:
     *  1. if *this does not have a PIMPL, one will not be created
     *  2. the resulting object is read-only.
     *
     *  @return a read-only reference to the `CapSet` object *this is using to
     *          hold the caps.
     *
     *  @throw std::runtime_error if *this does not have a supersystem or a
     *                            PIMPL. Strong throw guarantee.
     */
    const_cap_set_reference cap_set() const;

    /** @brief Adds a cap to *this.
     *
     *  This method is a convenience method for calling `cap_set` and inserting
     *  a cap into the resulting CapSet object.
     *
     *  @param[in] cap The cap to add to *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     *  @throw std::runtime_error if *this does not have a supersystem. Strong
     *                        throw guarantee.
     */
    void add_cap(cap_type cap) { cap_set().push_back(std::move(cap)); }

    /** @brief Exchanges the contents of *this with @p other.
     *
     *  @param[in,out] other The object to swap state with. After this call
     *                       @p other will contain the state which was
     *                       previously in *this
     *  @throw None No throw guarantee.
     */
    void swap(FragmentedNuclei& other) noexcept;

    /** @brief Is *this value equal to @p rhs?
     *
     *  This method defines value equal as both being null or both being
     *  non-null. If both are non-null then they must also satisfy:
     *  - Supersystems compare value equal
     *  - Same number of fragments
     *  - Same caps
     *  - (*this)[i] == rhs[i] for all i in range [0, size())
     *
     *  @param[in] rhs Teh object to compare against.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FragmentedNuclei& rhs) const noexcept;

    /** @brief Is *this different than @p rhs?
     *
     *  This method defines "different" as not value equal. See operator==
     *  for the definition of value equal.
     *
     *  @param[in] rhs The object to compare against.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const FragmentedNuclei& rhs) const noexcept;

protected:
    friend utilities::IndexableContainerBase<my_type>;

    /// Implements at() and operator[]
    reference at_(size_type i);

    /// Implements at() const and operator[] const
    const_reference at_(size_type i) const;

    /// Implements size_
    size_type size_() const noexcept;

    /// Implements supersystem()
    virtual supersystem_reference supersystem_() override;

    /// Implements supersystem() const
    virtual const_supersystem_reference supersystem_() const override;

    /// Implements concatenate()
    const_reference concatenate_(
      std::vector<size_type> fragment_indices) const override;

private:
    /// Factors out the code for checking if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /// Factors out the code for asserting that *this has a PIMPL
    void assert_pimpl_() const;

    /// The object actually implementing *this
    pimpl_pointer m_pimpl_;
};

// -- Out of class inline implementations --------------------------------------

template<typename NucleiType>
template<typename BeginItr, typename EndItr>
void FragmentedNuclei<NucleiType>::insert(BeginItr begin, EndItr end) {
    // If we put the arguments directly into a vector this is what would result
    using vec_t = decltype(std::vector(begin, end));

    // These would be the elements
    using input_type = typename vec_t::value_type;

    // Were we given indices or nuclei?
    if constexpr(std::is_integral_v<input_type>) {
        nucleus_index_set inputs(begin, end);
        insert(std::move(inputs));
    } else {
        // Assumed to be Nucleus or NucleusView objects
        using input_t = typename const_reference::reference_container;
        input_t inputs(begin, end);
        insert(const_reference(inputs));
    }
}

// -- Forward declare explicit template instantiations -------------------------

extern template class FragmentedNuclei<Nuclei>;
extern template class FragmentedNuclei<const Nuclei>;

} // namespace chemist::fragmenting
