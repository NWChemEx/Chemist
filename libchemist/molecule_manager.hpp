#pragma once

#include "libchemist/molecule.hpp"

namespace libchemist {
namespace detail_ {
/// Forward declare PIMPL
class MoleculeManagerPIMPL;
} // namespace detail_

/**
 * @brief A class for storing a list of pre-built molecules.
 *
 * This class is largely intended to serve as a glorified map between
 * molecule names and geometries.  How the molecules are obtained can be
 * controlled by changing out the PIMPL.  The default PIMPL simply contains a
 * hard-coded list of molecules.
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
     * @brief Initializes the instance with a pre-defined set of molecules
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        PIMPL.  Strong throw guarantee.
     */
    MoleculeManager();

    /**
     * @defgroup Copy/Move Ctors and Assignment Operators
     * @brief Functions for replicating the state of another MoleculeManager
     *        instance.
     *
     * Copy operations are deep copies and do not alias.
     *
     * @param[in] rhs The instance to copy/move from.  If moved from, @p rhs
     * will be in a valid, but otherwise undefined state after the move.
     * @return Copy/Move assignment operators return the current instance, but
     *         with its new state.
     * @throw std::bad_alloc if there is insufficient memory to copy the
     *        state of @p rhs.  Strong throw guarantee.  All move operations
     *        are no throw guarantee.
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
     * @brief Returns the requested molecule
     *
     * This member function can be used to retrieve molecules from the
     * database.
     *
     * @param name The name of the molecule one wants
     * @return A Molecule instance containing the requested molecule
     * @throw std::bad_alloc if there is insufficient memory to create the
     * return value.  Strong throw guarantee.
     * @throw std::out_of_range if there is no molecule within this
     *        instance under the @p name.  Strong throw guarantee.
     *
     */
    value_type at(const key_type& name) const;

private:
    /// The object actually implementing the class
    std::unique_ptr<detail_::MoleculeManagerPIMPL> pimpl_;
};

} // namespace libchemist
