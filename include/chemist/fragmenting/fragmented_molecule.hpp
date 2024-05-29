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
#include <chemist/chemical_system/molecule/molecule.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/fragmenting/fragmented_nuclei.hpp>
#include <chemist/traits/fragmenting_traits.hpp>

namespace chemist::fragmenting {
namespace detail_ {
template<typename MoleculeType>
class FragmentedMoleculePIMPL;

}

/** @brief Stores pieces of a Molecule object.
 *
 *  @tparam MoleculeType Type of the object being fragmented. Assumed to be
 *                       either Molecule or const Molecule.
 *
 *  When fragmenting a Molecule object the resulting fragments are also
 *  Molecule objects. This class stores the supersystem Molecule object and
 *  the fragments. For each fragment this class stores the nuclei in the
 *  fragment, the charge of the fragment, and the multiplicity of the fragment.
 *
 *  See https://tinyurl.com/32kezy43 for design notes.
 */
template<typename MoleculeType>
class FragmentedMolecule
  : public FragmentedBase<FragmentedMolecule<MoleculeType>> {
private:
    /// Type of this class
    using my_type = FragmentedMolecule<MoleculeType>;

    /// Type of the base class
    using base_type = FragmentedBase<my_type>;

public:
    /// Type of the object implementing *this
    using pimpl_type = detail_::FragmentedMoleculePIMPL<MoleculeType>;

    /// Type of a pointer to the implementation
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Struct defining the types *this uses
    using type_traits = ChemistClassTraits<my_type>;

    /// Makes base class's types available here
    ///@{
    using typename base_type::const_supersystem_reference;
    using typename base_type::supersystem_reference;
    using typename base_type::supersystem_type;
    ///@}

    /// Struct defining the types used to fragment the Nuclei part of *this
    using fragmented_nuclei_traits =
      typename type_traits::fragmented_nuclei_traits;

    /// Type of the fragments
    using value_type = typename type_traits::fragment_type;

    /// Type of a mutable reference to a fragment
    using reference = typename type_traits::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename type_traits::const_fragment_reference;

    /// Type of the piece of *this representing the fragmented Nuclei object
    using fragmented_nuclei_type =
      typename fragmented_nuclei_traits::value_type;

    /// Type of each fragment's charge
    using charge_type = typename value_type::charge_type;

    /// Type of each fragment's multiplicity
    using multiplicity_type = typename value_type::multiplicity_type;

    /// Type supersystem uses for indexing and offsets
    using size_type = typename supersystem_type::size_type;

    /// Type *this uses for holding the charges
    using charge_container = std::vector<charge_type>;

    /// Type *this uses for holding the multiplicities
    using multiplicity_container = std::vector<multiplicity_type>;

    /** @brief Fragments an empty Molecule object.
     *
     *  Default constructed objects are primarily useful as placeholders. To
     *  assign state to them use either copy or move assignment from a stateful
     *  FragmentedMolecule object.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedMolecule() noexcept;

    /** @brief Initializes an object capable of fragmenting @p molecule.
     *
     *  This ctor is meant primarily for use when you know the molecule to
     *  fragment, but have not actually fragmented it yet. The resulting object
     *  will be capable of storing subsets of @p molecule.
     *
     *
     *  @param[in] molecule The object to be fragmented.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    explicit FragmentedMolecule(supersystem_type molecule);

    /** @brief Initializes *this given a fragmented Nuclei object.
     *
     *  @warning For initialization purposes all fragments are assumed neutral
     *           singlets (even number of electrons) or neutral doublets (odd
     *           number of electrons). Either use the primary ctor to set the
     *           charges and multiplicities for each fragment, or manually
     *           modify the
     *       fragments, e.g. this->at(i).set_multiplicity().
     *
     *  @param[in] nuclear_frags How the nuclear framework is fragmented.
     *  @param[in] charge        The total electronic charge of the supersystem.
     *  @param[in] multiplicity  The multiplicity of the supersystem.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    FragmentedMolecule(fragmented_nuclei_type nuclear_frags, charge_type charge,
                       multiplicity_type multiplicity);

    /** @brief The primary value ctor
     *
     *  @warning No check is performed to ensure that the charges and/or
     *           multiplicities of the fragments are consistent with the overall
     *           charge and/or multiplicity of the supersystem. Checks are
     *           however performed to ensure each fragment's charge and
     *           multiplicity is consistent with its set of nuclei.
     *
     *  This is the primary value ctor, all other value ctors dispatch to it.
     *
     *  @param[in] nuclear_frags How the Nucleus part of the Molecule is
     *                           fragmented.
     *  @param[in] charge The total electronic charge of the supersystem.
     *  @param[in] multiplicity The multiplicity of the supersystem.
     *  @param[in] charges The electronic charge of each fragment.
     *  @param[in] multiplicities The multiplicity of each fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     *  @throw std::runtime_error if the resulting fragments have non-physical
     *                            charges or multiplicities.
     */
    FragmentedMolecule(fragmented_nuclei_type nuclear_frags, charge_type charge,
                       multiplicity_type multiplicity, charge_container charges,
                       multiplicity_container multiplicities);

    /** @brief Initializes *this to a deep copy of @p other.
     *
     *  @param[in] other The set of fragments to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state for
     *                        *this. Strong throw guarantee.
     */
    FragmentedMolecule(const FragmentedMolecule& other);

    /** @brief Initializes *this by taking the state from @p other.
     *
     *  @param[in,out] other The set of fragments to take the state from. After
     *                       calling this method @p other will be in a state
     *                       consistent with fragmenting an empty set.
     *
     *  @throw None No throw guarantee.
     *
     */
    FragmentedMolecule(FragmentedMolecule&& other) noexcept;

    /** @brief Replaces the state in *this with a copy of the stat in @p rhs.
     *
     *  @param[in] rhs The object whose state will be copied.
     *
     *  @return *this after replacing its state with a copy of the state in
     *          @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    FragmentedMolecule& operator=(const FragmentedMolecule& rhs);

    /** @brief Replaces the state in *this with the state in @p rhs.
     *
     *  @param[in,out] rhs The object whose state will be taken. After this
     *                     method returns @p rhs will be in a state consistent
     *                     with fragmenting an empty set.
     *
     *  @return *this after replacing its state with the state in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedMolecule& operator=(FragmentedMolecule&& rhs) noexcept;

    /// No throw defaulted dtor
    ~FragmentedMolecule() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters and setters
    // -------------------------------------------------------------------------

    /** @brief Returns the FragmentedNuclei piece of *this
     *
     *
     *  @return A mutable reference to FragmentedNuclei piece of *this.
     *
     *  @throw std::runtime_error if *this does not have a PIMPL. Strong throw
     *                            guarantee.
     */
    fragmented_nuclei_type& fragmented_nuclei();

    /** @brief Returns the FragmentedNuclei piece of *this
     *
     *
     *  @return A read-onlyreference to FragmentedNuclei piece of *this.
     *
     *  @throw std::runtime_error if *this does not have a PIMPL. Strong throw
     *                            guarantee.
     */
    const fragmented_nuclei_type& fragmented_nuclei() const;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state in *this with that in @p other.
     *
     *  @param[in,out] other The object whose state is being taken. After this
     *                       method is called @p other will contain the state
     *                       which was previously in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(FragmentedMolecule& other) noexcept;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two FragmentedMolecule objects are value equal if the supersystems they
     *  fragment are value equal, the FragmentedMolecule objects contain the
     *  same number of fragments, and if the `i`-th fragments in each
     *  FragmentedMolecule are value equal.
     *
     *  @param[in] rhs The object to compare *this to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FragmentedMolecule& rhs) const noexcept;

    /** @brief Is *this different than @p rhs?
     *
     *  Two FragmentedMolecule objects are different if they are not value
     *  equal. Hence this method simply negates operator==. See operator==
     *  for more details.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return False if *this and @p rhs are value equal and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const FragmentedMolecule& rhs) const noexcept;

protected:
    friend utilities::IndexableContainerBase<my_type>;

    /// Implements at() and operator[]
    reference at_(size_type i);

    /// Implements at() const and operator[] const
    const_reference at_(size_type i) const;

    /// Implements size
    size_type size_() const noexcept;

    /// Implements supersystem()
    virtual supersystem_reference supersystem_() override;

    /// Implements supersystem() const
    virtual const_supersystem_reference supersystem_() const override;

private:
    /// Code factorization for determining if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// Throws std::runtime_error if *this has no PIMPL
    void assert_pimpl_() const;

    /// The object which actually implements *this
    pimpl_pointer m_pimpl_;
};

extern template class FragmentedMolecule<Molecule>;
extern template class FragmentedMolecule<const Molecule>;

} // namespace chemist::fragmenting
