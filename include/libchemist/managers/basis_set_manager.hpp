#pragma once
#include "libchemist/basis_set/basis_set.hpp"
#include <string>

namespace libchemist {
namespace detail_ {
/// Forward declare PIMPL
class BasisSetManagerPIMPL;
} // namespace detail_

/**
 * @brief A class for managing the available basis sets.
 *
 * To a large extent this class is envisioned as nothing more than a glorified
 * map from string names of basis sets to the parameters.  How those basis sets
 * are stored and/or accessed is a detail left up to the PIMPL backend.
 *
 * As it stands now, there's not a whole lot of reason to justify making this
 * a class versus a free function; however, it's reasonable to down the line
 * add the ability to say interact with this class as if it really does have
 * state.  For example, one can envision inquiring into whether or not a
 * basis set is defined for an atom, or adding additional basis sets
 * dynamically.
 */
class BasisSetManager {
public:
    /// The type of an atom's AO basis set
    using ao_basis_type = Center<double>;

    /// The type of an atomic number
    using size_type = std::size_t;

    /**
     * @brief Makes a basis set manager that relies on hard-coded basis sets.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        PIMPL.  Strong throw guarantee.
     */
    BasisSetManager();

    /**
     * @defgroup Copy/Move CTors and Assignment Operators
     * @brief Can be used to construct BasisSetManagers with state obtained
     *        from another instance.
     *
     * @param[in] rhs The instance to copy/move.  If moved @p rhs is in a valid,
     *            but otherwise undefined state.
     * @throw std::bad_alloc copy ctor/assignment operators throw if there is
     *        insufficient memory to allocate the new state.  Strong throw
     *        guarantee.  All move functions are no throw guarantee.
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
     * @brief Returns the requested basis set for a particular atom.
     *
     * @param name The name of the basis set
     * @param Z The atomic number.
     * @return The shells for an atom of atomic number @p Z.
     * @throw std::bad_alloc if there is insufficient memory to make the basis
     *        set.  Strong throw guarantee.
     */
    ao_basis_type get_basis(const std::string& name, size_type Z) const;

private:
    /// The instance actually responsible for making the class run
    std::unique_ptr<detail_::BasisSetManagerPIMPL> pimpl_;
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
 *            It is assumed to be a single character, *i.e.*, l > 25 is not
 *            supported.
 * @return The numerical value of the orbital angular momentum in atomic
 *         units.
 * @throw std::logic_error if @p l is not a letter of the English alphabet.
 *        Strong throw guarantee.
 */
std::size_t l_converter(char l);

} // namespace libchemist
