#pragma once
#include "chemist/detail_/hashing.hpp"
#include "chemist/molecule/atom.hpp"
#include <vector> //For iterators

namespace chemist {
namespace detail_ {
/// Forward declaration of class that holds the molecule's implementation
class MolPIMPL;
} // namespace detail_

class Molecule {
public:
    /// Type of an atom contained in this class
    using value_type = Atom;

    /// Type of a reference to an atom
    using reference = value_type&;

    /// Type of a const reference to an atom
    using const_reference = const value_type&;

    /// Type of an iterator over the atoms
    using iterator = typename std::vector<value_type>::iterator;

    /// Type of a const iterator over the atoms
    using const_iterator = typename std::vector<value_type>::const_iterator;

    /// Type of a number returned by this class
    using size_type = std::size_t;

    /**
     * @brief Makes a molecule with no atoms, no charge, and a multiplicity of 1
     *
     * @throw std::bad_alloc if there is insufficient memory to
     */
    Molecule();

    ///@{
    Molecule(const Molecule& rhs);
    Molecule(Molecule&& rhs) noexcept;
    Molecule& operator=(const Molecule& rhs);
    Molecule& operator=(Molecule&& rhs) noexcept;
    ///@}

    /** @brief Initializes the Molecule with the provided atoms.
     *
     *  @param[in] atoms The initial atoms to occupy this molecule.
     */
    Molecule(std::initializer_list<value_type> atoms);

    /// Default dtor
    ~Molecule() noexcept;

    /**
     * @brief Function used to add an additional atom to the molecule
     *
     * Given a molecule that contains @f$N@f$ atoms, this will make the provided
     * atom the @f$N+1@f$-th atom, which resides at index @f$N@f$.
     *
     * @param value the atom to add to the molecule.
     * @throw std::bad_alloc if there is insufficient memory to copy the state
     *        of the molecule over.  Strong throw guarantee.
     */
    void push_back(value_type value);

    /**
     * @brief Used to determine the number of atoms within the system.
     *
     * @return The number of atoms within the molecule
     * @throw None. No throw guarantee.
     */
    size_type size() const noexcept;

    /**
     * @defgroup Atom Accessors
     * @brief Functions in this group can be used to set/get an individual atom.
     *
     * @param[in] i The index of the requested atom.  Should be in the range
     *            [0,size()).
     *
     * @warning No bounds checks are performed for any function in this section.
     * @return The requested atom.
     * @throw None no throw guarantee.
     */
    ///@{
    reference at(size_type i) noexcept;

    const_reference at(size_type i) const noexcept {
        return const_cast<Molecule&>(*this).at(i);
    }

    reference operator[](size_type i) noexcept { return at(i); }

    const_reference operator[](size_type i) const noexcept { return at(i); }
    ///@}

    /**
     * @defgroup Iterators
     * @brief Functions for returning iterators that can be used to iterate
     *        over the atoms in the molecule in an STL-like manner.
     *
     * @return An iterator pointing to the first Atom (for overloads of begin)
     *         or an iterator pointing to just past the last Atom (for overloads
     *         of end).
     * @throw none No throw guarantee.
     */
    ///@{
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    ///@}

    /** @brief Serialize Molecule instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& size();
        for(const auto& x : *this) ar& x;
    }

    /** @brief Deserialize for Molecule instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) {
        size_type s;
        ar& s;
        for(int i = 0; i < s; ++i) {
            Atom a;
            ar& a;
            this->push_back(std::move(a));
        }
    }

    /** @brief Hash Atom instance
     *
     * @param h Hasher object
     */
    void hash(detail_::Hasher& h) const;

private:
    /// The object actually implementing the Molecule class
    std::unique_ptr<detail_::MolPIMPL> pimpl_;
};

/**
 * @relates Molecule
 * @brief Overloads the stream insertion operator so that molecules can be
 *        printed.
 *
 * @param os The ostream instance to write to.
 * @param mol The Molecule instance to write to the stream.
 * @return The ostream instance containing the text representation of the
 *         molecule.
 * @throws std::ios_base::failure if anything goes wrong while writing.  Weak
 *         throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const Molecule& mol);

/**
 * @defgroup Molecule comparison operators
 * @relates Molecule
 * @brief Allows one to compare two molecule instances for exact equality.
 *
 * Two molecule instances are defined as equal if they have the same charge,
 * multiplicity, and atoms (w.r.t. atomic number, atomic mass, atomic
 * coordinates, and index within the atom-holding container). *N.B* that
 * floating-point comparisons are bit-wise with zero tolerance for deviation,
 * *i.e.*, 1.99999999999999 != 2.00000000000000
 *
 * @param[in] lhs The Molecule instance on the left of the equivalence operation
 * @param[in] rhs The Molecule instance on the right of the equivalence
 * operation
 * @return Whether the two molecules obey the requested equivalence relation.
 *
 * @throw none all comparisons are no throw guarantee.
 */
///@{
bool operator==(const Molecule& lhs, const Molecule& rhs) noexcept;
inline bool operator!=(const Molecule& lhs, const Molecule& rhs) noexcept {
    return !(lhs == rhs);
}
///@}

// TODO: Change Molecule class to Nuclei and remove typedef
using Nuclei = Molecule;

} // end namespace chemist
