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
 *                    in.
 */
template<typename BasisType>
class Nonrelativistic {
public:
    /// Type of the total spin
    using spin_type = unsigned int;

    /// Type of the basis_set
    using basis_set_type = BasisType;

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
    spin_type multiplicity() const noexcept { return 2 * m_spin_ + 1; }

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
auto make_wavefunction(OccType&& occ, VirtType&& virt,
                       //FockOperator fock,
                       unsigned int spin = 0);


template<typename LHSRefType, typename RHSRefType>
bool operator==(const Nonrelativistic<LHSRefType>& lhs,
                const Nonrelativistic<RHSRefType>& rhs);

template<typename LHSRefType, typename RHSRefType>
bool operator!=(const Nonrelativistic<LHSRefType>& lhs,
                const Nonrelativistic<RHSRefType>& rhs) {
    return !(lhs == rhs);
}

using Reference               = Nonrelativistic<DeterminantD>;
using CanonicalReference      = Nonrelativistic<CanonicalDeterminantD>;
using LocalReference          = Nonrelativistic<LocalDeterminantD>;
using CanonicalLocalReference = Nonrelativistic<CanonicalLocalDeterminantD>;

} // namespace libchemist::wavefunction

#include "nonrelativistic.ipp"
