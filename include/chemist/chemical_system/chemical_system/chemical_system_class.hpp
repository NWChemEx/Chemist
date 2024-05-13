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
#include <chemist/chemical_system/molecule/molecule.hpp>
#include <chemist/traits/chemical_system_traits.hpp>
#include <memory>

namespace chemist {
namespace detail_ {
class ChemicalSystemPIMPL;
}

/** @brief Class describing the entire chemical system to be modeled.
 *
 *  The Molecule class models the electrons and nuclei in the system; however,
 *  there are often additional pieces of the system beyond the molecule.
 *  Typically these additional pieces are fields (e.g. electric fields from
 *  point charges), but they could also be non-atomistically modeled solvent
 */
class ChemicalSystem {
public:
    /// The type of the PIMPL which implements this class
    using pimpl_t = detail_::ChemicalSystemPIMPL;

    /// The type of the pointer storing the PIMPL
    using pimpl_ptr_t = std::unique_ptr<pimpl_t>;

    /// Type of the class defining the types for *this
    using traits_type = ChemistClassTraits<ChemicalSystem>;

    /// Type of the class defining the types for the molecule piece of *this
    using molecule_traits = typename traits_type::molecule_traits;

    /// The type of the object describing the set of atoms
    using molecule_t = typename molecule_traits::value_type;

    /// Deprecated.
    using mol_ref_t = typename molecule_traits::reference;

    /// Type of a mutable reference to the Molecule object part of *this
    using molecule_reference = typename molecule_traits::reference;

    /// Deprecated
    using const_mol_ref_t = typename molecule_traits::const_view_type;

    /// Type of a read-only reference to the Molecule part of *this
    using const_molecule_reference = typename molecule_traits::const_view_type;

    /// Type used for counting/indexing
    using size_type = std::size_t;

    /// Type used to store the charge of the system
    using charge_type = typename molecule_traits::charge_type;

    /** @brief Creates a new ChemicalSystem with default constructed members.
     *
     *  @throws std::bad_alloc if there is an error while allocating the PIMPL.
     *                         Strong throw guarantee.
     */
    ChemicalSystem();

    /** @brief Makes a new ChemicalSystem by deep copying another one.
     *
     *  The copy ctor makes a member-wise deep copy of another ChemicalSystem
     *  instance. Note that if the other ChemicalSystem is in a null state
     *  (i.e. has no PIMPL) the resulting instance will also be in a null state.
     *
     *  @param[in] other The instance to deep copy.
     *
     *  @throw std::bad_alloc if an error arises while allocating the new state.
     *                        Strong throw guarantee.
     */
    ChemicalSystem(const ChemicalSystem& other);

    /** @brief Takes ownership of another ChemicalSystem's state.
     *
     *  The move ctor will transfer the state of @p other into the newly created
     *  ChemicalSystem instance. After the move @p other will be in a null state
     *  (i.e. have no PIMPL). N.B. if @p other was in a null state to begin with
     *  the newly created ChemicalSystem instance will also be in a null state.
     *
     *  @param[in,out] other The instance whose state is being taken. After the
     *                       construction @p other will be in a null state.
     *
     *  @throws none No throw guarantee.
     */
    ChemicalSystem(ChemicalSystem&& other) noexcept;

    /** @brief Creates a new ChemicalSystem with the provided state.
     *
     *  The resulting ChemicalSystem will contain the provided molecule.
     *
     *  @param[in] mol    The molecular system to add to this system.
     *
     *  @throw std::bad_alloc if there is error while allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    explicit ChemicalSystem(molecule_t mol);

    /// Default destructor, voids all references to member data
    ~ChemicalSystem() noexcept;

    /** @brief Overwrites this system's state with a deep copy of @p rhs's
     *         state.
     *
     *  @param[in] rhs The ChemicalSystem whose state will be deep copied.
     *
     *  @return The current ChemicalSystem after setting its state to a deep
     *          copy of @p rhs's state.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the state
     *                        strong throw guarantee.
     */
    ChemicalSystem& operator=(const ChemicalSystem& rhs);

    /** @brief Overwrites this system's state with @p rhs's state.
     *
     *  @param[in,out] rhs The system whose state is being taken. After this
     *                     operation @p rhs will be in a null state.
     *
     *  @return The current ChemicalSystem after it takes ownership of @p rhs's
     *          state.
     *
     *  @throw None No throw gurantee.
     */
    ChemicalSystem& operator=(ChemicalSystem&& rhs) noexcept;

    //--------------------------- Accessors -----------------------------------

    /** @brief Returns the molecular system associated with this system.
     *
     *  @return A read/write reference to the molecular system
     *  @throws std::bad_alloc if there is a problem allocating the return
     *                         object. Strong throw guarantee.
     */
    molecule_reference molecule();

    /** @brief Returns the molecular system associated with this system.
     *
     *  @return The molecular system in a read-only state.
     *  @throws std::bad_alloc if there is a problem allocating the return
     *                         object. Strong throw guarantee.
     */
    const_molecule_reference molecule() const;

    /** @brief Determines if two ChemicalSystem instances are the same.
     *
     *
     *  Two ChemicalSystem instances are equal if all of their members compare
     *  equal. It is worth noting that since these members contain
     *  floating-point data this implies exact floating-point comparisons,
     *  i.e. 1.0000 != 1.0001.
     *
     *  @param[in] rhs The ChemicalSystem on the right of the equality operator.
     *
     *  @return True if this and @p rhs are the same and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ChemicalSystem& rhs) const noexcept;

private:
    /// Code factorization for determining if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /** @brief Returns the PIMPL in a read/write state.
     *
     *  If the instance already has a PIMPL this function simply dereferences
     *  the pointer and returns it. However, if the PIMPL has not been set this
     *  function will create it and then return a refernece to the newly created
     *  instance.
     *
     *  @return A read/write reference to this class's implementation.
     *
     *  @throws std::bad_alloc if there is no PIMPL and a problem arises while
     *                         allocating the PIMPL. Strong throw guarantee.
     */
    pimpl_t& pimpl_();

    /** @brief Returns the PIMPL in a read-only state.
     *
     *  This function will check to see that the PIMPL has been created,
     *  throwing if it has not, before returning the PIMPL.
     *
     *  @return A read-only reference to this class's implementation.
     *
     *  @throws std::runtime_error if the PIMPL has not been set.
     */
    const pimpl_t& pimpl_() const;

    /// The object actually implementing this class
    pimpl_ptr_t m_pimpl_;
};

/** @brief Determines if two ChemicalSystem instances are different.
 *
 *  @relates ChemicalSystem
 *
 *  Two ChemicalSystem instances are equal if all of their members compare
 *  equal. It is worth noting that since these members contain floating-point
 *  data this implies exact floating-point comparisons, i.e. 1.0000 != 1.0001.
 *
 *  @param[in] lhs The ChemicalSystem on the left of the inequality operator.
 *  @param[in] rhs The ChemicalSystem on the right of the inequality operator.
 *
 *  @return True if @p lhs and @p rhs are different and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const ChemicalSystem& lhs,
                       const ChemicalSystem& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace chemist
