#include "chemist/wavefunction/active_space.hpp"

using namespace chemist::orbital_space;

namespace chemist::wavefunction {

#define ACTIVE_SPACE_TPARAMS \
    template<typename InactiveSpaceType, typename ActiveSpaceType, typename VirtualSpaceType, typename CoreSpaceType>
#define ACTIVE_SPACE ActiveSpace<InactiveSpaceType, ActiveSpaceType, VirtualSpaceType, CoreSpaceType>

// -----------------------------------------------------------------------------
//                                CTors
// -----------------------------------------------------------------------------

ACTIVE_SPACE_TPARAMS
ACTIVE_SPACE::ActiveSpace(inactive_orbital_type inact,
                          active_orbital_type act,
                          virtual_orbital_type virt,
                          core_orbital_type core,
                          fock_operator_type inact_fock,
                          fock_operator_type act_fock) :
  ActiveSpace(std::make_shared<inactive_orbital_type>(std::move(inact)),
              std::make_shared<active_orbital_type>(std::move(act)),
              std::make_shared<virtual_orbital_type>(std::move(virt)),
              std::make_shared<core_orbital_type>(std::move(core)),
              std::make_shared<fock_operator_type>(std::move(inact_fock)),
              std::make_shared<fock_operator_type>(std::move(act_fock))) {}

ACTIVE_SPACE_TPARAMS
ACTIVE_SPACE::ActiveSpace(const_inactive_pointer pinact,
                          const_active_pointer pact,
                          const_virtual_pointer pvir,
                          const_core_pointer pcore,
                          const_fock_pointer pinact_fock,
                          const_fock_pointer pact_fock) :
  m_pinact_(pinact), m_pact_(pact), m_pvirt_(pvir), m_pcore_(pcore),
  m_pinact_fock_(pinact_fock), m_pact_fock_(pact_fock) {}

// Accessors

ACTIVE_SPACE_TPARAMS typename ACTIVE_SPACE::const_inactive_reference
ACTIVE_SPACE::inactive_orbitals() const {
    if(m_pinact_) return *m_pinact_;
    throw std::runtime_error("Space has no inactive orbitals.");
}

ACTIVE_SPACE_TPARAMS typename ACTIVE_SPACE::const_active_reference
ACTIVE_SPACE::active_orbitals() const {
    if(m_pact_) return *m_pact_;
    throw std::runtime_error("Space has no active orbitals.");
}

ACTIVE_SPACE_TPARAMS typename ACTIVE_SPACE::const_virtual_reference
ACTIVE_SPACE::virtual_orbitals() const {
    if(m_pvirt_) return *m_pvirt_;
    throw std::runtime_error("Space has no virtual orbitals.");
}

ACTIVE_SPACE_TPARAMS typename ACTIVE_SPACE::const_core_reference
ACTIVE_SPACE::core_orbitals() const {
    if(m_pcore_) return *m_pcore_;
    throw std::runtime_error("Space has no core orbitals.");
}

ACTIVE_SPACE_TPARAMS typename ACTIVE_SPACE::const_fock_reference
ACTIVE_SPACE::inactive_fock_operator() const {
    if(m_pinact_fock_) return *m_pinact_fock_;
    throw std::runtime_error("Space has no inactive Fock operator.");
}

ACTIVE_SPACE_TPARAMS typename ACTIVE_SPACE::const_fock_reference
ACTIVE_SPACE::active_fock_operator() const {
    if(m_pact_fock_) return *m_pact_fock_;
    throw std::runtime_error("Space has no active Fock operator.");
}


// Utility

ACTIVE_SPACE_TPARAMS
bool ACTIVE_SPACE::operator==(const ActiveSpace& rhs) const {
    if(m_pinact_) {
        if(!rhs.m_pinact_) return false;
        if(inactive_orbitals() != rhs.inactive_orbitals()) return false;
    } else if(rhs.m_pinact_)
        return false;

    if(m_pact_) {
        if(!rhs.m_pact_) return false;
        if(active_orbitals() != rhs.active_orbitals()) return false;
    } else if(rhs.m_pact_)
        return false;

    if(m_pvirt_) {
        if(!rhs.m_pvirt_) return false;
        if(virtual_orbitals() != rhs.virtual_orbitals()) return false;
    } else if(rhs.m_pvirt_)
        return false;

    if(m_pcore_) {
        if(!rhs.m_pcore_) return false;
        if(core_orbitals() != rhs.core_orbitals()) return false;
    } else if(rhs.m_pcore_)
        return false;

    if(m_pinact_fock_) {
        if(!rhs.m_pinact_fock_) return false;
        if(inactive_fock_operator() != rhs.inactive_fock_operator()) return false;
    } else if(rhs.m_pinact_fock_)
        return false;

    if(m_pact_fock_) {
        if(!rhs.m_pact_fock_) return false;
        if(active_fock_operator() != rhs.active_fock_operator()) return false;
    } else if(rhs.m_pact_fock_)
        return false;

    return true;
}

ACTIVE_SPACE_TPARAMS
void ACTIVE_SPACE::hash(chemist::detail_::Hasher& h) const {
    h(m_pinact_, m_pact_, m_pvirt_, m_pcore_); //, m_inact_fock_, m_act_fock_);
}

#undef ACTIVE_SPACE
#undef ACTIVE_SPACE_TPARAMS

template class ActiveSpace<DerivedSpaceD>;

} // namespace chemist::wavefunction
