#pragma once
#include "chemist/basis_set/basis_set.hpp"
#include <string>

namespace chemist {
namespace detail_ {
/// Forward declare PIMPL
struct BasisSetManagerPIMPL;
} // namespace detail_

/**
 * @brief A class for managing the available basis sets.
 *
 * To a large extent this class is envisioned as nothing more than a glorified
 * map from string names of basis sets to the parameters. Basis sets are added
 * using the @ref insert method.
 */
class BasisSetManager {
public:
    /// The type of an atom's AO basis set
    using ao_basis_type = AtomicBasisSet<double>;

    /// The type of an atomic number
    using size_type = std::size_t;

    /// The type of a basis set, mapping atomic numbers to AO basis sets
    using ao_basis_map = std::map<size_type, ao_basis_type>;

    /**
     * @brief Makes a basis set manager which can have basis set maps inserted.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        PIMPL.  Strong throw guarantee.
     */
    BasisSetManager();

    /**
     * @name Copy/Move CTors and Assignment Operators
     * @brief Functions for replicating the state of another BasisSetManager
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
    BasisSetManager(const BasisSetManager& rhs);
    BasisSetManager(BasisSetManager&& rhs) noexcept;
    BasisSetManager& operator=(const BasisSetManager& rhs);
    BasisSetManager& operator=(BasisSetManager&& rhs) noexcept;
    ///@}

    /// Default dtor
    ~BasisSetManager() noexcept;

    /**
     * @name Getter/Setters
     */
    ///@{
    /**
     * @brief Returns the requested basis set for a particular atom.
     *
     * @param name The name of the basis set
     * @param Z The atomic number.
     *
     * @return The shells for an atom of atomic number @p Z.
     *
     * @throw std::out_of_range @p name is not a valid basis set name.
     *                          Strong throw guarantee.
     */
    ao_basis_type get_basis(const std::string& name, const size_type& Z) const;

    /**
     * @brief Adds a new basis set name and generation function.
     *
     * Adds a new basis set name and function to generate the basis set for a
     * particular element.
     *
     * @param name The name of the new basis set
     * @param basis_set The basis set mapping from atomic number to AO basis
     *
     * @throw std::runtime_error if basis set with the given name already
     *                           exists. Strong throw guarantee.
     * @throws ??? An exception is thrown in std::map::emplace. Strong throw
     *             guarantee.
     */
    void insert(const std::string& name, const ao_basis_map& basis_set);
    ///@}

    /**
     * @name Comparison Operators
     *
     * @param rhs BasisSetManager on the right-hand side of the operator
     * @return Truth of comparison operator
     */
    ///@{
    bool operator==(const BasisSetManager& rhs) const;
    bool operator!=(const BasisSetManager& rhs) const;
    ///@}

private:
    /**
     * @name PIMPL Interaction
     * @brief Methods used to interact with the PIMPL
     *
     * @return detail_::BasisSetManagerPIMPL&
     */
    ///@{
    /**
     * @brief Returns the PIMPL instance, creating it if it does not exist.
     *
     * @returns Existing PIMPL or newly created PIMPL if one did not exist yet.
     *
     * @throw std::bad_alloc There was not enough memory to create the new
     *                       PIMPL. Strong throw guarantee.
     */
    detail_::BasisSetManagerPIMPL& pimpl_();

    /**
     * @brief Returns the PIMPL instance, but throws if it does not exist.
     *
     * @return const detail_::BasisSetManagerPIMPL&
     *
     * @throw std::runtime_error A PIMPL does not exist. Strong throw
     *                           guarantee.
     */
    const detail_::BasisSetManagerPIMPL& pimpl_() const;
    ///@}

    /// The instance actually responsible for making the class run
    std::unique_ptr<detail_::BasisSetManagerPIMPL> m_pimpl_;
};

/**
 * @brief Converts the string representation of orbital angular momentum to
 *        its actual value in a.u.
 *
 *  This is a convenience function for taking a letter like: 's', 'p', 'd',
 *  or 'f' and converting it to the corresponding numerical angular momentum
 *  value (for the examples listed those values are respectively 0, 1, 2,
 *  and 3).  This function is capable of mapping the first 26 values (when
 *  going in alphabetical order after 'f', 's', 'p', and 'd' are skipped
 *  the second time they would be encountered *e.g.*, it goes 'm', 'n', 'o',
 *  'q' and not 'm', 'n', 'o', 'p', 'q'.
 *
 *
 * @param[in] l The string representation of the orbital angular momentum.
 *              It is assumed to be a single character, *i.e.*, l > 25 is not
 *              supported.
 *
 * @return The numerical value of the orbital angular momentum in atomic
 *         units.
 *
 * @throw std::logic_error if @p l is not a letter of the English alphabet.
 *                         Strong throw guarantee.
 */
std::size_t l_converter(char l);

} // namespace chemist
