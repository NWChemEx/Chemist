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

#pragma once
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set.hpp>
#include <chemist/detail_/view/traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename AtomicBasisSetType>
class AtomicBasisSetViewPIMPL;
}

template<typename AtomicBasisSetType>
class AtomicBasisSetView : public utilities::IndexableContainerBase<
                             AtomicBasisSetView<AtomicBasisSetType>> {
private:
    /// Type of this class
    using my_type = AtomicBasisSetView<AtomicBasisSetType>;

    /// Type of the base class implementing the container API
    using container_base = utilities::IndexableContainerBase<my_type>;

    /// Type of the view traits which does the TMP for us
    using traits_type = ViewTraits<AtomicBasisSetType>;

    /// Typedefs so we don't need "typename" and "template" below
    template<typename T>
    using apply_const = typename traits_type::template apply_const<T>;

    template<typename T>
    using apply_const_ref = typename traits_type::template apply_const_ref<T>;

    template<typename T>
    using ptr_type = typename traits_type::template apply_const_ptr<T>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::AtomicBasisSetViewPIMPL<ShellType>;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type this is a view of
    using atomic_basis_set_type = traits_type::type;

    /// String-like type used to store the basis set name
    using name_type = typename atomic_basis_set_type::name_type;

    /// Mutable reference to the basis set's name
    using name_reference = apply_const_ref<name_type>;

    /// Read-only reference to the basis set's name
    using const_name_reference =
      typename atomic_basis_set_type::const_name_reference;

    /// Type of a pointer to the basis set's name
    using name_pointer = ptr_type<name_type>;

    /// Unsigned integral type used to store the atomic number
    using atomic_number_type =
      typename atomic_basis_set_type::atomic_number_type;

    /// Mutable reference to the basis set's atomic number
    using atomic_number_reference = apply_const_ref<name_type>;

    /// Read-only reference to the basis set's atomic number
    using const_atomic_number_reference =
      typename atomic_basis_set_type::const_atomic_number_reference;

    /// Type of a pointer to the atomic number
    using atomic_number_pointer = ptr_type<atomic_number_type>;

    /// Unsigned integral type used for indexing/offsets
    using size_type = typename atomic_basis_set::size_type;

    /// Type of a read-only reference to a size
    using const_size_reference = const size_type&;

    /// Type of a pointer to a read-only set of sizes
    using const_size_pointer = const size_type*;

    /// Type used to return index ranges
    using range_type = typename atomic_basis_set::range_type;

    // -------------------------------------------------------------------------
    // -- Shell types
    // -------------------------------------------------------------------------

    /// Type of the Shells on this AtomicBasisSet
    using value_type = typename atomic_basis_set::value_type;

    /// Type of a read-/write-able reference to a AtomicBasisSet
    using reference = ShellView<apply_const<value_type>>;

    /// Type of a read-only reference to a AtomicBasisSet
    using const_reference = ShellView<const value_type>;

    /// Type used to specify angular momentum
    using angular_momentum_type = typename reference::angular_momentum_type;

    /// Type of a possibly mutable reference to the angular momentum
    using angular_momentum_reference =
      typename reference::angumar_momentum_reference;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference =
      typename reference::const_angular_momentum_reference;

    /// Type of a pointer to a possibly mutable angular momentum
    using angular_momentum_pointer = ptr_type<angular_momentum>;

    /// Type used to specify whether a Shell is pure or not
    using pure_type = typename reference::pure_type;

    /// Type of a possibly mutable reference to the purity of the shell
    using pure_reference = typename reference::pure_reference;

    /// Type of a read-only reference to the purity of the shell
    using const_pure_reference = typename reference::const_pure_reference;

    /// Type of a pointer to a possibly mutable purity
    using pure_pointer = ptr_type<pure_type>;

    // -------------------------------------------------------------------------
    // -- AO types
    // -------------------------------------------------------------------------

    /// The type of the AOs comprising a shell
    // using ao_type = typename value_type::value_type;

    /// A read-/write-able reference to an AO
    // using ao_reference = typename value_type::reference;

    /// A read-only reference to an AO
    //  using const_ao_reference = typename value_type::const_reference;

    // -------------------------------------------------------------------------
    // -- Contracted Gaussian Function types
    // -------------------------------------------------------------------------

    /// Type of a reference to a contracted Gaussian function
    using contracted_gaussian_reference =
      typename reference::contracted_gaussian_reference;

    /// Type of a vector of coefficients
    using coefficient_vector = typename reference::coefficient_vector;

    /// Type of a vector of exponents
    using exponent_vector = typename reference::exponent_vector;

    // -------------------------------------------------------------------------
    // -- Primitive types
    // -------------------------------------------------------------------------

    /// Type of a mutable reference to a primitive
    using primitive_reference = typename reference::primitive_reference;

    /// Type of a read-only reference to a primitive
    using const_primitive_reference =
      typename reference::const_primitive_reference;

    /// Rank-1 tensor-like object holding the center
    using center_type = typename reference::center_type;

    /// Floating-point type used for specifying the center's coordinates
    using coord_type = typename reference::coord_type;

    /// Type of a reference to a center's coordinates
    using center_reference = typename reference::center_reference;

    /// Floating-point type used for expansion coefficients of the primitive
    using coefficient_type = typename reference::coefficient_type;

    /// Type of a reference to a coefficient
    using coefficient_reference = apply_const_ref<coefficient_type>;

    /// Type of a pointer to a coefficient
    using coefficient_pointer = ptr_type<coefficient_type>;

    /// Floating-point type used for primitive exponents
    using exponent_type = typename reference::exponent_type;

    /// Type of a reference to an exponent
    using exponent_reference = apply_const_ref<exponent_type>;

    /// Type of a pointer to an exponent
    using exponent_pointer = ptr_type<exponent_type>;

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

    /** @brief Creates a new view with the provided state.
     *
     *  This ctor is used to create a new view of an AtomicBasisSet instance
     *  by aliasing the provided Cartesian coordinates, basis set name, and
     *  atomic number.
     *
     *  @param[in] name The name associated with the basis set.
     *  @param[in] atomic_n The atomic number associated with the basis set.
     *  @param[in] nshells The number of shells in the basis set
     *  @param[in] pure_per_shell The purity of the 0-th shell. The purity of
     *                            the i-th shell is assumed to be obtainable
     *                            by `(&pure_per_shell)[i]`
     *  @param[in] l_per_shell A reference to the angular momentum of the 0-th
     *                         shell. The angular momentum of the i-th shell
     *                         is assumed to be obtainable by
     *                         `(&l_per_shell)`.
     *  @param[in] prims_per_shell A reference to the number of primitives in
     *                             the 0-th shell. The number of primitives in
     *                             shell i is assumed to be obtainable by
     *                             `(&prims_per_shell)[i]`.
     *  @param[in] cs The coefficient for the first primitive. The coefficients
     *                for the i-th primitive in the basis set are assumed to be
     *                obtainable by `(&cs)[i]`.
     *  @param[in] exp The exponent for the first primitive. The exponents
     *                for the i-th primitive in the basis set are assumed to be
     *                obtainable by `(&exp)[i]`.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    AtomicBasisSetView(name_reference name, atomic_number_reference atomic_n,
                       size_type nshells, pure_reference pure_per_shell,
                       angular_momentum_reference l_per_shell,
                       const_size_reference prims_per_shell,
                       coefficient_reference cs, exponent_reference exp,
                       center_reference center);

    /** @brief Creates a new view of an AtomicBasisSet.
     *
     *  This ctor is used to create a new view of an AtomicBasisSet by
     *  aliasing the state provided to the ctor.
     *
     *  @param[in] nshells The number of shells in the basis set
     *  @param[in] pure_per_shell The purity of the 0-th shell. The purity of
     *                            the i-th shell is assumed to be obtainable by
     *                            `(&pure_per_shell)[i]`
     *  @param[in] l_per_shell A reference to the angular momentum of the 0-th
     *                         shell. The angular momentum of the i-th shell
     *                         is assumed to be obtainable by
     *                         `(&l_per_shell)`.
     *  @param[in] prims_per_shell A reference to the number of primitives in
     *                             the 0-th shell. The number of primitives in
     *                             shell i is assumed to be obtainable by
     *                             `(&prims_per_shell)[i]`.
     *  @param[in] cs The coefficient for the first primitive. The coefficients
     *                for the i-th primitive in the basis set are assumed to be
     *                obtainable by `(&cs)[i]`.
     *  @param[in] exp The exponent for the first primitive. The exponents
     *                for the i-th primitive in the basis set are assumed to be
     *                obtainable by `(&exp)[i]`.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    AtomicBasisSetView(size_type nshells, pure_reference pure_per_shell,
                       angular_momentum_reference l_per_shell,
                       const_size_reference prims_per_shell,
                       coefficient_reference cs, exponent_reference exp,
                       center_reference center);

    /// Defaulted no throw dtor
    ~AtomicBasisSetView() noexcept override;

    // -------------------------------------------------------------------------
    // -- Getters and setters
    // -------------------------------------------------------------------------

    /** @brief Does the basis set have a name assigned to it?
     *
     *  Many atomic basis sets are part of a named set, where "name" refers to
     *  monikers such as 6-31G* or cc-pVDZ. This method will tell you whether
     *  or not the name of *this was set.
     *
     *  @warning We suggest treating the name of a basis set as metadata. In
     *           particular *this makes no effort to assign the name of the
     *           basis set, *i.e.*, just because an instance of AtomicBasisSet
     *           does not have a name, does not mean it is a custom basis set.
     *
     *  @return True if *this has a name and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_name() const noexcept;

    /** @brief Returns the name of the basis set.
     *
     *  Many atomic basis sets are part of a named set, where "name" refers to
     *  monikers such as 6-31G* or cc-pVDZ. If the name has been set, this
     *  method will return the name.
     *
     *  @warning We suggest treating the name of a basis set as metadata. In
     *           particular *this makes no checks to ensure that the parameters
     *           stored in it are actually the parameters associated with the
     *           name returned by this method.
     *
     * @return The name of this basis set.
     *
     * @throw std::runtime_error if *this does not have a basis set name.
     *                           Strong throw guarantee
     */
    name_reference basis_set_name();

    /** @brief Returns the name of the basis set.
     *
     *  Many atomic basis sets are part of a named set, where "name" refers to
     *  monikers such as 6-31G* or cc-pVDZ. If the name has been set, this
     *  method will return the name.
     *
     *  @warning We suggest treating the name of a basis set as metadata. In
     *           particular *this makes no checks to ensure that the parameters
     *           stored in it are actually the parameters associated with the
     *           name returned by this method.
     *
     * @return The name of this basis set.
     *
     * @throw std::runtime_error if *this does not have a basis set assigned to
     *                           it. Strong throw guarantee.
     */
    const_name_reference basis_set_name() const;

    /** @brief Does *this have an atomic number associated with it?
     *
     *  All of the shells in *this are centered at a single point in space.
     *  Typically there is also a nucleus at this point too. Often the atomic
     *  number of that nucleus dictates what shells are in *this. This method
     *  can be used to determine if the functions in *this are associated with
     *  a specific atomic number.
     *
     *  @warning We suggest treating the atomic number of the basis set as
     *           metadata. In particular *this makes no attempt to assign the
     *           atomic number associated with the data, *i.e.*, just because
     *           the atomic number is not set, does not mean that the shells
     *           in *this are not associated with an atomic number.
     *
     * @return True If the atomic number of *this has been set and false
     *              otherwise.
     *
     * @throw None No throw guarantee.
     */
    bool has_atomic_number() const noexcept;

    /** @brief Returns the atomic number associated with *this.
     *
     *  All of the shells in *this are centered at a single point in space.
     *  Typically there is also a nucleus at this point too. Often the atomic
     *  number of that nucleus dictates what shells are in *this. This method
     *  can be used to get and set the atomic number. If *this is a null object,
     *  this method will first allocate state and then return the atomic
     *  number in that state (which will be set to 0).
     *
     *  @warning We suggest treating the atomic number of the basis set as
     *           metadata. In particular, *this makes no effort to ensure that
     *           the basis set in *this is actually consistent with the atomic
     *           number.
     *
     *  @return A mutable reference to the atomic number.
     *
     *  @throw std::bad_alloc if *this is null and allocating the state fails.
     *                        Strong throw guarantee.
     */
    atomic_number_reference atomic_number();

    /** @brief Returns the atomic number associated with *this.
     *
     *  This function is similar to the non-const version, except an error is
     *  raised if *this is a null basis set (and that the resulting reference
     *  is read-only).
     *
     *  @warning We suggest treating the atomic number of the basis set as
     *           metadata. In particular, *this makes no effort to ensure that
     *           the basis set in *this is actually consistent with the atomic
     *           number.
     *
     *  @return A read-only reference to the atomic number.
     *
     *  @throw std::bad_alloc if *this is null and allocating the state fails.
     *                        Strong throw guarantee.
     */
    const_atomic_number_reference atomic_number() const;

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
     */
    size_type primitive_to_shell(size_type primitive) const;

    /** @brief Returns the @p i-th unique primitive on the center.
     *
     *  Primitives on the center are numbered by flattening out the shells and
     *  then flattening out the unique primitives comprising the AOs. Typically
     *  there is only one set of unique primitives per shell, nevertheless this
     *  function encapsulates that detail.
     *
     *  @param[in] i The index of the requested primitive. Must be in the range
     *               [0, n_unique_primitives()).
     *
     *  @return A read-/write-able reference to the requested primitive
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_aos()).
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL for the resulting view. Strong throw
     *                        guarantee.
     *
     */
    primitive_reference primitive(size_type i);

    /** @brief Returns the @p i-th unique primitive on the center.
     *
     *  Primitives on the center are numbered by flattening out the shells and
     *  then flattening out the unique primitives comprising the AOs. Typically
     *  there is only one set of unique primitives per shell, nevertheless this
     *  function encapsulates that detail.
     *
     *  @param[in] i The index of the requested primitive. Must be in the range
     *               [0, n_unique_primitives()).
     *
     *  @return A read-only reference to the requested primitive
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_aos()).
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL for the resulting view. Strong throw
     *                        guarantee.
     *
     */
    const_primitive_reference primitive(size_type i) const;

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

    /// Throws std::runtime_error if there's no aliased name
    void assert_name_() const;

    /// Throws std::runtime_error if there's no aliased atomic number
    void assert_atomic_number_() const;

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

extern template class AtomicBasisSetView<AtomicBasisSetD>;
extern template class AtomicBasisSetView<const AtomicBasisSetD>;
extern template class AtomicBasisSetView<AtomicBasisSetF>;
extern template class AtomicBasisSetView<const AtomicBasisSetF>;

} // namespace chemist
