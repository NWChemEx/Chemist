#pragma once
#include "LibChemist/BasisSet.hpp"
#include "LibChemist/BasisShell.hpp"
#include <array>
#include <unordered_map>

namespace LibChemist {

/// These are the recognized properties an atom may have
enum class AtomProperty {
    Z,            ///< Atomic number/nuclear charge
    mass,         ///< Atomic mass (abundance-weighted mass)
    isotope_mass, ///< Mass of the selected isotope
    charge,       ///< Charge on the center
    multiplicity, ///< Electronic multiplicity
    nelectrons,   ///< Number of assigned electrons
    cov_radius,   ///< The covalent radius
    vdw_radius    ///< The van der waals radius
};

/*! \brief A center in a system
 *
 * An atom is basically just a collection of data associated with a point in
 * space.  Thus most of the get/set operations are trivial and we have made
 * those data members public to avoid a large number of get/set member
 * functions.  At the moment, the only non-trivial get/set is for
 * the basis set.  For the basis we have provided the add_shell/get_basis
 * member functions for setting and getting repsecitively.  These two functions
 * work asymmetrically in that add puts on a shell and get returns all shells.
 * This asymmetry mirrors actual workflows in which there was never any reason
 * to retrieve an individual shell; however, there are scenarios in which
 * individual shells are added.  Individual shells can be retrieved, if needed,
 * from the BasisSet instance.
 *
 * Another highlight of this class is that  many values which are traditionally
 * integers are instead represented as doubles, which allows for fractional
 * occupation, fractional nuclear charge, etc.
 *
 * \note The `create_atom` free functions that follow this class are the
 * intended way to create instances of this class. These free
 * functions, rely heavily on the information contained in the look up tables
 * located in the lut directory.  The reliance on such information is the
 * reason these functions are seperate as it allows us to decouple the physical
 * data from the class.
 *
 */
class Atom {
    public:
    /** \brief Makes an instance where all values are in an unitialized state.
     *
     *   \throws None No throw guarantee.
     *
     *   \threading Thread safe.
     */
    Atom() noexcept = default;

    /** \brief Deep copies another Atom instance
     *
     *  \param[in] other The Atom instance to copy
     *
     *  \throws std::bad_alloc if memory allocation fails.  Strong throw
     *  guarantee.
     *
     *  \threading All elements of \p other are accessed and data races may
     *  occur if \p other is concurrently modified.
     */
    Atom(const Atom& /*other*/) = default;

    /** \brief Takes ownership of another Atom instance
     *
     * \param[in] other The Atom instance to take ownership of
     *
     * \note After this call \p other is in a valid, but undefined state.
     *
     * \throws None No throw guarantee.
     *
     * \threading All elements of \p other are accessed and modified.
     */
    Atom(Atom&& /*other*/) noexcept = default;

    /** @brief Makes an Atom with the requested state.
     *
     * @tparam map_type An associative container (that is one filled with
     *         std::pair<AtomProperty,double> instances).  std::vector of pairs
     *         as well as std::map, and std::unorderd_map should work.
     * @param xyz The Cartesian coordinates in atomic units
     * @param props A list of pairs such that the first element of the pair is
     *        the property's tag and the second is the value.
     * @throw std::bad_alloc if there is insufficient memory for copying the
     *        properties over.  Strong throw guarantee.
     *
     */
    template<typename map_type>
    Atom(const std::array<double, 3>& xyz, const map_type& props) :
      coord(xyz),
      properties_(props.begin(), props.end()) {}

    /**
     * @brief Cleans up memory associated with this instance.
     *
     * @throw None. No throw guarantee.
     */
    ~Atom() noexcept = default;

    /** \brief Assigns a deep copy of another Atom instance to this instance
     *
     *  \param[in] rhs The Atom instance to deep copy.
     *
     *  \returns The current instance after copying \p rhs.
     *
     *  \throws std::bad_alloc if memory allocation fails.  Strong throw
     *          guarantee.
     *
     *  \threading All members of \p rhs are accessed and data races may occur
     *  if \p rhs is concurrently modified.
     */
    Atom& operator=(const Atom& /*rhs*/) = default;

    /** @brief Assigns another Atom instance's data to this instance.
     *
     *  @param[in] rhs The Atom instance to take ownership of.  After calling
     *  this function @p rhs is in a valid, but otherwise undefined state.
     *
     *  @returns The current instance after taking ownership of @p rhs.
     *
     *  @throws None No throw guarantee.
     *
     *  @threading All elements of this and @p rhs are accessed and modified.
     */
    Atom& operator=(Atom&& /*rhs*/) noexcept = default;

    /**
     * @brief Tells you whether a property is defined for the current atom
     * @param prop The property to look for.
     * @return True if the current instance has @p prop set and false otherwise.
     * @throw None. No throw guarantee.
     */
    bool count(AtomProperty prop) const noexcept {
        return properties_.count(prop);
    }

    /**
     * @brief Returns a requested property in a writable state
     * @param prop The requested property
     * @return The value of the requested property in a writable state.
     * @throws std::bad_alloc if the property doesn't exist and there is
     *     insufficient memory to allocate space for its value.  Strong throw
     *     guarantee.
     */
    double& property(AtomProperty prop) {
        if(!count(prop)) properties_[prop] = 0.0;
        return properties_[prop];
    }

    /**
     * @brief Returns a requested property in a read-only state
     * @param prop The requested property
     * @return The value of the requested property in a read-only state.
     * @throws std::out_of_range if the current atom does not have the requested
     *         property.  Strong throw guarantee.
     */
    const double& property(AtomProperty prop) const {
        return properties_.at(prop);
    }

    /** \brief Adds a basis set shell to the atom.
     *
     *  \param[in] bs_name Which basis set to add the shell to.
     *
     *  \param[in] shell The shell to add.
     *
     *  \throws std::bad_alloc if memory allocation fails.  Strong throw
     *  guarantee.
     *
     *  \threading Internal data pertaining to basis sets is modified and may
     *  cause data races if the user tries to access that information
     *  concurrently.
     *
     */
    void add_shell(const std::string& bs_name, const BasisShell& shell) {
        if(!basis_sets_.count(bs_name))
            basis_sets_[bs_name] = std::vector<BasisShell>();
        auto& bs = basis_sets_[bs_name];
        bs.push_back(shell);
    }

    /** \brief Returns the requested basis set as a deep copy.
     *
     * \param[in] bs_name The name of the basis set from which the shells will
     *                    be taken.
     *
     * \returns A BasisSet instance comprised of all shells belonging to
     *          \p bs_name.  If \p bs_name does not exist an empty BasisSet
     *          instance is returned.
     *
     * \throws std::bad_alloc if there is insufficient memory.
     *
     * \threading Generally thread safe although data races may occur if there
     * are concurrent calls to add_shell.
     */
    BasisSet get_basis(const std::string& bs_name) const {
        BasisSet rv;
        if(!basis_sets_.count(bs_name)) return rv;
        for(const auto& shell : basis_sets_.at(bs_name))
            rv.add_shell(coord.data(), shell);
        return rv;
    }

    /** \brief Returns true if this instance is exactly equal to another
     *     Atom instance.
     *
     *  \param[in] rhs The Atom instance to compare to.
     *
     *  \returns True if \p rhs is exactly equal to this instance.
     *
     *  \throws None No throw guarantee.
     *
     * \threading All members of \p rhs are accessed, concurrent modification of
     * \p rhs may result in data races.
     */
    bool operator==(const Atom& rhs) const noexcept;

    /** \brief Returns true if this instance is not exactly equal to another
     *     Atom instance.
     *
     *  \param[in] rhs The Atom instance to compare to.
     *
     *  \returns True if any member of this instance differs from the
     *    corresponding member of \p rhs.
     *
     *  \throws None No throw guarantee.
     *
     *  \threading All members of \p rhs are accessed.  Concurrent writes to
     *  \p rhs may result in data races.
     */
    bool operator!=(const Atom& rhs) const noexcept { return !(*this == rhs); }

    /// The coordinates of the Atom
    std::array<double, 3> coord;

    private:
    /// The properties of the Atom
    std::unordered_map<AtomProperty, double> properties_;

    /// A map between basis set names and its shells on this atom
    std::unordered_map<std::string, std::vector<BasisShell>> basis_sets_;
};

/** @relates Atom
 * @brief Create an atom given coordinates, and atomic number
 *
 * @param[in] xyz The coordinates where the atom is centered, in Bohr.
 * @param[in] Z The atomic number of the atom to be created
 *
 * @returns The created instance with all other data set to the default
 *  values for the neutral most common isotope.
 *
 * @throws  std::out_of_range if Z does not correspond to a known atom.  Strong
 *          throw guarantee.
 *
 * @threading Members of @p xyz are accessed and concurrent modifciation of
 * @p xyz may result in data races.
 */
Atom create_atom(const std::array<double, 3>& xyz, size_t Z);

/** @relates Atom
 * @brief Create an atom given coordinates, atomic number, and isotope number
 *
 * @param[in] xyz The coordinates where the atom is centered, in Bohr.
 * @param[in] Z The atomic number of the atom to be created
 * @param[in] isonum The isotope number of the atom to create
 *
 * @returns The created instance with all other data set to the default
 *  values for the neutral most common isotope.
 *
 * @throws  std::out_of_range if Z does not correspond to a known atom or if
 *   that atom does not contain an isotope with the requested isotope number.
 *   Strong throw guarantee.
 *
 * @threading Members of @p xyz are accessed and concurrent modification of
 * @p xyz may result in data races.
 */
Atom create_atom(const std::array<double, 3>& xyz, size_t Z, size_t isonum);

/** @relates Atom
 * @brief Makes a copy of an Atom that is a ghost atom.
 *
 * Ghost atoms are "atoms" that have no nucleus, electrons, or overall charge,
 * but have basis functions.  In other words they are a collection of basis
 * functions centered on a point.  This function will deep copy a given atom and
 * modify the copy's internal data so that it is a ghost atom.
 *
 * @param[in] atom The Atom to turn into a ghost atom.
 *
 * @returns A deep copy of @p atom that is a ghost atom.
 *
 * @throws None. No throw guarantee.
 *
 * @threading All members of @p atom are accessed and data races may result if
 * @p atom is concurrently modified.
 */
Atom create_ghost(const Atom& atom) noexcept;

/** @relates Atom
 * @brief Returns true if @p atom is a ghost atom.
 *
 * This function can be used to check if a given atom is a ghost atom.  Users
 * should not concern themselves with exactly what details make an atom a
 * ghost atom, but rather rely on this function.
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
 * @brief Makes a dummy atom.
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
