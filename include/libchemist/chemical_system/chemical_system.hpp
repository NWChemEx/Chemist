#pragma once
#include "libchemist/fields/electric_field.hpp"
#include "libchemist/molecule/molecule.hpp"
#include <memory>

namespace libchemist {
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

    /// The type of the object describing the set of atoms
    using molecule_t = Molecule;

    /// A read/write reference to the object describing the set of atoms
    using mol_ref_t = molecule_t&;

    /// A read-only reference to the object describing the set of atoms
    using const_mol_ref_t = const molecule_t&;

    /// The type used to model the electric field
    using electric_field_t = field::Electric;

    /// A read/write reference to the electric field
    using efield_ref_t = electric_field_t&;

    /// A read-only reference to the electric field
    using const_efield_ref_t = const electric_field_t&;

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

    /** @brief Creates a new ChemicalSystem with the provided molecule instance.
     *
     *  @throw std::bad_alloc if there is error while allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    explicit ChemicalSystem(molecule_t mol);

    /// Default destructor, voids all references to member data
    ~ChemicalSystem() noexcept;

    ChemicalSystem& operator=(const ChemicalSystem& rhs);

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

    efield_ref_t efield();

    const_efield_ref_t efield() const;

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
    std::unique_ptr<pimpl_t> m_pimpl_;
};

} // namespace libchemist