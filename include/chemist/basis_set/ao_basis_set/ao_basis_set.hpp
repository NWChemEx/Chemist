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
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set_traits.hpp>
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set_view.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::basis_set {
namespace detail_ {
template<typename AtomicBasisSetType>
class AOBasisSetPIMPL;
} // namespace detail_

/** @brief Models a set of atomic basis sets.
 *
 *  An AO basis set is a collection one-electron orbitals, represented here as a
 *  container of atomic basis sets.
 *
 *  @tparam AtomicBasisSetType The type of an atomic basis set.
 */
template<typename AtomicBasisSetType>
class AOBasisSet
  : public utilities::IndexableContainerBase<AOBasisSet<AtomicBasisSetType>> {
private:
    /// Type of this instance
    using my_type = AOBasisSet<AtomicBasisSetType>;

    /// Type of the base class implementing Container API
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    /// Type of the PIMPL implementing this class
    using pimpl_type = detail_::AOBasisSetPIMPL<AtomicBasisSetType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Unsigned integral type used for indexing/offsets
    using size_type = typename base_type::size_type;

    /// Type of the centers in this basis set
    using value_type = AtomicBasisSetType;

    /// Type of a read-/write-able reference to a AtomicBasisSet
    using reference = AtomicBasisSetView<value_type>;

    /// Type of a read-only reference to a AtomicBasisSet
    using const_reference = AtomicBasisSetView<const value_type>;

    /// Traits class holding all of the types related to the AtomicBasisSet
    using abs_traits = AtomicBasisSetTraits<reference>;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a new AOBasisSet instance containing no centers.
     *
     *  This ctor can be used to create a new AOBasisSet instance which contains
     *  no centers. Centers can be added by calling `add_center`.
     *
     *  @throw None No throw guarantee
     *
     *  Complexity: Constant
     */
    AOBasisSet() noexcept;

    /** @brief Creates a new AOBasisSet by deep copying another instance.
     *
     *  This ctor can be used to create a new AOBasisSet instance which contains
     *  a deep copy of @p rhs.
     *
     *  @param[in] rhs The AOBasisSet instance to deep copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to deep copy
     *                        @p rhs. Strong throw guarantee.
     *
     *  Complexity: Linear in the size of @p rhs.
     */
    AOBasisSet(const AOBasisSet& rhs);

    /** @brief Creates a new AOBasisSet instance which takes ownership of
     *         another instance's state.
     *
     *  This ctor will create a new AOBasisSet instance by taking ownership of
     *  @p rhs's state.
     *
     *  @param[in,out] rhs The instance whose state will be transferred to the
     *                     resulting instance. After the operation @p rhs will
     *                     not contain a PIMPL and will thus not be usable until
     *                     another PIMPL-containing AOBasisSet instance is
     *                     assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    AOBasisSet(AOBasisSet&& rhs) noexcept;

    /** @brief Makes the current instance contain a deep copy of another
     *         AOBasisSet.
     *
     *  This function can be used to make the current AOBasisSet instance a deep
     *  copy of @p rhs. The state this instance had prior to the call will be
     *  released.
     *
     *  @param[in] rhs The AOBasisSet instance to deep copy.
     *
     *  @return The current instance containing a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to deep copy
     *                        @p rhs. Strong throw guarantee.
     *
     *  Complexity: Linear in the size of @p rhs.
     */
    AOBasisSet& operator=(const AOBasisSet& rhs);

    /** @brief Makes the current instance take ownership of another AOBasisSet
     *         instance's state.
     *
     *  This function will transfer ownerhsip of @p rhs's state to the current
     *  instance. After the transfer the state originally held by this instance
     *  will be released.
     *
     *  @param[in] rhs The instance whose state is being taken. After this
     *                 function, @p rhs will not contain a PIMPL and will need
     *                 to be assigned to in order to be used again.
     *
     *  @return The current instance after taking ownership of @p rhs's state.
     *
     *  Complexity: Constant.
     */
    AOBasisSet& operator=(AOBasisSet&& rhs) noexcept;

    /// Defaulted no-throw dtor
    ~AOBasisSet() noexcept;

    // -------------------------------------------------------------------------
    // -- AtomicBasisSet getters/setters
    // -------------------------------------------------------------------------

    /** @brief Adds an additional AtomicBasisSet instance to this basis set.
     *
     *  @param[in] center The center to add to this basis set.
     *
     *  @throw std::bad_alloc if the underlying buffers need to reallocate and
     *                        there is insufficient memory to do so. Strong
     *                        throw guarantee.
     */
    void add_center(value_type center);

    /** @brief Returns the range of shell indices for the requested center
     *
     *  @return A pair whose first element is the index of the first shell which
     *          is part of the center and whose second index is the first shell
     *          part of the next center.
     *
     */
    typename abs_traits::range_type shell_range(size_type center) const;

    // -------------------------------------------------------------------------
    // -- Shell getters/setters
    // -------------------------------------------------------------------------

    /** @brief Determines the largest total angular momentum of the shells in
     *         the basis set.
     *
     *  Many integral libraries preallocate a scratch buffer given the maximum
     *  total angular momentum of any particular shell. This function is a
     *  convenience function for determining that value for the current basis
     *  set.
     *
     *  @return The maximum total angular momentum of the shell with the largest
     *          total angular momentum.
     *
     *  @throw std::runtime_error if this basis set does not contain any shells.
     *         Strong throw guarantee.
     *
     *  @note Since a total angular momentum of 0 is a valid result, we instead
     *        opt to throw if this function is called on an empty basis set.
     */
    size_type max_l() const;

    /** @brief Returns the total number of shells in this basis set.
     *
     *  @return The total number of shells in the basis set.
     *
     *  @throw None No throw guarantee.
     */
    size_type n_shells() const noexcept;

    /** @brief Returns the @p i-th shell in the basis set.
     *
     *  Shells are numbered by flattening out the centers comprising the basis
     *  set. The @f$n_0@f$ shells belonging to the 0-th center comprise shells
     *  0 to @f$n_0 -1@f$, the @f$n_1@f$ shells belonging to the 1-st center
     *  comprise shells @f$n_0@f$ through @f$n_0 + n_1 - 1@f$, etc.
     *
     *  @param[in] i The index of the requested shell. Must be in the range
     *               [0, n_shells()).
     *  @return A read-/write-able reference to the requested shell.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_shells()).
     *                           Strong throw guarantee.
     */
    typename abs_traits::shell_reference shell(size_type i);

    /** @brief Returns the @p i-th shell in the basis set.
     *
     *  Shells are numbered by flattening out the centers comprising the basis
     *  set. The @f$n_0@f$ shells belonging to the 0-th center comprise shells
     *  0 to @f$n_0 -1@f$, the @f$n_1@f$ shells belonging to the 1-st center
     *  comprise shells @f$n_0@f$ through @f$n_0 + n_1 - 1@f$, etc.
     *
     *  @param[in] i The index of the requested shell. Must be in the range
     *               [0, n_shells()).
     *
     *  @return A read-only reference to the requested shell.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_shells()).
     *                           Strong throw guarantee.
     */
    typename abs_traits::const_shell_reference shell(size_type i) const;

    /** @brief Returns the range of shell indices for the requested center
     *
     *  @param[in] shell The index of the requested shell. Must be in the range
     *                   [0, n_shells()).
     *
     *  @return A pair whose first element is the index of the first primitive
     *          which is part of the shell and whose second index is the first
     *          primitive that is part of the next shell.
     *
     */
    typename abs_traits::range_type primitive_range(size_type shell) const;

    // -------------------------------------------------------------------------
    // -- AO getters/setters
    // -------------------------------------------------------------------------

    /** @brief Returns the total number of AOs in this basis set.
     *
     *  This function will sum up the total number of AOs on each center in this
     *  basis set.
     *
     *  @return  The total number of AOs in this basis set.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Linear in the total number of shells.
     */
    size_type n_aos() const noexcept;

    /** @brief Returns the @p i-th AO in this basis set.
     *
     *  The AOs in this basis set are numbered such that the first @f$n_{0}@f$
     *  in this basis set's 0-th shell are numbered 0 to @f$n_{0} -1@f$. The
     *  @f$n_{1}@f$ AOs in the basis set's 1-st shell are numbered @f$n_0@f$ to
     *  @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested AO. Must be in the range [0,
     *               n_aos()).
     *  @return A read-/write-able reference to the requested AO.
     *
     *  @throw std::out_of_range if the requested AO is not in the range [0,
     *                           n_aos()). Strong throw guarantee.
     */
    // ao_reference ao(size_type i);

    /** @brief Returns the @p i-th AO in this basis set.
     *
     *  The AOs in this basis set are numbered such that the first @f$n_{0}@f$
     *  in this basis set's 0-th shell are numbered 0 to @f$n_{0} -1@f$. The
     *  @f$n_{1}@f$ AOs in the basis set's 1-st shell are numbered @f$n_0@f$ to
     *  @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested AO. Must be in the range [0,
     *               n_aos()).
     *
     *  @return A read-only reference to the requested AO.
     *
     *  @throw std::out_of_range if the requested AO is not in the range [0,
     *                           n_aos()). Strong throw guarantee.
     */
    // const_ao_reference ao(size_type i) const;

    // ---------------------------- Primitives ---------------------------------

    /** @brief Returns the total number of primitives in this basis set.
     *
     *  This function will compute the total number of primitives in this basis
     *  set.
     *
     *  @return The total number of primitives in this basis set.
     *
     *  @throw None No throw guarantee.
     */
    size_type n_primitives() const noexcept;

    /** @brief Returns the @p i-th primitive in this basis set.
     *
     *  The primitives in this basis set are numbered such that the first
     *  @f$n_{0}@f$  in this basis set's 0-th AO are numbered 0 to
     *  @f$n_{0} -1@f$. The @f$n_{1}@f$ primitives in the basis set's
     *  1-st AO are numbered @f$n_0@f$ to @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested primitive. Must be in the
     *               range [0, n_primitives()).
     *
     *  @return A read-/write-able reference to the requested primitive.
     *
     *  @throw std::out_of_range if the requested primitive is not in the
     *                           range [0, n_primitives()). Strong throw
     *                           guarantee.
     */
    typename abs_traits::primitive_reference primitive(size_type i);

    /** @brief Returns the @p i-th primitive in this basis set.
     *
     *  The primitives in this basis set are numbered such that the first
     *  @f$n_{0}@f$  in this basis set's 0-th AO are numbered 0 to
     *  @f$n_{0} -1@f$. The @f$n_{1}@f$ primitives in the basis set's
     *  1-st AO are numbered @f$n_0@f$ to @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested primitive. Must be in the
     *               range [0, n_primitives()).
     *
     *  @return A read-only reference to the requested primitive.
     *
     *  @throw std::out_of_range if the requested primitive is not in the
     *                           range [0, n_primitives()). Strong throw
     *                           guarantee.
     */
    typename abs_traits::const_primitive_reference primitive(size_type i) const;

    // -------------------------------------------------------------------------
    // -- Utility functions
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
    void swap(AOBasisSet& other) noexcept;

    /** @brief Makes this AOBasisSet the union of this set and @p rhs.
     *
     *  Strictly speaking, this function will concatenate the basis functions in
     *  the present basis set with those in @p rhs. It's only a union if the
     *  basis sets are disjoint.
     *
     *  @param[in] rhs The basis we are taking the union with.
     *
     *  @return The current basis set, after adding @p rhs to it.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new basis functions. Weak throw guarantee.
     */
    AOBasisSet& operator+=(const AOBasisSet& rhs);

    /** @brief Computes the AOBasisSet which is the union of this and @p rhs.
     *
     *  Strictly speaking, this function will concatenate the basis functions in
     *  the present basis set with those in @p rhs. It's only a union if the
     *  basis sets are disjoint.
     *
     *  @param[in] rhs The basis we are taking the union with.
     *
     *  @return The basis set resulting from the union of this with @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new basis functions. Strong throw guarantee.
     */
    AOBasisSet operator+(const AOBasisSet& rhs) const;

private:
    /// Allows the base class to implement container API
    friend base_type;

    AOBasisSet(pimpl_pointer pimpl) noexcept;

    bool has_pimpl_() const noexcept;

    /// Raise std::out_of_range if invalid center index
    void assert_center_index_(size_type center) const;

    /// Raise std::out_of_range if invalid shell index
    void assert_shell_index_(size_type shell) const;

    /// Raise std::out_of_range if invalid primitive index
    void assert_primitive_index_(size_type primitive) const;

    /// Implements `size()` function
    size_type size_() const noexcept;

    /// Implements `operator[](size_type i)`
    reference at_(size_type i);

    /// Implements `operator[](size_type i)const`
    const_reference at_(size_type i) const;

    /// The instance actually implementing the API
    pimpl_pointer m_pimpl_;
}; // class AOBasisSet

using AOBasisSetD = AOBasisSet<AtomicBasisSetD>;
using AOBasisSetF = AOBasisSet<AtomicBasisSetF>;

extern template class AOBasisSet<AtomicBasisSetD>;
extern template class AOBasisSet<AtomicBasisSetF>;

} // namespace chemist::basis_set