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
#include <chemist/chemical_system/chemical_system/chemical_system.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/fragmenting/fragmented_molecule.hpp>
#include <chemist/traits/fragmenting_traits.hpp>

namespace chemist::fragmenting {
namespace detail_ {
template<typename ChemicalSystemType>
class FragmentedChemicalSystemPIMPL;

}

/** @brief Stores fragments of a ChemicalSystem object.
 *
 *  @tparam ChemicalSystemType The type of the ChemicalSystem object we are
 *                             fragmenting. Expected to be either ChemicalSystem
 *                             or const ChemicalSystem.
 *
 * This class holds a ChemicalSystem supersystem and the fragments of that
 * object.
 *
 */

template<typename ChemicalSystemType>
class FragmentedChemicalSystem
  : public FragmentedBase<FragmentedChemicalSystem<ChemicalSystemType>> {
private:
    /// Type of *this
    using my_type = FragmentedChemicalSystem<ChemicalSystemType>;

    /// Type of the base class
    using base_type = FragmentedBase<my_type>;

public:
    /// Type of the PIMPL implementing *this
    using pimpl_type =
      detail_::FragmentedChemicalSystemPIMPL<ChemicalSystemType>;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Struct defining the types *this uses
    using type_traits = ChemistClassTraits<my_type>;

    /// Struct defining types for the FragmentedMolecule part of *this
    using fragmented_molecule_traits =
      typename type_traits::fragmented_molecule_traits;

    /// Type of a fragment
    using value_type = typename type_traits::fragment_type;

    /// Type of a reference to a fragment
    using reference = typename type_traits::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename type_traits::const_fragment_reference;

    /// Type of an object managing the FragmentedMolecule piece of *this
    using fragmented_molecule_type =
      typename fragmented_molecule_traits::value_type;

    /// Pull in base class's types
    ///@{
    using typename base_type::const_supersystem_reference;
    using typename base_type::supersystem_reference;
    using typename base_type::supersystem_type;
    ///@}

    /// Type the superysystem uses for indexing and offsets
    using size_type = typename supersystem_type::size_type;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Fragments an empty ChemicalSystem.
     *
     *  The default ctor prepares *this in a state consistent with the
     *  supersystem being an empty ChemicalSystem object and *this having no
     *  fragments of that empty object.
     *
     *  @throw None No throw guarantee.
     *
     */
    FragmentedChemicalSystem() noexcept;

    /** @brief Fragments are formed the provided state.
     *
     *  This method creates an object such that the Molecule piece of the
     *  supersystem is `frags.supersystem()`. The supersystem is then fragmented
     *  into `frags.size()` fragments such that the Molecule piece of the
     * `i`-th fragment is `frags[i]`.
     *
     *  @param[in] frags How the Molecule piece of the supersystem is fragmented
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    explicit FragmentedChemicalSystem(fragmented_molecule_type frags);

    /** @brief Initializes *this with a deep copy of @p rhs.
     *
     *  @param[in] other The object to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    FragmentedChemicalSystem(const FragmentedChemicalSystem& other);

    /** @brief Initializes *this with the state in @p rhs.
     *
     *  This method will initialize *this by taking the state from @p rhs.
     *
     *  @param[in,out] rhs The object to take the state from. After this ctor
     *                     is called @p rhs will be in a state consistent with
     *                     being default initialized.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedChemicalSystem(FragmentedChemicalSystem&& other) noexcept;

    /** @brief Replaces the state in *this with a copy of the state in @p rhs.
     *
     *  This method releases the state in *this and replaces it with a copy of
     *  the state in @p rhs.
     *
     *  @param[in] rhs The object to copy.
     *
     *  @return *this after setting its state to a copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        copy. Strong throw guarantee.
     */
    FragmentedChemicalSystem& operator=(const FragmentedChemicalSystem& rhs);

    /** @brief Replaces the state in *this with the state in @p rhs.
     *
     *  This method will release the state currently held by *this and then
     *  take the state from @p rhs.
     *
     *  @param[in,out] rhs The object to take the state from. After this method
     *                     is called @p rhs will be in a state consistent with
     *                     being default initialized.
     *
     *  @return *this after taking the state from @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedChemicalSystem& operator=(
      FragmentedChemicalSystem&& rhs) noexcept;

    /// Defaulted no-throw dtor
    ~FragmentedChemicalSystem() noexcept;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state in *this with that in @p other.
     *
     *  @param[in,out] other The instance to exchange state with. After this
     *                       method @p other will contain the state which was
     *                       previously in *this.
     *
     *  @throw None No throw gurantee.
     */
    void swap(FragmentedChemicalSystem& other) noexcept;

    /** @brief Is *this value equal to @p rhs?
     *
     *  Two FragmentedChemicalSystem objects are value equal if:
     *  - the supersystems they fragment are value equal,
     *  - they contain the same number of fragments,
     *  - if the `i`-th fragment of *this is value equal to the `i`-th fragment
     *    of @p rhs.
     *
     *  @param[in] rhs The FragmentedChemicalSystem to compare to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FragmentedChemicalSystem& rhs) const noexcept;

    /** @brief Determines if *this is different than @p rhs.
     *
     *  This method defines "different" as not value equal. See operator== for
     *  the definition of value equality.
     *
     *  @param[in] rhs The FragmentedChemicalSystem to compare against.
     *
     *  @return False if *this and @p rhs are value equal and true otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool operator!=(const FragmentedChemicalSystem& rhs) const noexcept;

protected:
    /// Allows base class to access implementations
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
    /// Code factorization for determining if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /// The object which actually implements *this
    pimpl_pointer m_pimpl_;
};

extern template class FragmentedChemicalSystem<ChemicalSystem>;
extern template class FragmentedChemicalSystem<const ChemicalSystem>;

} // namespace chemist::fragmenting
