#pragma once
#include "LibChemist/BasisShell.hpp"
#include "LibChemist/BasisSet.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <array>
#include <map>
#include <string>

namespace LibChemist {

/*! \brief A point with some associated properties
 *
 * The majority of computations in electronic structure theory assume that
 * the nuclei are point-like and the electrons move around in the resulting
 * field.  The current class aggregates the properties one typically
 * associates with a nucleus or other point-like object (e.g. ghost atom or
 * point-charge).  In order to keep the definition of various point-like
 * objects consistent users are encouraged to use the free functions
 * create_ghost, create_charge, and create_dummy to turn existing atoms into
 * ghost, point-charges, and dummy atoms respectively.
 *
 */
struct Atom {

    /// These are the recognized properties an atom may have
    enum class Property {
        charge,       ///< charge of particle (typically same as Z)
        mass,         ///< Atomic mass (abundance-weighted mass)
        isotope_mass, ///< Mass of the selected isotope
        cov_radius,   ///< The covalent radius
        vdw_radius    ///< The van der waals radius
    };

    /// The type of the Cartesian coordinates, satisfies random-access container
    using xyz_type = std::array<double, 3>;

    /// The type of a property descriptor, assumed to be POD-like
    using property_key = Property;

    /// The type of a property's value, will be a floating point POD type
    using property_value = double;

    /// The type of the properties map, satisfies associative array
    using property_map = std::map<property_key, property_value>;

    /// The type of the container holding the shells
    using shell_container = std::vector<BasisShell>;

    /// The type of the basis sets look-up table, satisfies associative array
    using basis_lut_type = Utilities::CaseInsensitiveMap<shell_container>;

    ///The atom's current position in a.u.
    xyz_type coords = {};

    /// The look-up table of properties for the current Atom
    property_map properties;

    /// A map between basis set names and its shells on this atom
    basis_lut_type bases;

    /**
     * @brief Returns the requested basis set
     * @param[in] name The name of the basis set.
     * @return The requested basis set, or an empty basis set instance if
     * there is no basis @p name on the current atom.
     * @throw std::bad_alloc if there is insufficient memory to copy the basis
     * set.  Strong throw guarantee.
     * @threading The contents of @p name and the current class are accessed and
     * data races may result if either @p name or the current instance are
     * concurrently modified.
     */
    BasisSet get_basis(const std::string& name)const;

    /**
     *  @brief Used to determine if two Atom instances are exactly identical.
     *
     *  Exact equality between two atoms is defined as having the same
     *  coordinates, properties, and atom-centered basis sets.
     *
     *  @param[in] rhs The atom to compare to.
     *  @return True if the two instances are identical and false otherwise.
     *  @throw None. No throw guarantee.
     *  @threading The fields of both this and @p rhs are accessed so data races
     *  may occur if either the current instance or @p rhs are concurrently
     *  modified.
     *
     */
    bool operator==(const Atom& rhs)const noexcept;

    /**
     *  @brief Used to determine if two Atom instances differ.
     *
     *  Exact equality between two atoms is defined as having the same
     *  coordinates, properties, and atom-centered basis sets.
     *
     *  @param[in] rhs The atom to compare to.
     *  @return True if the two instances differ in any manner and false if
     *  they are exactly equal.
     *  @throw None. No throw guarantee.
     *  @threading The fields of both this and @p rhs are accessed so data races
     *  may occur if either the current instance or @p rhs are concurrently
     *  modified.
     *
     */
    bool operator!=(const Atom& rhs)const noexcept{
        return !((*this) == rhs);
    }

}; //End struct Atom

/** @relates Atom
 *  @brief Makes a copy of an Atom that is a ghost atom.
 *
 * Ghost atoms are "atoms" that have no nucleus, electrons, or overall charge,
 * but have basis functions.  In other words they are a collection of basis
 * functions centered on a point.  This is a convenience function for setting
 * those properties correctly.
 *
 * @param[in] atom The Atom whose deep copy will be the ghost atom.
 *
 * @returns A deep copy of @p atom that is a ghost atom.
 *
 * @throws std::bad_alloc if there is insufficient memory to copy the contents
 * of @p atom.  Strong throw guarantee.
 *
 * @threading The contents of @p atom are accessed and data races may occur if
 * @p atom is concurrently modified.
 */
Atom create_ghost(const Atom& atom);

/** @relates Atom
 * @brief Returns true if @p atom is a ghost atom.
 *
 * This function is a convenience function for checking if a given
 * atom is a ghost atom.  It works by turning @p atom into a ghost
 * atom and comparing the result to @p atom.
 *
 * @param[in] atom The Atom instance to evaluate for its ghost-ness
 *
 * @returns True if @p atom is a ghost atom.
 *
 * @throws std::bad_alloc if there is insufficient memory to form the ghost
 * version of @p atom to compare against.  Strong throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 * @p atom is concurrently modified.
 */
bool is_ghost_atom(const Atom& atom);

/** @relates Atom
 *  @brief Makes a dummy atom.
 *
 * Dummy atoms are "atoms" that have no nucleus, electrons, overall charge,
 * or basis functions.  In other words they are points in space.
 *
 * @param[in] atom An atom instance whose deep copy will be a dummy atom.
 *
 * @returns A new Atom instance that is a dummy atom.
 *
 * @throws std::bad_alloc if there is insufficient memory to allocate the
 * copy.  Strong throw guarantee.
 *
 * @threading All members of @p atom are accessed and data races may occur if
 * @p atom is subsequently modified.
 */
Atom create_dummy(const Atom& atom);

/** @relates Atom
 * @brief Returns true if @p atom is a dummy atom.
 *
 * This function can be used to check if a given atom is a dummy atom.  Users
 * should not concern themselves with exactly what details make an atom a
 * dummy atom, but rather rely on this function.  The check is performed by
 * creating the dummy version of @p atom and comparing it to @p atom.
 *
 * @param[in] atom The Atom instance to evaluate for its stupidity
 *
 * @returns True if @p atom is a dummy atom.
 *
 * @throws std::bad_alloc if there is insufficient memory to allocate the dummy
 * instance.  Strong throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 *  @p atom is concurrently modified.
 */
bool is_dummy_atom(const Atom& atom);

/** @relates Atom
 * @brief Makes a point charge.
 *
 * Point charges are "atoms" that have no mass, electrons, or basis
 * functions, but have an overall charge.
 *
 * @param[in] atom The Atom instance whose deep copy will become a point charge.
 * @param[in] chg The charge (in A.U.) of the point charge
 *
 * @returns A new Atom instance that is a point charge.
 *
 * @throws std::bad_alloc if there is insufficient memory to allocate the
 * copy. Strong throw guarantee.
 *
 * @threading @p atom is accessed and concurrent modifications to @p atom may
 * result in data races.
 */
Atom create_charge(const Atom& atom, double chg);

/** @relates Atom
 * @brief Returns true if @p atom is a point charge.
 *
 * This function can be used to check if a given atom is a point charge.  Users
 * should not concern themselves with exactly what details make an atom a
 * point charge, but rather rely on this function.
 *
 * @param[in] atom The Atom instance to evaluate for its point charged-ness
 *
 * @returns True if @p atom is a point charge.
 *
 * @throws std::bad_alloc if there is insufficient memory to allocate the charge
 * to compare against.  Strong throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 * @p atom is concurrently modified.
 */
bool is_charge(const Atom& atom);

/** @relates Atom
 * @brief Returns true if @p atom is a "real" atom.
 *
 * This function can be used to determine if an Atom instance is a "real" atom.
 * At the moment an Atom instance is a "real" atom if it is not a ghost atom,
 * dummy atom, or point charge.
 *
 * @param[in] atom The atom for whose realness is in question
 *
 * @returns True if @p atom is a "real" atom.
 *
 * @throws std::bad_alloc if there is insufficient memory to copy @p atom.
 * Strong throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 * @p atom is concurrently modified.
 */
bool is_real_atom(const Atom& atom);

} // namespace LibChemist
