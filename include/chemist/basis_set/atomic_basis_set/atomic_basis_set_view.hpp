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

namespace chemist {
namespace detail_ {
template<typename ShellType>
class AtomicBasisSetViewPIMPL;
}

template<typename ShellType>
class AtomicBasisSetView {
private:
    /// Type of this class
    using my_type = AtomicBasisSetView<ShellType>;

    /// Type of the view traits which does the TMP for us
    using traits_type = ViewTraits<ShellType>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::AtomicBasisSetViewPIMPL<ShellType>;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type this is a view of
    using atomic_basis_set_type = AtomicBasisSet<traits_type::type>;

    /// String-like type used to store the basis set name
    using name_type = std::string;

    /// Mutable reference to the basis set's name
    using name_reference = name_type&;

    /// Read-only reference to the basis set's name
    using const_name_reference = const name_type&;

    /// Unsigned integral type used to store the atomic number
    using atomic_number_type = std::size_t;

    /// Mutable reference to the basis set's atomic number
    using atomic_number_reference = atomic_number_type&;

    /// Read-only reference to the basis set's atomic number
    using const_atomic_number_reference = const atomic_number_type&;

    /// Unsigned integral type used for indexing/offsets
    using size_type = typename container_base::size_type;

    /// Type used to return index ranges
    using range_type = std::pair<size_type, size_type>;

    // -------------------------------------------------------------------------
    // -- Shell types
    // -------------------------------------------------------------------------

    /// Type of the Shells on this AtomicBasisSet
    using value_type = ShellType;

    /// Type of a read-/write-able reference to a AtomicBasisSet
    using reference = ShellView<value_type>;

    /// Type of a read-only reference to a AtomicBasisSet
    using const_reference = ShellView<const value_type>;

    /// Type used to specify angular momentum
    using angular_momentum = typename value_type::angular_momentum;

    /// Type used to specify whether a Shell is pure or not
    using pure_type = typename value_type::pure_type;

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
      typename value_type::contracted_gaussian_reference;

    /// Type of a vector of coefficients
    using coefficient_vector =
      typename contracted_gaussian_reference::coefficient_vector;

    /// Type of a vector of exponents
    using exponent_vector =
      typename contracted_gaussian_reference::exponent_vector;

    // -------------------------------------------------------------------------
    // -- Primitive types
    // -------------------------------------------------------------------------

    /// Type of a mutable reference to a primitive
    using primitive_reference = typename value_type::primitive_reference;

    /// Type of a read-only reference to a primitive
    using const_primitive_reference =
      typename value_type::const_primitive_reference;

    /// Rank-1 tensor-like object holding the center
    using center_type = typename primitive_reference::center_type;

    /// Floating-point type used for specifying the center's coordinates
    using coord_type = typename center_type::coord_type;

    /// Floating-point type used for expansion coefficients of the primitive
    using coefficient_type = typename primitive_reference::coefficient_type;

    /// Floating-point type used for primitive exponents
    using exponent_type = typename primitive_reference::exponent_type;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a null AtomicBasisSet object.
     *
     *  A null AtomicBasisSet has no shells, is not centered anywhere, has no
     *  name, and is not associated with an atomic number.
     *
     *  @throw None No throw guarantee
     */
    AtomicBasisSet() noexcept;

    /** @brief Creates a new AtomicBasisSet by deep copying another instance.
     *
     *  @param[in] rhs The AtomicBasisSet instance to deep copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        instance. Strong throw guarantee.
     */
    AtomicBasisSet(const AtomicBasisSet& rhs);

    /** @brief Creates a new AtomicBasisSet instance by taking ownership of an
     *         already existing instance's state.
     *
     *  @param[in, out] rhs The instance to take the state from. After this ctor
     *                      @p rhs will no longer contain a PIMPL and can not
     *                      be used until a AtomicBasisSet instance with a valid
     *                      PIMPL is copy/move assigned to @p rhs.
     *
     *  @throw None no throw guarantee.
     */
    AtomicBasisSet(AtomicBasisSet&& rhs) noexcept;

    /** @brief Causes the current AtomicBasisSet to contain a deep copy of
     *         another AtomicBasisSet.
     *
     *  This function will assign to the current AtomicBasisSet a deep copy of
     *  another instance. This instance's previous state will be released.
     *
     *  @param[in] rhs The AtomicBasisSet instance to deep copy.
     *
     *  @return The current instance after setting it to a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        instance. Strong throw guarantee.
     */
    AtomicBasisSet& operator=(const AtomicBasisSet& rhs);

    /** @brief Causes the current AtomicBasisSet instance to take ownership of
     *         an already existing instance's state.
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
    AtomicBasisSet& operator=(AtomicBasisSet&& rhs) noexcept;

    /** @brief Creates a new AtomicBasisSet centered on the provided point.
     *
     *  This ctor is used to create a new AtomicBasisSet instance with the
     *  provided Cartesian coordinates, basis set name, and atomic number.
     *
     *  @param[in] name The name associated with the basis set.
     *  @param[in] atomic_n The atomic number associated with the basis set.
     *  @param[in] x The x-coordinate for the resulting AtomicBasisSet
     *  @param[in] y The y-coordinate for the resulting AtomicBasisSet
     *  @param[in] z The z-coordinate for the resulting AtomicBasisSet
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    AtomicBasisSet(const_name_reference name, atomic_number_type atomic_n,
                   coord_type x, coord_type y, coord_type z);

    /** @brief Creates a new AtomicBasisSet centered on the provided point.
     *
     *  This ctor is used to create a new AtomicBasisSet instance with the
     *  provided Cartesian coordinates, while the basis set name and atomic
     *  number are defaulted. The name can be set later by calling `name()`.
     *  Similarly the atomic number can be set later by calling
     *
     *  @param[in] x The x-coordinate for the resulting AtomicBasisSet
     *  @param[in] y The y-coordinate for the resulting AtomicBasisSet
     *  @param[in] z The z-coordinate for the resulting AtomicBasisSet
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    AtomicBasisSet(coord_type x, coord_type y, coord_types z);

    /** @brief Creates a new AtomicBasisSet with the provided name and atomic
     *         number.
     *
     *  This ctor is used to create a new AtomicBasisSet instance, centered
     *  on the origin, with the provided basis set name and atomic number.
     *
     *  @param[in] name The name associated with the basis set.
     *  @param[in] atomic_n The atomic number associated with the basis set.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    AtomicBasisSet(const_name_reference name, atomic_number_type atomic_n);

    /// Defaulted no throw dtor
    ~AtomicBasisSet() noexcept override;

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
     *  method will return the name. If *this is a null atomic basis set, this
     *  method will allocate a new state and then return a reference to the
     *  name attribute of that state (which will be an empty string).
     *
     *  @warning We suggest treating the name of a basis set as metadata. In
     *           particular *this makes no checks to ensure that the parameters
     *           stored in it are actually the parameters associated with the
     *           name returned by this method.
     *
     * @return The name of this basis set.
     *
     * @throw std::bad_alloc if *this is a null AtomicBasisSet and allocating
     *                       the state fails. Strong throw guarantee.
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

    /** @brief Adds a shell to the center.
     *
     *  This function will create a Shell instance, with the specified
     *  parameters, on the current AtomicBasisSet.
     *
     *  @param[in] pure Whether the new shell is pure or not.
     *  @param[in] l The total angular momentum of the new shell.
     *  @param[in] cs The contraction coefficients for the primitives comprising
     *                the new shell's CGTO.
     *  @param[in] es The exponents for the primitives comprising the new
     *                shell's CGTO.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        Shell. Weak throw guarantee.
     */
    void add_shell(pure_type pure, am_type l, coefficient_vector cs,
                   exponent_vector es);

    /** @brief Returns the total number of AOs on this center.
     *
     *  Each shell is comprised of AOs. This function will add up the total
     *  number of AOs in each shell.
     *
     *  @return The total number of AOs on this center.
     *
     *  @throw None no throw gurantee.
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
    void swap(AtomicBasisSet& other) noexcept;

    /** @brief Is this AtomicBasisSet null?
     *
     *  AtomicBasisSet objects can be null if they are default constructed or
     *  if they are moved from.
     *
     *  @return True if *this is a null basis set and false otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool is_null() const noexcept;

    /** @brief Compares two AtomicBasisSet instances for equality.
     *
     *  Two AtomicBasisSet instances are considered equal if they contain the
     *  same number of shells and if the i-th shell of *this is value equal to
     *  the i-th shell of @p rhs for all i in the range `[0, size())`. This
     *  function will also compare the metadata (basis set name and atomic
     *  number.)
     *
     *  @param[in] rhs The AtomicBasisSet being compared to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const AtomicBasisSet& rhs) const noexcept;

    /** @brief Determines if two AtomicBasisSet objects are different.
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
    bool operator!=(const AtomicBasisSet& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Allows the IndexableContainerBase to access implementations
    friend container_base;

    /** @brief Creates a new AtomicBasisSet with the provided state.
     *
     *  This ctor can be used to create a AtomicBasisSet which uses the provided
     *  PIMPL.
     *
     *  @param[in] pimpl The PIMPL to use for the AtomicBasisSet part of the
     *                    resulting instance.
     *
     *  @throw None no throw guarantee.
     */
    AtomicBasisSet(pimpl_pointer pimpl) noexcept;

    /// True if *this has a PIMPL and false otherwise
    bool has_pimpl_() const noexcept;

    /// Implements size()
    size_type size_() const noexcept;

    /// Implements non-const version of operator[]/at
    reference at_(size_type i);

    /// Implements const version of operator[]/at
    const_reference at_(size_type i) const;

    /// The instance that actually implements this class
    center_pimpl_ptr_t m_pimpl_;
};

extern template class AtomicBasisSet<double>;
extern template class AtomicBasisSet<float>;

} // namespace chemist
