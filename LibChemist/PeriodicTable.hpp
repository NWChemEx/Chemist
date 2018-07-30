#pragma once
#include "LibChemist/Atom.hpp"

namespace LibChemist {
namespace detail_ {
class PeriodicTablePIMPL;
}

/**
 * @brief A class serving as a factory for Atom instances
 *
 * The main purpose of the PeriodicTable class is to facilitate the creation of
 * new Atom instances, which are populated with the default data for that atom.
 */
class PeriodicTable {
public:
    /// The type of a counting number
    using size_type = std::size_t;

    /**
     * @brief Returns the maximum atomic number for which an Atom instance can
     *        be made.
     *
     * For the most part creation of an Atom instance amounts to specifying the
     * mass and atomic symbol for the atom.  It is assumed that the
     * implementation amounts to a look up table, hence this data is hard
     * coded up to some value, which should be the return of this function.  It
     * should be noted that properties may not be available for all atomic
     * numbers in the range [1, max_Z()).
     *
     * @return The largest atomic number for which an Atom instance can be
     * made.
     *
     * @throw None no throw guarantee.
     */
    constexpr size_type max_Z() const noexcept;

    /**
     * @defgroup Atom Creation Functions
     * @brief Returns an Atom instance pre-loaded with the data for the
     *        specified atomic number.
     *
     * In chemistry we are used to thinking of an atom as being uniquely
     * specified simply by the atomic number.  In truth, the atomic number
     * specifies everything but the mass.  Unfortunately there are two masses
     * one can reasonably use for an atom: the abundance weighted mass and the
     * isotope mass.  We provide two different functions to distinguish between
     * these use cases.  `get_atom` returns the abundance weighted variant,
     * whereas `get_isotope` returns an Atom whose mass is for a particular
     * isotope.
     *
     * @param Z The atomic number of the atom to create.  Should be in the range
     *        [0, max_Z()).
     * @param mass_num The mass number of the isotope to create.
     * @return The requested atom.
     * @throw std::bad_alloc if there is insufficient memory to make the Atom
     *        instance.  Strong throw guarantee.
     */
    ///@{
    Atom get_atom(size_type Z) const;
    Atom get_isotope(size_type Z, size_type mass_num) const;
    ///@}
private:
    /// The instance actually implementing the class.
    std::unique_ptr<detail_::PeriodicTablePIMPL> pimpl_;
};

} // namespace LibChemist
