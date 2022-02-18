#pragma once
#include "chemist/molecule/molecule.hpp"
#include "chemist/potentials/electrostatic.hpp"
#include <memory>

namespace chemist {
namespace detail_ {
class ChemicalSystemPIMPL;
}

/** @brief Class describing the entire chemical system to be modeled.
 *
 *  The Molecule class models the electrons and nuclei in the system; however,
 *  there are often additional pieces of the system beyond the atoms. Typically
 *  these additional pieces are fields (e.g. electric fields from point
 *  charges), but concievably the ChemicalSystem class could also be used to
 *  hold information such as lattice parameters and the symmetry of the system.
 */
class ChemicalSystem {
public:
    /// The type of the PIMPL which implements this class
    using pimpl_t = detail_::ChemicalSystemPIMPL;

    /// The type of the pointer storing the PIMPL
    using pimpl_ptr_t = std::unique_ptr<pimpl_t>;

    /// The type of the object describing the set of atoms
    using molecule_t = Molecule;

    /// A read/write reference to the object describing the set of atoms
    using mol_ref_t = molecule_t&;

    /// A read-only reference to the object describing the set of atoms
    using const_mol_ref_t = const molecule_t&;

    /// The type used to model the external electrostatic potential
    using epot_t = potentials::Electrostatic;

    /// A read/write reference to the external electrostatic potential
    using epot_ref_t = epot_t&;

    /// A read-only reference to the electrostatic potential
    using const_epot_ref_t = const epot_t&;

    /// Type used for counting/indexing
    using size_type = std::size_t;

    /// Type used to store the charge of the system
    using charge_type = int;

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
     *  The resulting ChemicalSystem will contain the provided molecule and
     *  external potential. The number of electrons will be determined by
     *  summing up the atomic numbers of the atoms in @p mol.
     *
     *  @param[in] mol    The molecular system to add to this system.
     *  @param[in] v      The external electrostatic potential that the system
     *                    lives in. Defaults to no potential.
     *
     *  @throw std::bad_alloc if there is error while allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    explicit ChemicalSystem(molecule_t mol, epot_t v = {});

    /** @brief Creates a new ChemicalSystem with the provided state.
     *
     *  The resulting ChemicalSystem will contain the provided molecule, number
     * of
     *  @param[in] mol    The molecular system to add to this system.
     *  @param[in] v      The external electrostatic potential that the system
     *                    lives in. Defaults to no potential.
     *
     *  @throw std::bad_alloc if there is error while allocating the PIMPL.
     *                        Strong throw guarantee.
     */

    ChemicalSystem(molecule_t mol, size_type nelectrons, epot_t v = {});

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
     *  @throws std::bad_alloc if the class has no PIMPL and an error arises
     *                         while allocating. Strong throw guarantee.
     */
    mol_ref_t molecule();

    /** @brief Returns the molecular system associated with this system.
     *
     *  @return The molecular system in a read-only state.
     *  @throws std::runtime_error if the class has no PIMPL. Strong throw
     *                             gurantee.
     */
    const_mol_ref_t molecule() const;

    /** @brief Returns the number of electrons in the system.
     *
     *  @return A read/write reference to the number of electrons in the system.
     *
     *  @throw std::bad_alloc if the class has no PIMPL and there's a problem
     *                        allocating one. Strong throw guarantee.
     */
    size_type& nelectrons();

    /** @brief Returns the number of electrons in the system.
     *
     *  @return The number of electrons in the system.
     *
     *  @throw std::runtime_error if the class has no PIMPL. Strong throw
     *         guarantee.
     */
    size_type nelectrons() const;

    /** @brief Returns the electronic charge of the system.
     *
     *  The charge of the system is based off of the number of electrons and
     *  the atomic numbers of the nuclei. To change the charge of the system,
     *  change the number of electrons (or add/remove nuclei).
     *
     *  @note By fiat we define an empty system to have no charge.
     *
     *  @return -1 times the net number of electrons (relative to the neutral
     *          system).
     *
     *  @throw None no throw guarantee.
     */
    charge_type charge() const noexcept;

    /** @brief Returns the external electrostatic potential in a read/write
     *         state.
     *
     *  @return The electrostatic potential that the molecule is embedded in.
     *
     *  @throws std::bad_alloc if the class has no PIMPL and an error arise
     *                         while allocating one. Strong throw guarantee.
     *
     */
    epot_ref_t external_electrostatic_potential();

    /** @brief Returns the external electrostatic potential in a read-only
     *         state.
     *
     *  @returns The electrostatic potential that the molecule is embedded in.
     *
     *  @throws std::runtime_error if the class has no PIMPL. Strong throw
     *                             guarantee.
     */
    const_epot_ref_t external_electrostatic_potential() const;

    /** @brief Determines if two ChemicalSystem instances are the same.
     *
     *
     *  Two ChemicalSystem instances are equal if all of their members compare
     *  equal. It is worth noting that since these members contain
     * floating-point data this implies exact floating-point comparisons,
     * i.e. 1.0000 != 1.0001.
     *
     *  @param[in] rhs The ChemicalSystem on the right of the equality operator.
     *
     *  @return True if this and @p rhs are the same and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ChemicalSystem& rhs) const noexcept;

    /** @brief Computes a hash of the ChemicalSystem.
     *
     *  @param[in,out] h The object used to hash the state. After this call @p h
     *                   will have been modified to include a hash of this
     *                   object's state.
     */
    void hash(detail_::Hasher& h) const;

private:
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
