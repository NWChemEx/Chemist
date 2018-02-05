#pragma once
#include "LibChemist/ShellTypes.hpp"
#include <vector>

namespace LibChemist {

/** \brief Class for holding the details of a basis set.
 *
 *  This class is meant to hold the information relating to a particular basis
 *  set shell.  It does not hold the center's location, but rather the exponents
 *  and coefficients.  These values live within this class and are thus not
 *  aliases or pointers to a contigious buffer.  When a user requests a BasisSet
 *  instance from a molecule the exponents and coefficients will be put into a
 *  contigious buffer.
 *
 *  \note This class is aware of general contractions.  What this means is
 *  rather than thinking of this as being the exponents and coefficients for
 *  \f$n\f$ contractions where \f$n\f$ is the number of primitive functions this
 *  class is actually \f$n\f$ exponents and \f$m\f$ rows of \f$n\f$ coefficients
 *  where \f$m\f$ is the number of contractions in the instance.  The main place
 *  this will get you is if your basis set has an "sp", "spd", etc. shell.
 *  These are examples of general contractions, but most codes expect the
 *  shells to be uncompressed. That can be done at the molecule level.
 *
 */
class BasisShell {
    private:
    /// A ngen_ by nprim_ array of expansion coefficients stored row-major
    std::vector<double> cs_;

    /// A nprim_ long array of primitive exponents
    std::vector<double> alphas_;

    public:
    /// The type of the shell
    ShellType type;

    /// The angular momentum of the shell
    int l;

    /// The number of general contractions in this shell
    size_t ngen;

    /// The number of primitives in this shell
    size_t nprim;

    /** \brief Constructs a new BasisShell by copying the input values.
     *
     *  \param[in] type The type (i.e. Cartesian, spherical, or slater) of the
     *                   shell.
     *  \param[in] l The angular momentum of the shell.
     *  \param[in] ngen The number of general contractions in the shell.
     *  \param[in] alphas The exponents of the primitives.
     *  \param[in] coefs  The expansion coefficients of the primitives.
     *
     *  \throws std::bad_alloc if memory allocation fails.  Strong throw
     *  guarantee.
     */
    BasisShell(ShellType type_, int l_, size_t ngen_,
               const std::vector<double>& alphas,
               const std::vector<double>& coefs) :
      cs_(coefs),
      alphas_(alphas),
      type(type_),
      l(l_),
      ngen(ngen_),
      nprim(alphas.size()) {}

    /** \brief Constructs a new BasisShell instance by moving the input values.
     *
     *  \param[in] type The type (i.e. Cartesian, spherical, or slater) of the
     *                   shell.
     *  \param[in] l The angular momentum of the shell.
     *  \param[in] ngen The number of general contractions in the shell.
     *  \param[in] alphas The exponents of the primitives.
     *  \param[in] coefs  The expansion coefficients of the primitives.
     *
     *  \throws No throw guarantee.
     */
    BasisShell(ShellType type_, int l_, size_t ngen_,
               std::vector<double>&& alphas,
               std::vector<double>&& coefs) noexcept :
      cs_(std::move(coefs)),
      alphas_(std::move(alphas)),
      type(type_),
      l(l_),
      ngen(ngen_),
      nprim(alphas_.size()) {}

    /** \brief Creates a default BasisShell instance.
     *
     *  The resulting instance is unusable aside from being a placeholder.
     *  \throw No throw guarantee.
     */
    BasisShell() noexcept = default;

    /** \brief Constructs a new BasisShell by deep copying.
     *
     * \param[in] other The BasisShell instance to copy.
     *
     * \throw std::bad_alloc if memory allocation fails.  Strong throw
     * guarantee.
     */
    BasisShell(const BasisShell& /*other*/) = default;

    /** \brief Takes ownership of another BasisShell instance.
     *
     *  \param[in] other The BasisShell instance to take ownership of.
     *  \throw No throw guarantee.
     *  \note After this function other is in a valid, but undefined state.
     */
    BasisShell(BasisShell&& /*other*/) noexcept = default;

    /** \brief Assigns a deep copy of another BasisShell instance to this
     *         instance.
     *
     * \param[in] other The BasisShell instance to deep copy.
     * \returns The current instance after the deep copy.
     * \throw std::bad_alloc if memory allocation fails.  Strong throw
     *  guarantee.
     */
    BasisShell& operator=(const BasisShell& /*other*/) = default;

    /** \brief Takes ownership of another BasisShell instance via assignment.
     *
     * \param[in] other The BasisShell instance to take over.
     * \returns The current instance after taking \p other 's data.
     * \throw No throw guarantee.
     * \note After this function \p other is in a valid, but undefined state.
     */
    BasisShell& operator=(BasisShell&& /*other*/) noexcept = default;

    /** \brief Returns true if this instance is exactly equal to another
     *   instance.
     *
     *  \param[in] rhs The instance to compare against.
     *  \returns True if this instance is exactly equal to \p rhs.
     *  \throw No throw guarantee.
     *
     */
    bool operator==(const BasisShell& rhs) const noexcept;

    /** \brief Returns true if any part of this instance differs from another
     *     instance.
     *
     * \param[in]  rhs The instance to compare against.
     * \returns True if any member of this instance differs from the
     *    corresponding member of \p rhs.
     * \throw No throw guarantee.
     */
    bool operator!=(const BasisShell& rhs) const noexcept {
        return !((*this) == rhs);
    }

    /** \brief Returns the i-th exponent
     *  \param[in] i Which exponent to return. I in range [0,number of prims)
     *  \returns The requested exponent
     *  \throw No throw guarantee.
     */
    double alpha(size_t i) const noexcept { return alphas_[i]; }

    /** \brief Returns the i-th coefficient of the j-th contraction
     *  \param[in] i Which coefficientt to return. I in range
     *             [0,number of prims)
     *  \param[in] j Which contraction to use. J in range
     *             [0,number of general contractions)
     *  \returns The requested coefficient
     *  \throw No throw guarantee.
     */
    double coef(size_t i, size_t j) const noexcept {
        return cs_[j * nprim + i];
    }

    /** \brief Returns the number of basis functions in the i-th contraction
     *
     *  \note This is not the number of primitives in the i-th contraction, but
     *  rather 2l+1 for pure/Slater shells and 3 multichoose l for Cartesian
     *  shells.
     */
    size_t nfunctions(size_t i) const noexcept;
};

} // namespace LibChemist
