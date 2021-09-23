#pragma once
#include "libchemist/wavefunction/determinant_space.hpp"

namespace libchemist::wavefunction {

/** @brief Models a nonrelativistic wavefunction
 *
 *  In nonrelativistic quantum mechanics, total spin is a good quantum number.
 *  The Nonrelativistic wavefunction class pairs the spin of the system with a
 *  basis set to expand the wavefunction in (for example a DeterminantSpace).
 *
 *  @tparam BasisType The type of basis set that the wavefunction is expanded
 *                    in. Assumed to be an instantiation of DeterminantSpace.
 */
template<typename BasisType>
class Nonrelativistic {
public:
    /// Type of the total spin
    using spin_type = float;

    /// Type of the multiplicity
    using multiplicity_type = unsigned int;

    /// Type of the basis_set
    using basis_set_type = BasisType;

    /** @brief Makes a non-relativistic wavefunction with the specified state.
     *
     *  This constructor serves as both the default ctor (making a wavefunction
     *  with no basis set and no spin) as well as the value ctor. The only way
     *  to change the state of a wavefunction after creation is by assigning to
     *  it.
     *
     *  @param[in] ref The basis set for the wavefunction. Default is an empty
     *                 set.
     *  @param[in] spin The total spin of the wavefunction. Default is zero.
     *
     *  @throw None No throw guarantee.
     */
    explicit Nonrelativistic(basis_set_type ref = {}, spin_type spin = 0);

    /** @brief The multiplicity of the wavefunction.
     *
     *  The multiplicity of a wavefunction with total spin @f$S@f$ is
     *  @f$2S + 1@f$. This function uses the spin set by the user to compute
     *  the multiplicity.
     *
     *  @return The multiplicity of the wavefunction.
     *
     *  @throw None No throw guarantee.
     */
    multiplicity_type multiplicity() const noexcept { return 2 * m_spin_ + 1; }

    /** @brief The total spin of the wavefunction
     *
     *  This is the spin set by the user. Spin contamination in the actual
     *  wavefunction may cause the actual value to deviate.
     *
     *  @return The spin of the wavefunction
     *
     *  @throw None No throw guarantee.
     */
    spin_type spin() const noexcept { return m_spin_; }

    /** @brief The basis_set this wavefunction is based on.
     *
     *  The basis set for a wavefunction is a set of determinants or
     *  configuration state functions. This function returns this set, which in
     *  turn will contain the more traditional quantities known as "basis set"
     *  (*e.g.* the AOs or MOs).
     *
     *  @return The basis_set used to construct this wavefunction.
     */
    const auto& basis_set() const noexcept { return m_basis_; }

    /** @brief Hashes the wavefunction.
     *
     *  This function will add a hash of the wavefunction to the provided
     *  Hasher instance
     *
     *  @param[in,out] h The instance to use for hashing. After this call @p h
     *                   will contain a hash of this wavefunction.
     */
    void hash(pluginplay::Hasher& h) const { h(m_spin_, m_basis_); }

private:
    /// The total spin of the wavefunction
    spin_type m_spin_;

    /// The basis_set
    basis_set_type m_basis_;
};

/** @brief Convenience function for making a wavefunction from occupied and
 *         virtual orbitals.
 *
 *  @relates Nonrelativistic
 *
 *  A common scenario is the need to prepare a wavefunction given a set of
 *  occupied and virtual orbitals. This function takes care of preparing the
 *  determinant space for you and returns the wavefunction object.
 *
 *  @return The determinant-based wavefunction.
 */
template<typename OccType, typename VirtType>
auto make_wavefunction(OccType&& occ, VirtType&& virt, operators::Fock fock,
                       unsigned int spin = 0);

/** @brief Compares two non-relativistic wavefunctions for equality.
 *
 *  @relates Nonrelativistic
 *
 *  Two nonrelativistic wavefunctions are equal if they have the same basis set
 *  and the same spin. Otherwise they are different.
 *
 *  @tparam LHSRefType The type of @p LHS's basis set. Same type requirements as
 *                     the Nonrelativistic class's BasisType type parameter.
 *  @tparam RHSRefType The type of @p RHS's basis set. Same type requirements as
 *                     the Nonrelativistic class's BasisType type parameter.
 *
 *  @param[in] lhs The Nonrelativistic instance on the left of the equality
 *                 operator.
 *  @param[in] rhs The Nonrelativistic instance on the right of the equality
 *                 operator.
 *  @return True if @p lhs compares equal to @p rhs and false otherwise.
 *
 *  @throw ??? Throws if comparing the basis sets throws. Strong throw
 *             guarantee.
 */
template<typename LHSRefType, typename RHSRefType>
bool operator==(const Nonrelativistic<LHSRefType>& lhs,
                const Nonrelativistic<RHSRefType>& rhs);

/** @brief Determines if two non-relativistic wavefunctions are different.
 *
 *  @relates Nonrelativistic
 *
 *  Two nonrelativistic wavefunctions are equal if they have the same basis set
 *  and the same spin. Otherwise they are different.
 *
 *  @tparam LHSRefType The type of @p LHS's basis set. Same type requirements as
 *                     the Nonrelativistic class's BasisType type parameter.
 *  @tparam RHSRefType The type of @p RHS's basis set. Same type requirements as
 *                     the Nonrelativistic class's BasisType type parameter.
 *
 *  @param[in] lhs The Nonrelativistic instance on the left of the not equal
 *                 operator.
 *  @param[in] rhs The Nonrelativistic instance on the right of the not equal
 *                 operator.
 *
 *  @return False if @p lhs compares equal to @p rhs and true otherwise.
 *
 *  @throw ??? Throws if comparing the basis sets throws. Strong throw
 *             guarantee.
 */
template<typename LHSRefType, typename RHSRefType>
bool operator!=(const Nonrelativistic<LHSRefType>& lhs,
                const Nonrelativistic<RHSRefType>& rhs) {
    return !(lhs == rhs);
}

/// Type of a wavefunction built from MOs
using Reference = Nonrelativistic<Determinant>;

/// Type of a wavefunction built from canonical MOs
using CanonicalReference = Nonrelativistic<CanonicalDeterminant>;

using SparseReference = Nonrelativistic<SparseDeterminant>;

} // namespace libchemist::wavefunction

#include "nonrelativistic.ipp"
