#pragma once

#include "chemist/managers/periodic_table.hpp"
#include "chemist/molecule/molecule.hpp"

namespace chemist {
namespace detail_ {
/// Forward declare PIMPL
struct MoleculeManagerPIMPL;
} // namespace detail_

/**
 * @brief A class for storing a list of pre-built molecules.
 *
 * This class is largely intended to serve as a glorified map between
 * molecule names and geometries. Molecules must be added to the manager using
 * the MoleculeManager::insert method, since the loaded molecules are no longer
 * controlled by changing out the PIMPL.
 *
 * As it currently stands there is little to justify making this a class, but
 * conceptually one can envision interacting with this class as a sort of
 * search engine for geometries, *e.g.*, give me all molecules marked as part
 * of the G2 test set, or molecules that don't contain carbon.
 */
class MoleculeManager {
public:
    /// Type of a key
    using key_type = std::string;

    /// Type of the value
    using value_type = Molecule;

    /**
     * @brief Initializes the instance with no molecules
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *                       PIMPL. Strong throw guarantee.
     */
    MoleculeManager();

    /**
     * @name Copy/Move Ctors and Assignment Operators
     * @brief Functions for replicating the state of another MoleculeManager
     *        instance.
     *
     * Copy operations are deep copies and do not alias.
     *
     * @param[in] rhs The instance to copy/move from. If moved, @p rhs will be
     *                in a valid, but otherwise undefined state after the move.
     *
     * @return Copy/Move assignment operators return the current instance, but
     *         with its new state.
     *
     * @throw std::bad_alloc Copy ctor/assignment operators throw if there is
     *                       insufficient memory to allocate the new state.
     *                       Strong throw guarantee.
     * @throw None All move functions are no throw guarantee.
     */
    ///@{
    MoleculeManager(const MoleculeManager& rhs);
    MoleculeManager(MoleculeManager&& rhs) noexcept;
    MoleculeManager& operator=(const MoleculeManager& rhs);
    MoleculeManager& operator=(MoleculeManager&& rhs) noexcept;
    ///@}

    /// Default dtor
    ~MoleculeManager() noexcept;

    /**
     * @name Getter/Setters
     */
    ///@{
    /**
     * @brief Returns the requested molecule
     *
     * This member function can be used to retrieve molecules from the
     * database.
     *
     * @param[in] name The name of the molecule one wants
     *
     * @return A Molecule instance containing the requested molecule
     *
     * @throw std::bad_alloc if there is insufficient memory to create the
     *                       return value. Strong throw guarantee.
     * @throw std::out_of_range if there is no molecule within this
     *                          instance under the @p name. Strong throw
     *                          guarantee.
     */
    value_type at(const key_type& name) const;

    /**
     * @brief Inserts a new molecule into the molecule manager
     *
     * @param[in] name The name of the molecule being inserted
     * @param[in] molecule The molecule object being inserted
     *
     * @throw std::runtime_error if molecule with the given name already
     *                           exists. Strong throw guarantee.
     * @throw ??? An exception is thrown in std::map::emplace. Strong throw
     *             guarantee
     */
    void insert(const key_type& name, const value_type& molecule);
    ///@}

    /**
     * @name Comparison Operators
     *
     * @param[in] rhs MoleculeManager on the right-hand side of the operator
     * @return Truth of comparison operator
     */
    ///@{
    bool operator==(const MoleculeManager& rhs) const;
    bool operator!=(const MoleculeManager& rhs) const;
    ///@}

private:
    /**
     * @name PIMPL Interaction
     * @brief Methods used to interact with the PIMPL
     *
     * @return detail_::MoleculeManagerPIMPL&
     */
    ///@{
    /**
     * @brief Returns the PIMPL instance, creating it if it does not exist.
     *
     * @return Existing PIMPL or newly created PIMPL if one did not exist yet.
     *
     * @throw std::bad_alloc There was not enough memory to create the new
     *                        PIMPL. Strong throw guarantee.
     */
    detail_::MoleculeManagerPIMPL& pimpl_();

    /**
     * @brief Returns the PIMPL instance, but throws if it does not exist.
     *
     * @return const detail_::MoleculeManagerPIMPL&
     *
     * @throw std::runtime_error A PIMPL does not exist. Strong throw
     *                            guarantee.
     */
    const detail_::MoleculeManagerPIMPL& pimpl_() const;
    ///@}

    /// The object managing the state of the class
    std::unique_ptr<detail_::MoleculeManagerPIMPL> m_pimpl_;
};

} // namespace chemist
