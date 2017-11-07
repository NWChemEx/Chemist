#pragma once
#include "LibChemist/BasisShell.hpp"

namespace LibChemist {

/** \brief A class for consolidating all of the basis set information about a
 *  SetOfAtoms instance.
 *
 *  \note In designing this class I am somewhat torn between making the member
 *  data private and having it be public.  Filling it in and accessing it is a
 *  somewhat non-trivial operation suggesting it should be private whereas the
 *  number of accessors required to expose the data causes me to lean towards
 *  public.  For the moment I went with public exposure as most people will not
 *  be directly building a BasisSet, but rather getting it from a SetOfAtoms; in
 *  this case they will be more interested in accessing the data.
 *
 *
 */
struct BasisSet {
    /** \brief Where the shells are centered.
     *
     *  This is an nshells by 3 array in row-major form.  Thus element i of this
     *  array is actually shell (i-i%3)/3 component i%3, components running x,
     *  y, and then z.
     */
    std::vector<double> centers;

    /** \brief The number of general contractions in each shell.
     *
     *   This array is such that ngens[i] is the number of general contractions
     *   in shell i, i in the range [0,nshells).
     */
    std::vector<size_t> ngens;

    /** \brief The number of primitives in each shell.
     *
     *  This array is such that nprims[i] is the number of primitives in shell
     *  i, i in the range [0,nshells).
     */
    std::vector<size_t> nprims;

    /** \brief The actual expansion coefficients
     *
     *  This is a jagged rank 3 array where the first dimension is over shells,
     *  the second is over the number of general contractions in that shell,
     *  and the third is over the number of primitives in that shell.  There is
     *  no simple mapping between the index of this array and the owner of the
     *  coefficient.  The best we can say is that coefs[offset+j*nprims[i]+k] is
     *  the k-th expansion coefficient in the j-th general contraction of the
     *  i-th shell and offset is the total number of coefficients for all shells
     *  with index lower than i.
     */
    std::vector<double> coefs;

    /** \brief The primitives' exponents.
     *
     *  This is an nshells by nprimitives array where alphas[offset+i] is the
     *  i-th primitive in the current shell, call it j, and offset is the total
     *  number of primitives in all shells with index lower than j.
     */
    std::vector<double> alphas;

    /** \brief The type of the shell (Cartesian, spherical, or Slater)
     *
     * This is an nshells element array where types[i] is the type of shell i,
     * such that i is in the range [0,nshells).
     *
     */
    std::vector<ShellType> types;

    /** \brief The angular momentum of a shell.
     *
     * To my knowledge general contractions work such that they are of a range
     * of angular momenta, like an sp shell, or are such that all of the
     * contractions have the same coefficients.  This array is thus an nshells
     * long array where ls[i] is the angular momentum of shell i, i in the range
     * [0,nshells).  For contractions containing a single angular momentum the
     * stored value is simply that angular momentum, in a.u., whereas for
     * general contractions with multiple angular momentum the value is the
     * negative of the highest angular momentum in the contraction, e.g. -1 is
     * an sp shell.
     */
    std::vector<int> ls;///< The angular momentum of each shell.

    /** \brief Makes an empty BasisSet instance.
     *
     *  The resulting basis set is empty.  Shells can be added via the
     *  add_shell member function or manually.
     *
     *  \throws No throw guarantee.
     */
    BasisSet()noexcept=default;

    /** \brief Frees up the memory associated with the current instance.
     *
     *
     * \throws No throw guarantee.
     */
    ~BasisSet()noexcept=default;

    /** \brief Deep copies another BasisSet instance.
     *
     *  \param[in] other The instance to copy.
     *  \throws std::bad_alloc if memory allocation fails.  Strong throw
     *  guarantee.
     */
    BasisSet(const BasisSet& /*other*/)=default;

    /** \brief Takes ownership of another BasisSet instance.
     *
     *  \param[in] other The instance to take over.
     *  \throws No throw guarantee.
     *  \note After this call \p other will be in a valid, but undefined state.
     */
    BasisSet(BasisSet&& /*other*/)noexcept=default;

    /** \brief Assigns a deep copy of another BasisSet instance to this.
     *
     *  \param[in] other The instance to copy.
     *  \returns The current instance after the copy.
     *  \throws std::bad_alloc if memory allocation fails.  Strong throw
     *  guarantee.
     */
    BasisSet& operator=(const BasisSet& /*other*/)=default;

    /** \brief Takes ownership of another BasisSet instance via assignment.
     *
     *  \param[in] other The instance to take over.
     *  \returns The current instance after taking other's memory
     *  \throws No throw guarantee.
     *  \note After this call \p other will be in a valid, but undefined state.
     */
    BasisSet& operator=(BasisSet&& /*other*/)noexcept=default;
    
    /** \brief Adds a shell to the current basis set.
     *
     * \param[in] center A pointer to the x, y, and z locations (in a.u.) of
     *                   the basis function's center stored contigiously and in
     *                   that order.
     * \param[in] shell The instance from which we are obtaining the shell info.
     *
     * \throws std::bad_alloc if there is insufficient memory. Basic throw
     * guarantee.  Could be made strong by checking member capacities
     * before filling and then copying if a reallocation occurs.
     */
    void add_shell(const double* center, const BasisShell& shell);

    /** \brief Returns the maximum angular momentum in the basis set.
     *
     * \note For general contractions like "sp", "spd", etc. The highest
     * angular momentum of the contraction is considered.  For example, the
     * maximum angular momentum of a general contraction of the form "spd" is 2.
     *
     * \returns The maximum angular momentum in any shell.
     * \throws No throw guarantee.
     */
    size_t max_am()const noexcept;

    /** \brief Returns the total number of basis functions in this basis set.
     *
     * \note The number of basis functions in a general contraction is summed.
     * That is to say a general contraction "sp" has 4 functions not 1.
     *
     * \returns The number of basis functions in the basis set.
     * \throws Throws std::bad_alloc if a temporary string representation of the
     *         angular momentum can not be allocated.
     */
    size_t size()const;

    /** \brief Returns the total number of basis functions in shell i.
     *
     * \note The number of basis functions in a general contraction is summed.
     * That is to say a general contraction "sp" has 4 functions not 1.
     *
     * \param[in] i The shell index.
     * \returns The number of basis functions in shell i.
     * \throws Throws std::bad_alloc if a temporary string representation of the
     *         angular momentum can not be allocated.
     */
    size_t shellsize(const size_t i) const;

    /** \brief Returns true if this instance is exactly equal to \p rhs.
     *
     *  \param[in] rhs The instance to compare to.
     *  \returns True if all members of this are exactly equal to the
     *  corresponding members of \p rhs.
     * \throws No throw guarantee.
     */
    bool operator==(const BasisSet& other)const noexcept;

    /** \brief Returns true if any member of this instance differs from \p rhs.
     *
     *  \param[in] rhs The instance to compare to.
     *  \returns True if any member of this instance is not exactly equal to the
     *  corresponding member of \p rhs.
     * \throws No throw guarantee.
     */
    bool operator!=(const BasisSet& rhs)const noexcept
    {
        return !((*this)==rhs);
    }
};

/** \relates BasisSet
 *
 * \brief Un-generalizes a BasisSet
 * \param[in] basis The instance to un-generalize.
 *
 * \returns A new BasisSet instance where no shell is a general contraction.
 * \throws std::bad_alloc if memory allocation fails.  Strong throw guarantee.
 *
 */
BasisSet ungeneralize_basis_set(const BasisSet& bs);


/** \relates BasisSet
 *
 *  \brief Concatenates two basis sets.
 *
 *  \param[in] lhs The BasisSet instance to append to.
 *  \param[in] rhs The BasisSet instance to append.
 *
 *  \returns \p lhs with rhs added to it.
 *  \throws std::bad_alloc if memory allocation fails.  Weak throw guarantee.
 */
BasisSet& basis_set_concatenate(BasisSet& lhs, const BasisSet& rhs);


}//End namespace


