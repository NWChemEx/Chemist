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
#include <chemist/experimental/basis_set/atomic_basis_set_class.hpp>
#include <chemist/experimental/basis_set/atomic_basis_set_view.hpp>
#include <chemist/experimental/basis_set/molecular_basis_set_common.hpp>
#include <chemist/experimental/point/point_set_class.hpp>
#include <initializer_list>
#include <span>
#include <utility>
#include <vector>

namespace chemist::experimental {

/** @brief Models the basis set of a molecule by value.
 *
 *  See MolecularBasisSetCommon for the shared API and for how the atomic
 *  basis sets are stored. *this owns that state. See MolecularBasisSetView
 *  for a class with the same API which aliases state owned by something
 *  else.
 *
 *  Every member of *this is a value, so copying *this is a deep copy, and
 *  moving it moves the state.
 *
 *  A default-constructed instance holds no atoms.
 *
 *  @note A moved-from instance may only be assigned to, swapped, or
 *        destroyed.
 */
class MolecularBasisSet
  : public MolecularBasisSetCommon<MolecularBasisSet, MolecularBasisSet> {
private:
    /// Type *this inherits from
    using base_type =
      MolecularBasisSetCommon<MolecularBasisSet, MolecularBasisSet>;

    /// Struct defining the types of *this
    using traits_type = ChemistClassTraits<MolecularBasisSet>;

    /// Lets the CRTP bases reach the *_data_ hooks
    template<typename OtherDerived, typename OtherType>
    friend class MolecularBasisSetCommon;

    /// Lets a view alias *this's state
    template<typename MolecularBasisSetType>
    friend class MolecularBasisSetView;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_type;
    using typename base_type::buffer_reference;
    using typename base_type::buffer_type;
    using typename base_type::center_set_reference;
    using typename base_type::const_buffer_reference;
    using typename base_type::const_center_set_reference;
    using typename base_type::name_type;
    using typename base_type::size_type;
    using typename base_type::value_type;
    ///@}

    /// Type of a read-only view of the atomic basis set to add
    using const_atomic_basis_set_view_type =
      typename traits_type::const_atomic_basis_set_reference;

    /// Type used to own the centers
    using center_set_type = typename traits_type::center_set_type;

    /// Type used to model an atomic number
    using atomic_number_type = typename traits_type::atomic_number_type;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /// Creates a set with no atoms
    MolecularBasisSet() = default;

    /** @brief Creates a set holding copies of the provided atomic basis sets.
     *
     *  @param[in] atoms The atomic basis sets, in the order they should
     *                   appear in *this.
     *
     *  @throw std::runtime_error under the same conditions as push_back.
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the state.
     *                        Strong throw guarantee.
     */
    MolecularBasisSet(std::initializer_list<value_type> atoms) :
      MolecularBasisSet(atoms.begin(), atoms.end()) {}

    /** @brief Creates a set holding copies of the atomic basis sets in the
     *         range [@p begin, @p end).
     *
     *  @tparam BeginItr The type of @p begin.
     *  @tparam EndItr The type of @p end.
     *
     *  @param[in] begin An iterator to the first atomic basis set to add.
     *  @param[in] end An iterator to just past the last atomic basis set to
     *                 add. Dereferencing an iterator must give something
     *                 convertible to a read-only AtomicBasisSetView, e.g. an
     *                 AtomicBasisSet or a view of one.
     *
     *  @throw std::runtime_error under the same conditions as push_back.
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the state.
     *                        Strong throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    MolecularBasisSet(BeginItr&& begin, EndItr&& end) {
        for(auto itr = begin; itr != end; ++itr) push_back(*itr);
    }

    /// Defaulted deep copy/move ctors and assignment operators, and dtor
    ///@{
    MolecularBasisSet(const MolecularBasisSet& other)                = default;
    MolecularBasisSet(MolecularBasisSet&& other) noexcept            = default;
    MolecularBasisSet& operator=(const MolecularBasisSet& other)     = default;
    MolecularBasisSet& operator=(MolecularBasisSet&& other) noexcept = default;
    ~MolecularBasisSet() noexcept                                    = default;
    ///@}

    // -------------------------------------------------------------------------
    // -- Adding atoms
    // -------------------------------------------------------------------------

    /** @brief Adds a copy of @p atom to the end of *this.
     *
     *  Only the values of @p atom are copied into *this; *this does not alias
     *  @p atom afterwards. Its state is appended to the state of *this in
     *  place, so building a set atom by atom costs time linear in the size
     *  of the set.
     *
     *  @warning This may reallocate the state of *this and therefore
     *           invalidates every atomic basis set, shell, primitive, center,
     *           and buffer view previously obtained from *this.
     *
     *  @param[in] atom The atomic basis set to add. An AtomicBasisSet, or
     *                  either kind of view of one, converts to this
     *                  implicitly.
     *
     *  @throw std::runtime_error if the coefficients, exponents, or center
     *                            coordinates of @p atom are not of the same
     *                            concrete floating-point type as the ones
     *                            already in *this. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        If this happens part way through appending,
     *                        *this is left empty (basic throw guarantee).
     */
    void push_back(const_atomic_basis_set_view_type atom);

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @throw None No throw guarantee.
     */
    void swap(MolecularBasisSet& other) noexcept;

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what MolecularBasisSetCommon::save wrote, replacing
     *  whatever *this held beforehand.
     *
     *  @tparam Archive The type of the cereal input archive.
     *
     *  @param[in,out] ar The archive to read from.
     *
     *  @throw std::runtime_error if the archive names a floating-point type
     *                            chemist does not know about or can not
     *                            deserialize. Strong throw guarantee.
     */
    template<typename Archive>
    void load(Archive& ar) {
        size_type n_atoms{};
        ar(n_atoms);
        MolecularBasisSet buffer;
        for(size_type a = 0; a < n_atoms; ++a) {
            value_type atom;
            atom.load(ar);
            buffer.push_back(atom);
        }
        swap(buffer);
    }

private:
    /// Resets *this to an empty set. Used to recover from a failed push_back.
    void clear_() noexcept;

    /// Implements the CRTP base's state access
    ///@{
    buffer_reference coefficient_data_() { return m_coefficients_.as_view(); }
    const_buffer_reference coefficient_data_() const {
        return m_coefficients_.as_view();
    }
    buffer_reference exponent_data_() { return m_exponents_.as_view(); }
    const_buffer_reference exponent_data_() const {
        return m_exponents_.as_view();
    }
    typename traits_type::l_span l_data_() noexcept { return m_ls_; }
    typename traits_type::const_l_span l_data_() const noexcept {
        return m_ls_;
    }
    typename traits_type::offset_span primitive_offset_data_() const noexcept {
        return m_primitive_offsets_;
    }
    typename traits_type::offset_span shell_offset_data_() const noexcept {
        return m_shell_offsets_;
    }
    center_set_reference center_data_() {
        return center_set_reference(m_centers_);
    }
    const_center_set_reference center_data_() const {
        return const_center_set_reference(m_centers_);
    }
    typename traits_type::name_span name_data_() noexcept { return m_names_; }
    typename traits_type::const_name_span name_data_() const noexcept {
        return m_names_;
    }
    typename traits_type::atomic_number_span atomic_number_data_() noexcept {
        return m_atomic_numbers_;
    }
    typename traits_type::const_atomic_number_span atomic_number_data_()
      const noexcept {
        return m_atomic_numbers_;
    }
    typename traits_type::purity_span purity_data_() const noexcept {
        return m_purities_;
    }
    typename traits_type::ordering_span ordering_data_() const noexcept {
        return m_orderings_;
    }
    ///@}

    /// The coefficient of every primitive of every shell of every atom
    buffer_type m_coefficients_;

    /// The exponent of every primitive of every shell of every atom
    buffer_type m_exponents_;

    /// The angular momentum of each shell
    std::vector<angular_momentum_type> m_ls_;

    /// The offsets of the shells into the parameter arrays, plus one past
    /// the end; so shell s owns [m_primitive_offsets_[s],
    /// m_primitive_offsets_[s + 1])
    std::vector<size_type> m_primitive_offsets_{0};

    /// The offsets of the atoms into the shells, plus one past the end; so
    /// atom a owns shells [m_shell_offsets_[a], m_shell_offsets_[a + 1])
    std::vector<size_type> m_shell_offsets_{0};

    /// The center of each atom
    center_set_type m_centers_;

    /// The name of each atom's basis set
    std::vector<name_type> m_names_;

    /// The atomic number of each atom
    std::vector<atomic_number_type> m_atomic_numbers_;

    /// The purity of each atom's shells
    std::vector<ShellPurity> m_purities_;

    /// The AO ordering of each atom's shells
    std::vector<AOOrdering> m_orderings_;
};

} // namespace chemist::experimental
