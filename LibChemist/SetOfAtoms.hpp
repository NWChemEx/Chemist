#pragma once
#include "LibChemist/Atom.hpp"
#include "LibChemist/BasisSet.hpp"
#include <algorithm>
#include <map>
#include <vector>

namespace LibChemist {

/** \brief This is the main class for storing the atoms in our system.
 *
 * \note This is the class most chemists would call Molecule, but apparently
 * calling something a molecule is a sore spot for other disciplines...
 *
 * \note This class is basically an ordered set.  Unfortunately the STL doesn't
 * implement such a class.  If an ordered set class is ever made (either by the
 * STL or this project) it makes sense to typedef ordered_set<Atoms> to
 * SetOfAtoms.
 *
 * \note By making Atom hashable and storing an unordered_map<Atom,size_t> where
 * size_t is the index assigned to Atom this class could be sped up if need be.
 *
 * This class is bare bones in the sense that this class does not know anything
 * about bonds, angles, etc. It is basically a typedef of std::set<Atom> with
 * some syntactic sugar related to combining instances.  One can build these
 * higher-level constructs using the public API of the class.
 *
 * Design details:
 *
 * When designing this class we were guided by the following:
 *
 * 1. Most codes will use this class is to obtain the basis set, charge,
 *    multiplicity, and nothing else.
 * 2. The remaining codes will need to build molecules up by adding/subtracting
 *    atoms.
 * 3. It does not make sense for SetOfAtoms to contain two copies of exactly the
 *    same atom (note same is not just the same place in space, but same basis,
 *    same mass, etc.)
 *
 * Implementation details:
 *
 *
 * Despite appearing to the outside world as if the atoms are stored in a set,
 * they are really stored in an std::vector.  This is because it
 * is my experience that users feel they are entitled to refer to atoms in the
 * order they provided, i.e. if an atom is the second atom in the input file it
 * should forever be obtainable by requesting atom 1 (the first being atom 0).
 * std::set would reorder the atoms.  A secondary reason is that we need an
 * order to our atoms, it is not sufficient to use a non-random iterator in all
 * cases (mainly when mapping atoms to elements of tensors).  Rather than having
 * to call std::advance to get the i-th element of the std::set we can simply
 * use std::vector's index operator.
 */
class SetOfAtoms {
    protected:
    /// The class that actually handles the Atom lookup semantics
    std::vector<Atom> atoms_;

    public:
    using iterator       = std::vector<Atom>::iterator;
    using const_iterator = std::vector<Atom>::const_iterator;

    double charge =
        0.0; ///< The charge of this collection of atoms in atomic units
    double multiplicity = 1.0; ///< The multiplicity of this collection of atoms

    /** \brief Default initializes a SetOfAtoms to an empty set.
     *
     * The resulting SetOfAtoms instance will have a charge of 0 and a
     * multiplicity of 1.
     *
     * \throws No throw guarantee.
     */
    SetOfAtoms() noexcept = default;

    /** \brief Destructor
     *
     *  \throws No throw guarantee.
     */
    ~SetOfAtoms() noexcept = default;

    /** \brief Deep copies a SetOfAtoms instance.
     *
     *  \param[in] other The SetOfAtoms instance to copy.
     *
     * \throws std::bad_alloc if memory allocation fails.  Strong throw
     * guarantee.
     *
     */
    SetOfAtoms(const SetOfAtoms& other) = default;

    /** \brief Takes ownership of another SetOfAtoms instance
     *
     * \param[in] other The instance to take ownership of.
     * \throws No throw guarantee.
     */
    SetOfAtoms(SetOfAtoms&& other) noexcept = default;

    /** \brief Assigns a deep copy of another SetOfAtoms instance to this.
     *
     *  \param[in] rhs The instance to copy.
     *  \returns The current instance containing a copy of \p rhs.
     *  \throws std::bad_alloc if memory allocation fails.  Strong throw
     *  guarantee.
     *
     */
    SetOfAtoms& operator=(const SetOfAtoms& rhs) = default;

    /** \brief Takes ownership of another instance via move assignment.
     *
     * \returns The current instance containing the data of \p rhs.
     * \throws No throw guarantee.
     */
    SetOfAtoms& operator=(SetOfAtoms&& rhs) noexcept = default;

    /** \brief Returns the number of times \p atom is in the current instance.
     *
     * \note This class is set-like so an atom may only be present at most once.
     *
     * \param[in] atom The Atom instance to look for.
     *
     * \returns Returns true (i.e. 1) if there is an Atom instance exactly
     *          matching \p atom and false (i.e. 0) otherwise.
     *
     * \throw No throw guarantee.
     */
    bool count(const Atom& atom) const noexcept
    {
        return std::find(atoms_.begin(), atoms_.end(), atom) != atoms_.end();
    }

    /** \brief Returns the number of Atoms in this instance.
     *
     * \returns The number of atoms.
     * \throws No throw guarantee.
     */
    size_t size() const noexcept { return atoms_.size(); }

    /** \brief Inserts an atom into the current instance.
     *
     * \note \p atom is only inserted if it is not already present in the
     *       current instance.
     * \returns The current instance with \p atom inserted as the
     *          `this->size()`-th atom if it is not already present.
     * \throws std::bad_alloc if memory allocation fails
     */
    SetOfAtoms& insert(const Atom& atom)
    {
        if(!count(atom))
            atoms_.push_back(atom);
        return *this;
    }

    /** \brief Returns an atom for reading.
     *
     * \param[in] i The index of the requested atom.  \p i assumed in the range
     *              [0,size()).
     * \returns The requrested atom.
     * \throws No throw guarantee.
     */
    const Atom& operator[](size_t i) const noexcept { return atoms_[i]; }

    /** \brief Returns the requested atom fro reading/writing
     *
     * \param[in] i The index of the requested atom. \p i assumed in the
     *              range [0,size()).
     * \returns The requested atom.
     * \throws No throw guarantee.
     */
    Atom& operator[](size_t i) noexcept { return atoms_[i]; }

    /** \brief Returns true if this instance is exactly equal to other.
     *
     *  Two SetOfAtom instances are equal if they contain exactly the same
     *  Atom instances in exactly the same order.
     *
     *  \param[in] rhs The SetOfAtoms instance to compare to.
     *  \returns True if this instance is exactly equal to \p rhs.
     *  \throws No throw guarantee.
     *
     */
    bool operator==(const SetOfAtoms& rhs) const noexcept
    {
        return std::tie(charge, multiplicity, atoms_)
               == std::tie(rhs.charge, rhs.multiplicity, rhs.atoms_);
    }

    /** \brief Returns true if any element of this differs from that of another
     *  instance.
     *
     *  \param[in] rhs The SetOfAtoms instance to comapre to.
     *  \returns True if any element in this instance differs from its
     *  corresponding element in \p rhs.
     *  \throws No throw guarantee.
     */
    bool operator!=(const SetOfAtoms& rhs) const noexcept
    {
        return !((*this) == rhs);
    }

    iterator begin() noexcept { return atoms_.begin(); }

    const_iterator begin() const noexcept { return atoms_.begin(); }

    iterator end() noexcept { return atoms_.end(); }

    const_iterator end() const noexcept { return atoms_.end(); }
};

/** \relates SetOfAtoms
 *
 * \brief Applies a basis set file to each atom in a SetOfAtoms.
 *
 * \param[in] name The key under which to apply the basis set.
 * \param[in] bs The basis set to apply.  Input is expected to be a map from
 *               atomic number to a vector of shells atoms with that Z get.
 * \param[in] atoms The instance to apply the basis set to.
 * \returns A deep copy of \p atoms with \p bs applied to it.
 * \throws std::bad_alloc if memory allocation fails.  Strong throw guarantee.
 *
 */
SetOfAtoms apply_basis_set(const std::string& name,
                           const std::map<size_t, std::vector<BasisShell>>& bs,
                           const SetOfAtoms& atoms);

/** \relates SetOfAtoms
 *  \brief Pulls the basis set off of a SetOfAtoms instances and places it
 *  in a more integrals-friendly container.  General contractions will be
 *  expanded out (i.e. exponents will be copied for each shell.
 *
 * \note This function will un-generalize the basis set.  If you want the
 *       basis set to remain general, use get_general_basis instead.
 *
 * \param[in] name The basis set key to get.
 * \param[in] atoms The SetOfAtoms instance to obtain the basis set from.
 *
 * \returns A deep copy of the requested basis set on the atoms.
 *
 */
BasisSet get_basis(const std::string& name, const SetOfAtoms& atoms);

/** \relates SetOfAtoms
 *  \brief Pulls the basis set off of a SetOfAtoms instances and places it
 *  in a more integrals-friendly container.  The resulting basis set still
 *  contains general contractions (if it had any).
 *
 * \note This function will un-generalize the basis set.  If you want the
 *       basis set to remain general, use get_general_basis instead.
 *
 * \param[in] name The basis set key to get.
 * \param[in] atoms The SetOfAtoms instance to obtain the basis set from.
 *
 * \returns A deep copy of the requested basis set on the atoms.
 *
 */
BasisSet get_general_basis(const std::string& name, const SetOfAtoms& atoms);

} // namespace LibChemist
