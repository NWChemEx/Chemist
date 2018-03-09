#pragma once
#include "LibChemist/BasisSet.hpp"
#include "LibChemist/BasisShell.hpp"
#include <array>
#include <map>
#include <string>

namespace LibChemist {

/*! \brief A center in a system
 *
 * An atom is basically just a collection of data associated with a point in
 * space.  This class assumes that there is some other routine/class that is
 * capable of instantiating it with its initial state.  When possible properties
 * are computed (e.g. the number of electrons is determined based on the
 * charge and atomic number of the instance).
 *
 * This class satisfies the concept of aggregate.
 */
struct Atom {

    /// These are the recognized properties an atom may have
    enum class Properties {
        Z,            ///< Atomic number/nuclear charge
        mass,         ///< Atomic mass (abundance-weighted mass)
        isotope_mass, ///< Mass of the selected isotope
        charge,       ///< Charge on the center
        multiplicity, ///< Electronic multiplicity
        cov_radius,   ///< The covalent radius
        vdw_radius    ///< The van der waals radius
    };

    /// The type of the Cartesian coordinates, satisfies random-access container
    using xyz_type = std::array<double, 3>;

    /// The type of an atom property descriptor, assumed to be POD-like
    using atom_property_type = Properties;

    /// The type of an atom's property, will be a floating point POD type
    using property_value = double;

    /// The type of the properties map, satisfies associative array
    using properties_map = std::map<atom_property_type, property_value>;

    /// The type of a key to a basis set, assumed to be string-like
    using basis_key = std::string;

    /// The type of the basis sets look-up table, satisfies associative array
    using basis_lut_type = std::map<basis_key, std::vector<BasisShell>>;

    ///The atom's current position in a.u.
    xyz_type coords;

    /// The look-up table of properties for the current Atom
    properties_map properties;

    /// A map between basis set names and its shells on this atom
    basis_lut_type bases;

    /**
     * @brief Returns the number of electrons associated with this atom.
     *
     * This function assumes that the number of electrons that can be associated
     * with this particular atom is given by:
     * @f[
     *  n_e = Z - q,
     * @f]
     * where @f$Z@f$ is the atomic number and @f$q@f$ is the charge of the
     * current instance.
     *
     * @return The number of electrons in the current atom.
     *
     * @throw std::out_of_range if the current instance's atomic number and
     * charge are not set.  Strong throw guarantee.
     *
     * @threading The properties of the current instance will be accessed and
     * a data-race may occur if another thread modifies the properties
     * concurrently.
     */
    property_value nelectrons() const {
        return properties.at(Properties::Z) - properties.at(Properties::charge);
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
 * @threading All operations occur on a deep copy of @p atom and data of @p
 * atom is accessed via the deep copy, hence this function is thread-safe.
 */
Atom create_ghost(Atom atom) noexcept;

/** @relates Atom
 * @brief Returns true if @p atom is a ghost atom.
 *
 * This function is a convenience function for checking if a given
 * atom is a ghost atom.
 *
 * @param[in] atom The Atom instance to evaluate for its ghost-ness
 *
 * @returns True if @p atom is a ghost atom.
 *
 * @throws None. No throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 * @p atom is concurrently modified.
 */
bool is_ghost_atom(const Atom& atom) noexcept;

/** @relates Atom
 *  @brief Makes a dummy atom.
 *
 * Dummy atoms are "atoms" that have no nucleus, electrons, overall charge,
 * or basis functions.  In other words they are points in space.
 *
 * @param[in] xyz Where in space the dummy atom is located.
 *
 * @returns A new Atom instance that is a dummy atom.
 *
 * @throws None No throw guarantee.
 *
 * @threading All members of @p xyz are accessed and data races may occur if
 * @p xyz is subsequently modified.
 */
Atom create_dummy(const std::array<double, 3>& xyz) noexcept;

/** @relates Atom
 * @brief Returns true if @p atom is a dummy atom.
 *
 * This function can be used to check if a given atom is a dummy atom.  Users
 * should not concern themselves with exactly what details make an atom a
 * dummy atom, but rather rely on this function.
 *
 * @param[in] atom The Atom instance to evaluate for its stupidity
 *
 * @returns True if @p atom is a dummy atom.
 *
 * @throws None No throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 *  @p atom is concurrently modified.
 */
bool is_dummy_atom(const Atom& atom) noexcept;

/** @relates Atom
 * @brief Makes a point charge.
 *
 * Point charges are "atoms" that have no nucleus, electrons, or basis
 * functions, but have an overall charge.
 *
 * @param[in] xyz Where in space the point charge is located.
 * @param[in] chg The charge (in A.U.) of the point charge
 *
 * @returns A new Atom instance that is a point charge.
 *
 * @throws None No throw guarantee.
 *
 * @threading @p xyz is accessed and concurrent modifications to @p xyz may
 * result in data races.
 */
Atom create_charge(const std::array<double, 3>& xyz, double chg) noexcept;

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
 * @throws None No throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 * @p atom is concurrently modified.
 */
bool is_charge(const Atom& atom) noexcept;

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
 * @throws None No throw guarantee.
 *
 * @threading Members of @p atom are accessed and data races may result if
 * @p atom is concurrently modified.
 */
bool is_real_atom(const Atom& atom) noexcept;

} // namespace LibChemist
