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
#include <chemist/basis_set2/atomic_basis_set/atomic_basis_set.hpp>
#include <chemist/detail_/view/traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::basis_set {
namespace detail_ {
template<typename AtomicBasisSetType>
class AtomicBasisSetViewPIMPL;
}

/** @brief Behaves like a reference to an AtomicBasisSet.
 *
 *  AtomicBasisSet instances own their state. AtomicBasisSetView instances alias
 *  their state, but otherwise have the same API as a AtomicBasisSet instance.
 *
 *  @tparam AtomicBasisSetType The type of AtomicBasisSet this is a view of.
 *                             @p AtomicBasisSetType is expected to be a
 *                             (possibly) cv-qualified instantiation of
 *                             AtomicBasisSet
 */
template<typename AtomicBasisSetType>
class AtomicBasisSetView : public utilities::IndexableContainerBase<
                             AtomicBasisSetView<AtomicBasisSetType>> {
private:
    /// Type of this class
    using my_type = AtomicBasisSetView<AtomicBasisSetType>;

    /// Type of the base class implementing the container API
    using container_base = utilities::IndexableContainerBase<my_type>;

    /// Type of the view traits which does the TMP for us
    using traits_type = chemist::detail_::ViewTraits<AtomicBasisSetType>;

    /// Is ShellType const-qualified?
    static constexpr bool is_const_v = traits_type::is_const_v;

    /// Typedefs so we don't need "typename" and "template" below
    template<typename T>
    using apply_const = typename traits_type::template apply_const<T>;

    template<typename T>
    using apply_const_ref = typename traits_type::template apply_const_ref<T>;

    template<typename T>
    using ptr_type = typename traits_type::template apply_const_ptr<T>;

    /// Is @p OtherType a mutable view?
    template<typename OtherType>
    static constexpr auto other_is_mutable_view =
      std::is_same_v<OtherType, AtomicBasisSetView<typename traits_type::type>>;

    /// Is @p OtherType a read-only view?
    template<typename OtherType>
    static constexpr auto other_is_const_view =
      std::is_same_v<OtherType,
                     AtomicBasisSetView<const typename traits_type::type>>;

    /// Disables a function if *this is not read-only and @p OtherType isn't
    /// a mutable view
    template<typename OtherType>
    using enable_if_this_const_other_mutable =
      std::enable_if_t<is_const_v && other_is_mutable_view<OtherType>>;

    /// Disables a function if *this is read-only and @p OtherType is
    /// a mutable view
    template<typename OtherType>
    using enable_if_this_mutable_other_const =
      std::enable_if_t<!is_const_v && other_is_const_view<OtherType>>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::AtomicBasisSetViewPIMPL<AtomicBasisSetType>;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type this is a view of
    using atomic_basis_set_type = typename traits_type::type;

    /// Reference to an instance of the viewed type
    using atomic_basis_set_reference = apply_const_ref<atomic_basis_set_type>;

    /// The type used to store the basis set name
    using name_type = typename atomic_basis_set_type::name_type;

    /// Potentially mutable reference to the basis set's name
    using name_reference = apply_const_ref<name_type>;

    /// Read-only reference to the basis set's name
    using const_name_reference =
      typename atomic_basis_set_type::const_name_reference;

    /// Unsigned integral type used to store the atomic number
    using atomic_number_type =
      typename atomic_basis_set_type::atomic_number_type;

    /// Potentially mutable reference to the basis set's atomic number
    using atomic_number_reference = apply_const_ref<atomic_number_type>;

    /// Read-only reference to the basis set's atomic number
    using const_atomic_number_reference =
      typename atomic_basis_set_type::const_atomic_number_reference;

    /// Unsigned integral type used for indexing/offsets
    using size_type = typename atomic_basis_set_type::size_type;

    /// Type used to return index ranges
    using range_type = typename atomic_basis_set_type::range_type;

    /// Type of the Shells on this AtomicBasisSet
    using value_type = typename atomic_basis_set_type::value_type;

    /// Type of a potentially mutable reference to a shell
    using reference = ShellView<apply_const<value_type>>;

    /// Type of a read-only reference to a shell
    using const_reference = ShellView<const value_type>;

    /// Traits from the ShellView class
    using shell_traits = ShellTraits<reference>;

    /// Type used for the center of this basis set
    using center_type = typename shell_traits::center_type;

    /// A possibly mutable reference to the center
    using center_reference = typename shell_traits::center_reference;

    /// A read-only reference to the center
    using const_center_reference =
      typename shell_traits::const_center_reference;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a view of a null AtomicBasisSet object.
     *
     *  A null AtomicBasisSet has no shells, is not centered anywhere, has no
     *  name, and is not associated with an atomic number.
     *
     *  @throw None No throw guarantee
     */
    AtomicBasisSetView() noexcept;

    /** @brief Creates a new view by deep copying another instance.
     *
     *  Deep copy in this context refers to the state in the view, not what
     *  the view is aliasing. In other words, the view resulting from this
     *  ctor will be a view of same AtomicBasisSet as @p rhs, but *this can
     *  be made to alias a different AtomicBasisSet without affecting @p rhs.
     *
     *  @param[in] rhs The instance to deep copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        instance. Strong throw guarantee.
     */
    AtomicBasisSetView(const AtomicBasisSetView& rhs);

    /** @brief Creates a new view instance by taking ownership of an
     *         already existing instance's state.
     *
     *  This ctor simply transfers the state in @p rhs to *this. The resulting
     *  view will still alias the same state and all references to that state
     *  will remain valid.
     *
     *  @param[in, out] rhs The instance to take the state from. After this ctor
     *                      @p rhs will no longer contain a PIMPL and can not
     *                      be used until a AtomicBasisSet instance with a valid
     *                      PIMPL is copy/move assigned to @p rhs.
     *
     *  @throw None no throw guarantee.
     */
    AtomicBasisSetView(AtomicBasisSetView&& rhs) noexcept;

    /** @brief Causes *this to contain a deep copy of @p rhs.
     *
     *  This function will assign to *this a deep copy of another instance.
     *  This instance's previous state will be released in the process. Deep
     *  copy in this sense is identical to the copy ctor, *i.e.*, the resulting
     *  copy will still alias the same AtomicBasisSet, but will do so with newly
     *  allocated state.
     *
     *  @param[in] rhs The instance to deep copy.
     *
     *  @return The current instance after setting it to a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        instance. Strong throw guarantee.
     */
    AtomicBasisSetView& operator=(const AtomicBasisSetView& rhs);

    /** @brief Causes *this to take ownership of an already existing instance's
     *         state.
     *
     *  @param[in, out] rhs The instance to take the state from. After this ctor
     *                      @p rhs will no longer contain a PIMPL and can not
     *                      be used until a AtomicBasisSet instance with a valid
     *                      PIMPL is copy/move assigned to @p rhs.
     *
     *  @return The current instance after taking ownership of @p rhs's state.
     *
     *  @throw None no throw guarantee.
     */
    AtomicBasisSetView& operator=(AtomicBasisSetView&& rhs) noexcept;

    /** @brief Creates a new view instance with the provided state.
     *
     *  @param[in] name The name associated with the basis set.
     *  @param[in] atomic_n The atomic number associated with the basis set.
     *  @param[in] r The point the basis set is centered on.
     *  @param[in] shells The shells of the basis set this views.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    AtomicBasisSetView(name_reference name, atomic_number_reference z,
                       center_reference r, std::vector<reference> shells);

    /** @brief Creates a new view instance that views the provided basis set.
     *
     *  @param[in] bs The basis set this will reference.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    AtomicBasisSetView(atomic_basis_set_reference bs);

    /** @brief Implicit conversion of a mutable view to a read-only view
     *
     *  @note This is NOT the copy ctor.
     *
     *  This ctor can be used to convert a AtomicBasisSetView of a mutable
     *  AtomicBasisSet to a view of a read-only AtomicBasisSet.
     *
     *  @tparam OtherType The type of the other view. @p OtherType is assumed to
     *                    be AtomicBasisSetView<AtomicBasisSet<T>>.
     *  @tparam <anonymous> Used to disable this function when *this is mutable
     *                      or when @p OtherType is read-only
     *
     *  @param[in] other The view we are implicitly converting to a read-only
     *                   view.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    template<typename OtherType,
             typename = enable_if_this_const_other_mutable<OtherType>>
    AtomicBasisSetView(const OtherType& other);

    /** @brief Sets the aliased AtomicBasisSet's state to a copy of @p rhs.
     *
     *  This operator will set the state of the AtomicBasisSet aliased by *this
     *  to a copy of @p rhs. In particular, this operator will NOT alias the
     *  state in @p rhs. To make *this alias the state in @p rhs, use the copy
     *  assignment operator.
     *
     *  @tparam ABSType2 The type of the AtomicBasisSet whose state is being
     *                   copied. Should always be non-cv-qualified
     *                   @p AtomicBasisSetType.
     *  @tparam <anonymous> Used to disable this function when the type of *this
     *                      is a view of read-only Shell.
     *
     *  @param[in] rhs The AtomicBasisSet whose state is being assigned to
     *                 *this.
     *
     *  @return *this after modifying the aliased state.
     *
     *  @throw None No throw guarantee.
     *
     */
    template<
      typename ABSType2 = std::decay_t<AtomicBasisSetType>,
      typename = std::enable_if<std::is_same_v<ABSType2, AtomicBasisSetType>>>
    AtomicBasisSetView& operator=(const ABSType2& rhs);

    /// Defaulted no throw dtor
    ~AtomicBasisSetView() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters and setters
    // -------------------------------------------------------------------------

    /** @brief Returns the name of the basis set.
     *
     *  Many atomic basis sets are part of a named set, where "name" refers to
     *  monikers such as 6-31G* or cc-pVDZ. This method will return a mutable
     *  reference to the name optional.
     *
     *  @warning We suggest treating the name of a basis set as metadata. In
     *           particular *this makes no checks to ensure that the parameters
     *           stored in it are actually the parameters associated with the
     *           name returned by this method.
     *
     * @return The name of this basis set.
     *
     * @throw std::runtime_error if *this is null. Strong throw guarantee.
     */
    name_reference basis_set_name();

    /** @brief Returns the name of the basis set.
     *
     *  Many atomic basis sets are part of a named set, where "name" refers to
     *  monikers such as 6-31G* or cc-pVDZ. This method will return a read-only
     *  reference to the name optional.
     *
     *  @warning We suggest treating the name of a basis set as metadata. In
     *           particular *this makes no checks to ensure that the parameters
     *           stored in it are actually the parameters associated with the
     *           name returned by this method.
     *
     * @return The name of this basis set.
     *
     * @throw std::runtime_error if *this is null. Strong throw guarantee.
     */
    const_name_reference basis_set_name() const;

    /** @brief Returns the atomic number associated with *this.
     *
     *  All of the shells in *this are centered at a single point in space.
     *  Typically there is also a nucleus at this point too. Often the atomic
     *  number of that nucleus dictates what shells are in *this. This method
     *  can be used to get and set the atomic number.
     *
     *  @warning We suggest treating the atomic number of the basis set as
     *           metadata. In particular, *this makes no effort to ensure that
     *           the basis set in *this is actually consistent with the atomic
     *           number.
     *
     *  @return A mutable reference to the atomic number.
     *
     *  @throw std::bad_alloc if *this is null. Strong throw guarantee.
     */
    atomic_number_reference atomic_number();

    /** @brief Returns the atomic number associated with *this.
     *
     *  All of the shells in *this are centered at a single point in space.
     *  Typically there is also a nucleus at this point too. Often the atomic
     *  number of that nucleus dictates what shells are in *this. This method
     *  can be used to get a read-only reference to the atomic number.
     *
     *  @warning We suggest treating the atomic number of the basis set as
     *           metadata. In particular, *this makes no effort to ensure that
     *           the basis set in *this is actually consistent with the atomic
     *           number.
     *
     *  @return A read-only reference to the atomic number.
     *
     *  @throw std::bad_alloc if *this is null. Strong throw guarantee.
     */
    const_atomic_number_reference atomic_number() const;

    /** @brief Returns the center associated with *this.
     *
     *  This method can be used to get and set the center.
     *
     *  @return A mutable reference to the center.
     *
     *  @throw std::bad_alloc if *this is null. Strong throw guarantee.
     */
    center_reference center();

    /** @brief Returns the center associated with *this.
     *
     *  This method can be used to get a read-only reference the center.
     *
     *  @return A read-only reference to the center.
     *
     *  @throw std::runtime_error if *this is null. Strong throw guarantee.
     */
    const_center_reference center() const;

    /** @brief Returns the total number of AOs on this center.
     *
     *  Each shell is comprised of AOs. This function will add up the total
     *  number of AOs in each shell.
     *
     *  @return The total number of AOs on this center.
     *
     *  @throw None no throw guarantee.
     */
    size_type n_aos() const noexcept;

    /** @brief Returns the @p i-th AO on the center.
     *
     *  AOs on the center are numbered by flattening out the shells. In other
     *  words, if the first shell has n AOs, the first n AOs are the n AOs of
     *  that shell. The n + 1-th AO is then the 0-th AO of the second shell and
     *  so on.
     *
     *  @param[in] i The index of the requested AO. Must be in the range [0,
     *                n_aos()).
     *
     *  @return A read-/write-able reference to the requested AO
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_aos()).
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL for the resulting view. Strong throw
     *                        guarantee.
     *
     */
    // ao_reference ao(size_type i);

    /** @brief Returns the @p i-th AO on the center.
     *
     *  AOs on the center are numbered by flattening out the shells. In other
     *  words, if the first shell has n AOs, the first n AOs are the n AOs of
     *  that shell. The n + 1-th AO is then the 0-th AO of the second shell and
     *  so on.
     *
     *  @param[in] i The index of the requested AO. Must be in the range [0,
     *                n_aos()).
     *
     *  @return A read-only reference to the requested AO.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_aos()).
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL for the resulting view. Strong throw
     *                        guarantee.
     *
     */
    // const_ao_reference ao(size_type i) const;

    /** @brief Computes the total number of primitives in this basis set.
     *
     *  Each shell is associated with one contracted Gaussian function, which
     *  itself is comprised of one or more primitives. This method sums up
     *  the total number of primitives in this basis set.
     *
     *  @return The total number of primitives in *this
     *
     *  @throw None No throw guarantee.
     */
    size_type n_primitives() const noexcept;

    /** @brief Returns the index range for the primitives in the shell with
     *         index @p shell.
     *
     *  @param[in] shell the index of the shell we want the primitive range for.
     *                   Must be in the range [0, size())
     *
     *  @return The range of primitives associated with the shell index.
     *
     *  @throw std::out_of_range if the @p shell is outside the range of shells
     *                           on this basis set. Strong throw guarantee.
     */
    range_type primitive_range(size_type shell) const;

    /** @brief Returns the shell index of primitive @p primitive belongs to.
     *
     *  @param[in] primitive the primitive whose shell we want the index of.
     *                       Must be in the range [0, n_primitives()).
     *
     *  @return The index of the shell primitive number @p primitive belongs
     *          to.
     *
     *  @throw std::out_of_range if the @p primitive is outside the range of
     *                           primitives on this basis set. Strong throw
     *                           guarantee.
     */
    size_type primitive_to_shell(size_type primitive) const;

    /** @brief Returns the @p i-th unique primitive on the center.
     *
     *  Primitives in *this are numbered by flattening out the contracted
     *  Gaussians in each shell.
     *
     *  @param[in] i The index of the requested primitive. Must be in the range
     *               [0, n_primitives()).
     *
     *  @return A read-/write-able reference to the requested primitive
     *
     *  @throw std::out_of_range if the @p primitive is outside the range of
     *                           primitives on this basis set. Strong throw
     *                           guarantee.
     *
     */
    typename shell_traits::primitive_reference primitive(size_type i);

    /** @brief Returns the @p i-th unique primitive on the center.
     *
     *  Primitives in *this are numbered by flattening out the contracted
     *  Gaussians in each shell.
     *
     *  @param[in] i The index of the requested primitive. Must be in the range
     *               [0, n_primitives()).
     *
     *  @return A read-only reference to the requested primitive
     *
     *  @throw std::out_of_range if the @p primitive is outside the range of
     *                           primitives on this basis set. Strong throw
     *                           guarantee.
     *
     */
    typename shell_traits::const_primitive_reference primitive(
      size_type i) const;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Exchanges the contents of *this with the state of @p other.
     *
     *  @param[in,out] other The object we are exchanging the state with. After
     *                       this call @p other will contain the stat which was
     *                       previously in *this and *this will contain the
     *                       state which was in @p other.
     *
     * @throw None No throw guarantee
     */
    void swap(AtomicBasisSetView& other) noexcept;

    /** @brief Is this a view of a null AtomicBasisSet?
     *
     *  AtomicBasisSet objects can be null, meaning *this can be a view of a
     *  null object.
     *  @return True if *this is a null basis set and false otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool is_null() const noexcept;

    /** @brief Compares two view instances for equality.
     *
     *  Two views are considered equal if they both alias AtomicBasisSet objects
     *  with the same number of shells and if the i-th shell of *this is value
     *  equal to the i-th shell of @p rhs for all i in the range `[0, size())`.
     *  This
     *  function will also compare the metadata (basis set name and atomic
     *  number.)
     *
     *  @param[in] rhs The object being compared to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const AtomicBasisSetView& rhs) const noexcept;

    /** @brief Compares *this to an AtomicBasisSet object for equality.
     *
     *  An AtomicBasisSetView and an AtomicBasisSet object are equal
     *  if the AtomicBasisSet aliased by the AtomicBasisSetView
     *  compares equal to the AtomicBasisSet object.
     *
     *  @param[in] rhs The object being compared to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const AtomicBasisSetType& rhs) const noexcept;

    /** @brief Determines if *this and @p rhs are different.
     *
     *  We define different as being not value equal. See operator== for the
     *  definition of value equal.
     *
     *  @param[in] rhs The object being compared to *this.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const AtomicBasisSetView& rhs) const noexcept {
        return !(*this == rhs);
    }

    /** @brief Determines if *this and @p rhs are different.
     *
     *  We define different as being not value equal. See operator== for the
     *  definition of value equal.
     *
     *  @param[in] rhs The object being compared to *this.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const AtomicBasisSetType& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Allows the IndexableContainerBase to access implementations
    friend container_base;

    /** @brief Creates a new view of an AtomicBasisSet with the provided state.
     *
     *  This ctor can be used to create a AtomicBasisSetView which uses the
     * provided PIMPL.
     *
     *  @param[in] pimpl The PIMPL to use for the AtomicBasisSetView part of the
     *                    resulting instance.
     *
     *  @throw None no throw guarantee.
     */
    AtomicBasisSetView(pimpl_pointer pimpl) noexcept;

    /// Throws std::runtime_error if this aliases a null basis
    void assert_non_null_() const;

    /// Throws std::out_of_range if primitive index is not valid
    void assert_primitive_index_(size_type i) const;

    /// Throws std::out_of_range if shell index is not valid
    void assert_shell_index_(size_type i) const;

    /// True if *this has a PIMPL and false otherwise
    bool has_pimpl_() const noexcept;

    /// Implements size()
    size_type size_() const noexcept;

    /// Implements non-const version of operator[]/at
    reference at_(size_type i);

    /// Implements const version of operator[]/at
    const_reference at_(size_type i) const;

    /// The instance that actually implements this class
    pimpl_pointer m_pimpl_;
};

// -- Inline implementations
template<typename AtomicBasisSetType>
template<typename OtherType, typename>
AtomicBasisSetView<AtomicBasisSetType>::AtomicBasisSetView(
  const OtherType& other) :
  AtomicBasisSetView(
    !other.is_null() ?
      AtomicBasisSetView(
        other.basis_set_name(), other.atomic_number(),
        std::move(other.center()),
        std::move(std::vector<reference>{other.begin(), other.end()})) :
      AtomicBasisSetView()) {}

template<typename AtomicBasisSetType>
template<typename ABSType2, typename>
AtomicBasisSetView<AtomicBasisSetType>&
AtomicBasisSetView<AtomicBasisSetType>::operator=(const ABSType2& rhs) {
    assert_non_null_();
    if(rhs.is_null())
        throw std::runtime_error(
          "Assigning a null AtomicBasisSet to a non-null AtomicBasisSet is "
          "currently not supported");
    if(rhs.size() != this->size())
        throw std::runtime_error(
          "AtomicBasisSet to assign must be the same size as the aliased one");

    basis_set_name() = rhs.basis_set_name();
    atomic_number()  = rhs.atomic_number();
    center().x()     = rhs.center().x();
    center().y()     = rhs.center().y();
    center().z()     = rhs.center().z();
    for(auto shell = 0; shell < this->size(); ++shell) {
        (*this)[shell] = rhs[shell];
    }
    return *this;
}

/// Ensures operator== is symmetric for when AtomicBasisSet is on the left
template<typename AtomicBasisSetType>
bool operator==(const AtomicBasisSetType& lhs,
                const AtomicBasisSetView<AtomicBasisSetType>& rhs) noexcept {
    return rhs == lhs;
}

template<typename AtomicBasisSetType>
bool operator==(
  const AtomicBasisSetType& lhs,
  const AtomicBasisSetView<const AtomicBasisSetType>& rhs) noexcept {
    return rhs == lhs;
}

/// Ensures operator!= is symmetric for when AtomicBasisSet is on the left
template<typename AtomicBasisSetType>
bool operator!=(const AtomicBasisSetType& lhs,
                const AtomicBasisSetView<AtomicBasisSetType>& rhs) noexcept {
    return rhs != lhs;
}

template<typename AtomicBasisSetType>
bool operator!=(
  const AtomicBasisSetType& lhs,
  const AtomicBasisSetView<const AtomicBasisSetType>& rhs) noexcept {
    return rhs != lhs;
}

extern template class AtomicBasisSetView<AtomicBasisSetD>;
extern template class AtomicBasisSetView<const AtomicBasisSetD>;
extern template class AtomicBasisSetView<AtomicBasisSetF>;
extern template class AtomicBasisSetView<const AtomicBasisSetF>;

} // namespace chemist::basis_set