#pragma once
#include "chemist/chemical_system/chemical_system.hpp"

namespace chemist::detail_ {

class ChemicalSystemPIMPL {
public:
    /// The type of the class this PIMPL is implementing.
    using chem_sys_t = ChemicalSystem;

    /// The type of a pointer containing a PIMPL
    using pimpl_ptr_t = typename chem_sys_t::pimpl_ptr_t;

    /// The type of the object modeling the nuclear framework
    using molecule_t = typename chem_sys_t::molecule_t;

    /// A read/write reference to the object describing the set of atoms
    using mol_ref_t = typename chem_sys_t::mol_ref_t;

    /// A read-only reference to the object describing the set of atoms
    using const_mol_ref_t = typename chem_sys_t::const_mol_ref_t;

    /// Type used for the system's overall charge
    using size_type = typename chem_sys_t::size_type;

    /// The type used to model the external electrostatic potential
    using epot_t = typename chem_sys_t::epot_t;

    /// A read/write reference to the external electrostatic potential
    using epot_ref_t = typename chem_sys_t::epot_ref_t;

    /// A read-only reference to the electrostatic potential
    using const_epot_ref_t = typename chem_sys_t::const_epot_ref_t;

    /** @brief Makes a ChemicalSystemPIMPL with an empty molecule, no electrons,
     *         and no external potentials.
     *
     *  @throw std::bad_alloc if the default ctors of the molecular or external
     *                        field throw. Strong throw guarantee.
     */
    ChemicalSystemPIMPL() = default;

    /** @brief Creates a PIMPL with the provided state.
     *
     *  @param[in] mol The nuclear framework in the system.
     *  @param[in] nelectrons The number of electrons
     *  @param[in] epot The external electrostatic potential
     *
     *  @throw None No throw guarantee.
     */
    explicit ChemicalSystemPIMPL(molecule_t mol, size_type nelectrons,
                                 epot_t epot = {}) noexcept;

    /// Standard defaulted polymorphic dtor
    virtual ~ChemicalSystemPIMPL() noexcept = default;

    /** @brief Deleted move ctor.
     *
     *  The move ctor is deleted to avoid slicing the PIMPL's class hierarchy.
     *  Moving of the actual ChemicalSystem class is allowed and works by moving
     *  from the underlying unique_ptr, which has no chance of slicing.
     */
    ChemicalSystemPIMPL(ChemicalSystemPIMPL&&) = delete;

    /** @brief Deleted copy assignment.
     *
     *  Copy asssignment of the ChemicalSystem is done via the clone function
     *  of the PIMPL and swapping. There is no need to define it for the PIMPL.
     */
    ChemicalSystemPIMPL& operator=(const ChemicalSystemPIMPL&) = delete;

    /** @brief Deleted move assignment
     *
     *  Move assignment of the ChemicalSystem is done by move assigning the
     *  underlying pointer and thus does not need to be handled by the PIMPL.
     */
    ChemicalSystemPIMPL& operator=(ChemicalSystemPIMPL&&) = delete;

    /** @brief Polymorphic copy.
     *
     *  This function makes a deep, polymorphic copy of the PIMPL.
     *
     *  @throws std::bad_alloc if there is insufficient memory to make the copy.
     *                         Strong throw guarantee.
     */
    pimpl_ptr_t clone() const { return clone_(); }

    /** @brief Accessor for the contained Molecule instance.
     *
     *  @return The wrapped Molecule instance in a modifiable state.
     *
     *  @throw None No throw guarantee.
     */
    auto& molecule() noexcept { return m_mol_; }

    /** @brief Accessor for the contained Molecule instance.
     *
     *  @return The wrapped Molecule instance in a read-only state
     *
     *  @throw None No throw guarantee.
     */
    const auto& molecule() const noexcept { return m_mol_; }

    /** @brief The number of electrons.
     *
     *  @return A read/write reference to the number of electrons.
     *
     *  @throw None No throw guarantee.
     */
    size_type& nelectrons() noexcept { return m_nelectrons_; }

    /** @brief The number of electrons.
     *
     *  @return The number of elctrons.
     *
     *  @throw None No throw guarantee.
     */
    size_type nelectrons() const noexcept { return m_nelectrons_; }

    /** @brief Accessor for the contained electrostatic potential.
     *
     *  @return The held electrostatic potential in a modifiable state.
     *
     *  @throw None No throw guarantee.
     */
    auto& external_electrostatic_potential() { return m_epot_; }

    /** @brief Accessor for the contained electrostatic potential.
     *
     *  @return The held electrostatic potential in a read-only state.
     *
     *  @throw None No throw guarantee.
     */
    const auto& external_electrostatic_potential() const { return m_epot_; }

    /** @brief Polymorphically compares two ChemicalSystemPIMPLs.
     *
     *  Two ChemicalSystemPIMPLs are equal if their respective molecule
     *  instances compare equal and their respective external potentials compare
     *  equal.
     *
     *  @param[in] rhs The PIMPL to compare to.
     *
     *  @return True if this PIMPL is the same as @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ChemicalSystemPIMPL& rhs) const noexcept;

    /** @brief Serializes the ChemicalSystemPIMPL.
     *
     *  @param[in,out] ar The archive instance being used for serialization.
     *                    After this call @p ar will contain this instance's
     *                    serialized state.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& m_mol_& m_nelectrons_& m_epot_;
    }

    /** @brief Deserializes the ChemicalSystemPIMPL
     *
     *  @param[in,out] ar The archive instance which contains the serialized
     *                    state for this instance. After this call @p ar will
     *                    no longer contain this instance's serialized state.
     */
    template<typename Archive>
    void load(Archive& ar) {
        ar& m_mol_& m_nelectrons_& m_epot_;
    }

    /** @brief Computes a hash of the ChemicalSystemPIMPL.
     *
     *  @param[in,out] h The object used to hash the state. After this call @p h
     *                   will have been modified to include a hash of this
     *                   object's state.
     */
    void hash(Hasher& h) const;

protected:
    /// Implements polymorphic copy, should be overriden by derived classes
    virtual pimpl_ptr_t clone_() const;

    /// Implements the polymorphic equality comparison
    virtual bool are_equal_(const ChemicalSystemPIMPL& rhs) const noexcept;

    /// Copy ctor, protected so it can be used by clone, but not by users
    ChemicalSystemPIMPL(const ChemicalSystemPIMPL&) = default;

private:
    /// The nuclear framework
    molecule_t m_mol_;

    /// The number of electrons
    size_type m_nelectrons_ = 0;

    /// The electrostatic potential external to the molecular framework
    epot_t m_epot_;
};

// ------------------- Out-of-line inline definitions --------------------------

inline ChemicalSystemPIMPL::ChemicalSystemPIMPL(molecule_t mol,
                                                size_type nelectrons,
                                                epot_t epot) noexcept :
  m_mol_(std::move(mol)), m_nelectrons_(nelectrons), m_epot_(std::move(epot)) {}

inline bool ChemicalSystemPIMPL::operator==(
  const ChemicalSystemPIMPL& rhs) const noexcept {
    return are_equal_(rhs);
}

inline void ChemicalSystemPIMPL::hash(Hasher& h) const {
    h(m_mol_, m_nelectrons_, m_epot_);
}

inline typename ChemicalSystemPIMPL::pimpl_ptr_t ChemicalSystemPIMPL::clone_()
  const {
    auto pnew = new ChemicalSystemPIMPL(*this);
    return std::unique_ptr<ChemicalSystemPIMPL>(pnew);
}

inline bool ChemicalSystemPIMPL::are_equal_(
  const ChemicalSystemPIMPL& rhs) const noexcept {
    return std::tie(m_mol_, m_nelectrons_, m_epot_) ==
           std::tie(rhs.m_mol_, rhs.m_nelectrons_, rhs.m_epot_);
}

} // namespace chemist::detail_
