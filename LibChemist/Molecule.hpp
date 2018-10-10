#pragma once
#include "LibChemist/Atom.hpp"
#include <bphash/Hasher_fwd.hpp>
#include <vector> //For iterators

namespace LibChemist {
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

    /// Wrapper for tagging an input double as a charge
    struct Charge {
        double value = 0.0;
    };

    /// Wrapper for tagging an input size_type as a multiplicity
    struct Multiplicity {
        size_type value = 1ul;
    };

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

    /**
     * @defgroup State CTors
     * @brief The CTors in this section can be used to create a Molecule
     * initialized with a particular state.
     *
     * The Ctors in this section use look for a particular capture group.  The
     * available capture groups are:
     *
     * - [Multiplicity] denotes that the wrapped value should be regarded as the
     *   multiplicity of the molecule
     * - [Charge] denotes that the wrapped value should be utilized as the
     *   charge of the molecule
     * - [Atom] provides an atom for the molecule
     *
     * With the exception of Atom instances, a user may not supply more than one
     * of any other instance; attempting to provide multiple charges or
     * multiplicities will result in a compile error.  The order of the capture
     * groups is irrelevant.
     *
     * @param[in] mult The value to set the multiplicity of the molecule to.
     * @param[in] c The value to set the charge of the molecule to.
     * @param[in] a One of the atoms in the molecule.
     * @param[in] args The remaining unparsed arguments
     * @tparam Args The types of the remaining unparsed arguments.
     *
     * @throw std::bad_alloc if there is insufficient memory to add an Atom to
     *        the molecule.  Strong throw guarantee.
     *
     */
    ///@{
    template<typename... Args>
    Molecule(const Multiplicity& mult, Args&&... args) :
      Molecule(std::forward<Args>(args)...) {
        constexpr bool is_mult =
          std::disjunction_v<std::is_same<std::decay_t<Args>, Multiplicity>...>;
        static_assert(!is_mult, "Please only pass one multiplicity");
        multiplicity() = mult.value;
    }

    template<typename... Args>
    Molecule(const Charge& c, Args&&... args) :
      Molecule(std::forward<Args>(args)...) {
        constexpr bool is_q =
          std::disjunction_v<std::is_same<std::decay_t<Args>, Charge>...>;
        static_assert(!is_q, "Please only pass one charge");
        charge() = c.value;
    }

    template<typename... Args>
    Molecule(const Atom& a, Args&&... args) :
      Molecule(std::forward<Args>(args)..., ColoredAtom{a}) {}
    ///@}

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
     * @brief Used to determine the number of electrons/alpha orbitals/beta
     * orbitals within the molecule.
     *
     * @return The value of the number of electrons/alpha orbitals/beta orbitals
     * within the molecule.
     *
     * @throw None. No throw guarantee.
     */
    ///@{
    size_type nelectrons() const noexcept;
    size_type nalpha() const noexcept;
    size_type nbeta() const noexcept;

    /**
     * @defgroup Property Accessors
     * @brief Functions for getting/setting properties of the molecule.
     *
     * @return The requested property.
     * @throw none no throw guarantee.
     */
    ///@{
    double& charge() noexcept;
    const double& charge() const noexcept {
        return const_cast<Molecule&>(*this).charge();
    }
    size_type& multiplicity() noexcept;
    const size_type& multiplicity() const noexcept {
        return const_cast<Molecule&>(*this).multiplicity();
    }
    size_type nelectrons() const noexcept {
        size_type n = 0;
        for(const auto& x : *this) n += x.Z();
        return n - charge();
    }

    ///@}

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
private:
    BPHASH_DECLARE_HASHING_FRIENDS
    void hash(bphash::Hasher& h) const;

    /// Struct for coloring an atom as seen
    struct ColoredAtom {
        Atom value;
    };

    /// Catches the scenario where a colored atom has made it back to the front
    template<typename... Args>
    Molecule(const ColoredAtom& a, Args&&... args) :
      Molecule({a.value, args.value...}) {}

    /// End-point for state ctor
    Molecule(std::initializer_list<Atom> atoms);

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

} // end namespace LibChemist
