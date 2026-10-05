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
#include <chemist/experimental/basis_set/molecular_basis_set_class.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace chemist::experimental {

/** @brief An object which behaves like a MolecularBasisSet, but aliases its
 *         state.
 *
 *  *this is the molecular-basis-set counterpart of AtomicBasisSetView: it
 *  has the same API as the owning class, but writing through it writes into
 *  whatever owns the state. *this can not change the shape of what it
 *  aliases, so it has no push_back, and assignment writes values through
 *  rather than rebinding.
 *
 *  *this aliases each piece of the state separately (see
 *  MolecularBasisSetCommon), so the state need not be owned by a
 *  MolecularBasisSet.
 *
 *  @tparam MolecularBasisSetType A, possibly const-qualified,
 *                                MolecularBasisSet.
 *                                MolecularBasisSetView<MolecularBasisSet>
 *                                aliases mutable state and has setters;
 *                                MolecularBasisSetView<const
 *                                MolecularBasisSet> aliases read-only state
 *                                and has none.
 */
template<typename MolecularBasisSetType>
class MolecularBasisSetView
  : public MolecularBasisSetCommon<MolecularBasisSetView<MolecularBasisSetType>,
                                   MolecularBasisSetType> {
private:
    /// Type *this inherits from
    using base_type =
      MolecularBasisSetCommon<MolecularBasisSetView<MolecularBasisSetType>,
                              MolecularBasisSetType>;

    /// Struct defining the types of the set *this acts like
    using traits_type = ChemistClassTraits<MolecularBasisSetType>;

    /// Lets the CRTP bases reach the *_data_ hooks
    template<typename OtherDerived, typename OtherType>
    friend class MolecularBasisSetCommon;

    /// Lets the other const-qualification of *this reach its members
    template<typename OtherType>
    friend class MolecularBasisSetView;

    /// Is the aliased state read-only?
    static constexpr bool is_const = std::is_const_v<MolecularBasisSetType>;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<is_const && !std::is_const_v<T>>;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::buffer_reference;
    using typename base_type::center_set_reference;
    using typename base_type::size_type;
    ///@}

    /// Type of a, possibly read-only, view of the angular momenta
    using l_span = typename traits_type::l_span;

    /// Type of a read-only view of a set of offsets
    using offset_span = typename traits_type::offset_span;

    /// Type of a, possibly read-only, view of the names
    using name_span = typename traits_type::name_span;

    /// Type of a, possibly read-only, view of the atomic numbers
    using atomic_number_span = typename traits_type::atomic_number_span;

    /// Type of a read-only view of the purity of each atom's shells
    using purity_span = typename traits_type::purity_span;

    /// Type of a read-only view of the AO ordering of each atom's shells
    using ordering_span = typename traits_type::ordering_span;

    /// Type of the owning set *this materializes into
    using molecular_basis_set_type = MolecularBasisSet;

    /// Type of a, possibly read-only, reference to the aliased set
    using molecular_basis_set_reference = typename traits_type::reference;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view of @p mbs.
     *
     *  This ctor is deliberately implicit so that a MolecularBasisSet can be
     *  passed wherever a MolecularBasisSetView is expected. The result
     *  aliases @p mbs's state; writing through *this writes into @p mbs. It
     *  is invalidated by anything which reallocates that state, e.g. adding
     *  an atom to @p mbs.
     *
     *  @param[in] mbs The set *this will alias.
     *
     *  @throw None No throw guarantee.
     */
    MolecularBasisSetView(molecular_basis_set_reference mbs) :
      MolecularBasisSetView(mbs.coefficient_data_(), mbs.exponent_data_(),
                            mbs.l_data_(), mbs.primitive_offset_data_(),
                            mbs.shell_offset_data_(), mbs.center_data_(),
                            mbs.name_data_(), mbs.atomic_number_data_(),
                            mbs.purity_data_(), mbs.ordering_data_()) {}

    /** @brief Creates a view aliasing the provided state.
     *
     *  This ctor is what lets *this alias state which is not owned by a
     *  MolecularBasisSet at all. The state is laid out as described in
     *  MolecularBasisSetCommon. Offsets are taken relative to the first
     *  element of their array, which is where the arrays they index are
     *  taken to start.
     *
     *  @param[in] coefficients Every coefficient, shell-major.
     *  @param[in] exponents Every exponent, shell-major.
     *  @param[in] ls The angular momentum of each shell.
     *  @param[in] primitive_offsets The offsets of the shells into the
     *                               parameter arrays, plus one past the end.
     *  @param[in] shell_offsets The offsets of the atoms into the shells,
     *                           plus one past the end.
     *  @param[in] centers The center of each atom.
     *  @param[in] names The name of each atom's basis set.
     *  @param[in] atomic_numbers The atomic number of each atom.
     *  @param[in] purities The purity of each atom's shells.
     *  @param[in] orderings The AO ordering of each atom's shells.
     *
     *  @throw std::invalid_argument if the arrays do not describe the same
     *                               numbers of atoms, shells, and primitives.
     *                               Strong throw guarantee.
     */
    MolecularBasisSetView(buffer_reference coefficients,
                          buffer_reference exponents, l_span ls,
                          offset_span primitive_offsets,
                          offset_span shell_offsets,
                          center_set_reference centers, name_span names,
                          atomic_number_span atomic_numbers,
                          purity_span purities, ordering_span orderings) :
      m_coefficients_(std::move(coefficients)),
      m_exponents_(std::move(exponents)),
      m_ls_(ls),
      m_primitive_offsets_(primitive_offsets),
      m_shell_offsets_(shell_offsets),
      m_centers_(std::move(centers)),
      m_names_(names),
      m_atomic_numbers_(atomic_numbers),
      m_purities_(purities),
      m_orderings_(orderings) {
        const auto n_atoms = m_names_.size();
        if(m_shell_offsets_.size() != n_atoms + 1 ||
           m_centers_.size() != n_atoms ||
           m_atomic_numbers_.size() != n_atoms ||
           m_purities_.size() != n_atoms || m_orderings_.size() != n_atoms)
            throw_bad_state_("there must be one center, atomic number, "
                             "purity, and AO ordering per name, and one more "
                             "atom offset than there are names");
        if(m_shell_offsets_.back() - m_shell_offsets_.front() != m_ls_.size() ||
           m_primitive_offsets_.size() != m_ls_.size() + 1)
            throw_bad_state_("the atom offsets must describe exactly the "
                             "shells, and there must be one more shell offset "
                             "than there are shells");
        const auto n =
          m_primitive_offsets_.back() - m_primitive_offsets_.front();
        if(m_coefficients_.size() != n || m_exponents_.size() != n)
            throw_bad_state_("the parameter arrays must hold exactly the "
                             "primitives the shell offsets describe");
    }

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherType,
             typename = enable_mutable_to_const_t<OtherType>>
    MolecularBasisSetView(const MolecularBasisSetView<OtherType>& other) :
      m_coefficients_(other.m_coefficients_),
      m_exponents_(other.m_exponents_),
      m_ls_(other.m_ls_),
      m_primitive_offsets_(other.m_primitive_offsets_),
      m_shell_offsets_(other.m_shell_offsets_),
      m_centers_(other.m_centers_),
      m_names_(other.m_names_),
      m_atomic_numbers_(other.m_atomic_numbers_),
      m_purities_(other.m_purities_),
      m_orderings_(other.m_orderings_) {}

    /** @brief Creates a view aliasing the same state as @p other.
     *
     *  The copy is shallow.
     *
     *  @throw None No throw guarantee.
     */
    MolecularBasisSetView(const MolecularBasisSetView& other) = default;

    /** @brief Takes the aliased state from @p other.
     *
     *  @throw None No throw guarantee.
     */
    MolecularBasisSetView(MolecularBasisSetView&& other) noexcept = default;

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's values through into whatever *this is already
     *        aliasing. Use swap if you want to rebind *this.
     *
     *  The shape of the aliased state can not be changed through a view, so
     *  @p rhs must have the same number of atoms as *this, and each of its
     *  atoms must hold shells of the same type, the same number of shells,
     *  and the same number of primitives in each shell as the corresponding
     *  atom of *this. Its names, atomic numbers, centers, angular momenta,
     *  and parameters are then written into the state *this aliases.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherType The molecular-basis-set type @p rhs models.
     *
     *  @param[in] rhs The set whose values should be copied into the state
     *                 *this aliases.
     *
     *  @return *this, after overwriting the aliased state.
     *
     *  @throw std::runtime_error if the shape of @p rhs differs from that of
     *                            *this (strong throw guarantee), or if the
     *                            concrete floating-point types of the
     *                            parameters differ (weak throw guarantee).
     */
    template<typename OtherDerived, typename OtherType>
    MolecularBasisSetView& operator=(
      const MolecularBasisSetCommon<OtherDerived, OtherType>& rhs) {
        static_assert(
          !is_const,
          "Can not assign through a read-only MolecularBasisSetView.");
        this->assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  See the templated assignment operator; this is the copy assignment
     *  operator and behaves identically.
     */
    MolecularBasisSetView& operator=(const MolecularBasisSetView& rhs) {
        static_assert(
          !is_const,
          "Can not assign through a read-only MolecularBasisSetView.");
        if(this != &rhs) this->assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  This is identical to the copy assignment operator; there is nothing to
     *  steal, because *this does not own what it aliases.
     */
    MolecularBasisSetView& operator=(MolecularBasisSetView&& rhs) {
        static_assert(
          !is_const,
          "Can not assign through a read-only MolecularBasisSetView.");
        if(this != &rhs) this->assign_(rhs);
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~MolecularBasisSetView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a MolecularBasisSet holding a copy of the aliased
     *         state.
     *
     *  @return An owning set, independent of what *this aliases.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    molecular_basis_set_type as_molecular_basis_set() const {
        return molecular_basis_set_type(this->begin(), this->end());
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased values.
     *
     *  @throw None No throw guarantee.
     */
    void swap(MolecularBasisSetView& other) noexcept {
        // As in PointSetView::swap, moving a view rebinds it
        std::swap(m_coefficients_, other.m_coefficients_);
        std::swap(m_exponents_, other.m_exponents_);
        std::swap(m_ls_, other.m_ls_);
        std::swap(m_primitive_offsets_, other.m_primitive_offsets_);
        std::swap(m_shell_offsets_, other.m_shell_offsets_);
        m_centers_.swap(other.m_centers_);
        std::swap(m_names_, other.m_names_);
        std::swap(m_atomic_numbers_, other.m_atomic_numbers_);
        std::swap(m_purities_, other.m_purities_);
        std::swap(m_orderings_, other.m_orderings_);
    }

private:
    /// Throws std::invalid_argument explaining why the state was rejected
    [[noreturn]] static void throw_bad_state_(const std::string& why) {
        throw std::invalid_argument(
          "chemist::experimental::MolecularBasisSetView: " + why + ".");
    }

    /** @brief Implements the CRTP base's state access.
     *
     *  Only const-qualified overloads are needed: whether the returned views
     *  are mutable is decided by MolecularBasisSetType, not by the
     *  const-qualification of *this.
     */
    ///@{
    buffer_reference coefficient_data_() const { return m_coefficients_; }
    buffer_reference exponent_data_() const { return m_exponents_; }
    l_span l_data_() const noexcept { return m_ls_; }
    offset_span primitive_offset_data_() const noexcept {
        return m_primitive_offsets_;
    }
    offset_span shell_offset_data_() const noexcept { return m_shell_offsets_; }
    center_set_reference center_data_() const { return m_centers_; }
    name_span name_data_() const noexcept { return m_names_; }
    atomic_number_span atomic_number_data_() const noexcept {
        return m_atomic_numbers_;
    }
    purity_span purity_data_() const noexcept { return m_purities_; }
    ordering_span ordering_data_() const noexcept { return m_orderings_; }
    ///@}

    /// Aliases the coefficients of every primitive
    buffer_reference m_coefficients_;

    /// Aliases the exponents of every primitive
    buffer_reference m_exponents_;

    /// Aliases the angular momentum of each shell
    l_span m_ls_;

    /// Aliases the offsets of the shells into the parameter arrays
    offset_span m_primitive_offsets_;

    /// Aliases the offsets of the atoms into the shells
    offset_span m_shell_offsets_;

    /// Aliases the centers
    center_set_reference m_centers_;

    /// Aliases the names
    name_span m_names_;

    /// Aliases the atomic numbers
    atomic_number_span m_atomic_numbers_;

    /// Aliases the purity of each atom's shells
    purity_span m_purities_;

    /// Aliases the AO ordering of each atom's shells
    ordering_span m_orderings_;
};

/// Type of a view of a mutable MolecularBasisSet
using molecular_basis_set_view = MolecularBasisSetView<MolecularBasisSet>;

/// Type of a view of a read-only MolecularBasisSet
using const_molecular_basis_set_view =
  MolecularBasisSetView<const MolecularBasisSet>;

/** @note Only the mutable instantiation is declared here, for the reason
 *        given on ContractedGaussianView's: explicitly instantiating the
 *        read-only one would force its (deliberately non-compiling)
 *        copy-assignment operator.
 */
extern template class MolecularBasisSetView<MolecularBasisSet>;

} // namespace chemist::experimental
