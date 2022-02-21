#include "chemist/wavefunction/determinant_space.hpp"

using namespace chemist::orbital_space;

namespace chemist::wavefunction {

#define DETERMINANT_SPACE_TPARAMS \
    template<typename OccupiedSpaceType, typename VirtualSpaceType>
#define DETERMINANT_SPACE DeterminantSpace<OccupiedSpaceType, VirtualSpaceType>

// -----------------------------------------------------------------------------
//                                CTors
// -----------------------------------------------------------------------------

DETERMINANT_SPACE_TPARAMS
DETERMINANT_SPACE::DeterminantSpace(occupied_orbital_type occ,
                                    virtual_orbital_type virt,
                                    fock_operator_type fock) :
  DeterminantSpace(std::make_shared<occupied_orbital_type>(std::move(occ)),
                   std::make_shared<virtual_orbital_type>(std::move(virt)),
                   std::make_shared<fock_operator_type>(std::move(fock))) {}

DETERMINANT_SPACE_TPARAMS
DETERMINANT_SPACE::DeterminantSpace(const_occupied_pointer pocc,
                                    const_virtual_pointer pvir,
                                    const_fock_pointer pfock) :
  m_pocc_(pocc), m_pvirt_(pvir), m_pfock_(pfock) {}

// Accessors

DETERMINANT_SPACE_TPARAMS typename DETERMINANT_SPACE::const_occupied_reference
DETERMINANT_SPACE::occupied_orbitals() const {
    if(m_pocc_) return *m_pocc_;
    throw std::runtime_error("Space has no occupied orbitals.");
}

DETERMINANT_SPACE_TPARAMS typename DETERMINANT_SPACE::const_virtual_reference
DETERMINANT_SPACE::virtual_orbitals() const {
    if(m_pvirt_) return *m_pvirt_;
    throw std::runtime_error("Space has no virtual orbitals.");
}

DETERMINANT_SPACE_TPARAMS typename DETERMINANT_SPACE::const_fock_reference
DETERMINANT_SPACE::fock_operator() const {
    if(m_pfock_) return *m_pfock_;
    throw std::runtime_error("Space has no Fock operator.");
}

// Utility

DETERMINANT_SPACE_TPARAMS
bool DETERMINANT_SPACE::operator==(const DeterminantSpace& rhs) const {
    if(m_pocc_) {
        if(!rhs.m_pocc_) return false;
        if(occupied_orbitals() != rhs.occupied_orbitals()) return false;
    } else if(rhs.m_pocc_)
        return false;

    if(m_pvirt_) {
        if(!rhs.m_pvirt_) return false;
        if(virtual_orbitals() != rhs.virtual_orbitals()) return false;
    } else if(rhs.m_pvirt_)
        return false;

    if(m_pfock_) {
        if(!rhs.m_pfock_) return false;
        if(fock_operator() != rhs.fock_operator()) return false;
    } else if(rhs.m_pfock_)
        return false;

    return true;
}

DETERMINANT_SPACE_TPARAMS
void DETERMINANT_SPACE::hash(chemist::detail_::Hasher& h) const {
    h(m_pocc_, m_pvirt_); //, m_fock_);
}

#undef DETERMINANT_SPACE
#undef DETERMINANT_SPACE_TPARAMS

template class DeterminantSpace<DerivedSpaceD>;
template class DeterminantSpace<CanonicalSpaceD>;
template class DeterminantSpace<CanonicalIndSpace>;
template class DeterminantSpace<CanonicalIndSpace, CanonicalToTSpace>;

} // namespace chemist::wavefunction
