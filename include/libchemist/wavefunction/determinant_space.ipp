#pragma once

/** @file determinant_space.ipp
 *
 *  This file contains the definitions of member functions for the
 *  DeterminantSpace class and functions associated with it. It should only be
 *  included in determinant_space.hpp
 */

namespace libchemist::wavefunction {

#define DETERMINANT_SPACE DeterminantSpace<OccupiedSpaceType, VirtualSpaceType>

template<typename OccupiedSpaceType, typename VirtualSpaceType>
DETERMINANT_SPACE::DeterminantSpace(occupied_orbital_type occ,
                                    virtual_orbital_type virt,
                                    fock_operator_type fock) :
  m_occ_(std::move(occ)), m_virt_(std::move(virt)), m_fock_(std::move(fock)) {}

template<typename OccupiedSpaceType, typename VirtualSpaceType>
void DETERMINANT_SPACE::hash(sde::Hasher& h) const {
    h(m_occ_, m_virt_); //, m_fock_);
}

#undef DETERMINANT_SPACE

// ----------------------- Associated Functions --------------------------------

template<typename LHSOccSpace, typename LHSVirtSpace, typename RHSOccSpace,
         typename RHSVirtSpace>
bool operator==(const DeterminantSpace<LHSOccSpace, LHSVirtSpace>& lhs,
                const DeterminantSpace<RHSOccSpace, RHSVirtSpace>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return false;
    } else {
        const auto& lhs_occ  = lhs.occupied_orbitals();
        const auto& lhs_virt = lhs.virtual_orbitals();
        const auto& lhs_fock = lhs.fock_operator();

        const auto& rhs_occ  = rhs.occupied_orbitals();
        const auto& rhs_virt = rhs.virtual_orbitals();
        const auto& rhs_fock = rhs.fock_operator();

        return std::tie(lhs_occ, lhs_virt, lhs_fock) ==
               std::tie(rhs_occ, rhs_virt, rhs_fock);
    }
}

extern template class DeterminantSpace<orbital_space::DerivedSpaceD>;
extern template class DeterminantSpace<orbital_space::IndDerivedSpaceD,
                                       orbital_space::DepDerivedSpaceD>;

extern template class DeterminantSpace<orbital_space::CanonicalSpaceD>;
extern template class DeterminantSpace<orbital_space::CanonicalIndSpaceD,
                                       orbital_space::CanonicalDepSpaceD>;

} // namespace libchemist::wavefunction
