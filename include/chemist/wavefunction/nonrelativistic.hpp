#pragma once
#include "chemist/wavefunction/determinant_space.hpp"

namespace chemist::wavefunction {

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
    /// Determines if BasisType is implicitly convertible to @p T
    template<typename T>
    static constexpr bool bs_is_convertible_v =
      std::is_convertible_v<BasisType, T>;

    /// Enables a function if @p T is implicitly convertible to BasisType
    template<typename T>
    using enable_if_convertible_t = std::enable_if_t<bs_is_convertible_v<T>>;

public:
    /// Type of the total spin
    using spin_type = float;

    /// Type of the multiplicity
    using multiplicity_type = unsigned int;

    /// Type of the basis_set
    using basis_set_type = BasisType;

    /// Type of a read-only reference to the basis set
    using const_basis_set_reference = const basis_set_type&;

    /// Type of a read-only pointer to the basis set
    using const_basis_set_pointer = std::shared_ptr<const basis_set_type>;

    Nonrelativistic() = default;

    /** @brief Makes a non-relativistic wavefunction with the specified state.
     *
     *  This constructor will create a wavefunction with the specified state.
     *  The only way to change the state of the wavefunction after creation is
     *  by assigning to it.
     *
     *  @param[in] ref The basis set for the wavefunction.
     *  @param[in] spin The total spin of the wavefunction. Default is zero.
     *
     *  @throw std::bad_alloc if allocating the poitner for @p ref fails. Strong
     *                        throw guarantee
     */
    explicit Nonrelativistic(basis_set_type ref, spin_type spin = 0);

    Nonrelativistic(const_basis_set_pointer pref, spin_type spin);

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
    const_basis_set_reference basis_set() const;

    /** @brief Adds implicit conversions among various Nonrelativistic
     *         wavefunctions.
     *
     *  @tparam T The type of the basis set we are attempting to convert to.
     *  @tparam <Anonymous> Used to disable the conversion when BasisType can
     *                      not be implicitly converted to @p T.
     *
     *  @return The current wavefunction casted to a wavefunction of type
     *          Nonrelativistic<T>.
     */
    template<typename T, typename = enable_if_convertible_t<T>>
    operator Nonrelativistic<T>() const {
        return Nonrelativistic<T>(m_pbasis_ ? T(*m_pbasis_) : T(), m_spin_);
    }

    bool operator==(const Nonrelativistic& rhs) const;

    /** @brief Hashes the wavefunction.
     *
     *  This function will add a hash of the wavefunction to the provided
     *  Hasher instance
     *
     *  @param[in,out] h The instance to use for hashing. After this call @p h
     *                   will contain a hash of this wavefunction.
     */
    void hash(chemist::detail_::Hasher& h) const { h(m_spin_, m_pbasis_); }

private:
    /// The total spin of the wavefunction
    spin_type m_spin_ = 0;

    /// The basis_set
    const_basis_set_pointer m_pbasis_;
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
                       unsigned int spin = 0) {
    using clean_occ  = std::decay_t<OccType>;
    using clean_virt = std::decay_t<VirtType>;
    using det_type   = DeterminantSpace<clean_occ, clean_virt>;
    det_type det(std::forward<OccType>(occ), std::forward<VirtType>(virt),
                 std::move(fock));
    return Nonrelativistic<det_type>(std::move(det), spin);
}

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
                const Nonrelativistic<RHSRefType>& rhs) {
    if constexpr(!std::is_convertible_v<RHSRefType, LHSRefType>) {
        return false;
    } else {
        return lhs.operator==(rhs);
    }
}

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

/// Type of a wavefunction built from MOs with a sparsity relationship
using SparseReference = Nonrelativistic<SparseDeterminant>;

using SparseToTReference = Nonrelativistic<SparseToTDeterminant>;

extern template class Nonrelativistic<Determinant>;
extern template class Nonrelativistic<CanonicalDeterminant>;
extern template class Nonrelativistic<SparseDeterminant>;
extern template class Nonrelativistic<SparseToTReference>;

} // namespace chemist::wavefunction
