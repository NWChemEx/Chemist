#pragma once

/** @file nonrelativistic.ipp
 *
 *  Contains implementations of the functions associatd with the Nonrelativistic
 *  class. This file should only be included in nonrelativistic.hpp.
 */

namespace libchemist::wavefunction {

#define NONRELATIVISTIC Nonrelativistic<BasisType>

template<typename BasisType>
NONRELATIVISTIC::Nonrelativistic(basis_set_type ref, spin_type spin) :
  m_spin_(spin), m_basis_(std::move(ref)) {}

#undef NONRELATIVISTIC

// ----------------------------- Related Functions -----------------------------

template<typename OccType, typename VirtType>
auto make_wavefunction(OccType&& occ, VirtType&& virt, operators::Fock fock,
                       unsigned int spin) {
    using clean_occ  = std::decay_t<OccType>;
    using clean_virt = std::decay_t<VirtType>;
    using det_type   = DeterminantSpace<clean_occ, clean_virt>;
    det_type det(std::forward<OccType>(occ), std::forward<VirtType>(virt),
                 std::move(fock));
    return Nonrelativistic<det_type>(std::move(det), spin);
}

template<typename LHSRefType, typename RHSRefType>
bool operator==(const Nonrelativistic<LHSRefType>& lhs,
                const Nonrelativistic<RHSRefType>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return false;
    } else {
        return (lhs.spin() == rhs.spin()) &&
               (lhs.basis_set() == rhs.basis_set());
    }
}

extern template class Nonrelativistic<Determinant>;
extern template class Nonrelativistic<CanonicalDeterminant>;
extern template class Nonrelativistic<LocalDeterminant>;
extern template class Nonrelativistic<CanonicalLocalDeterminant>;

} // namespace libchemist::wavefunction
