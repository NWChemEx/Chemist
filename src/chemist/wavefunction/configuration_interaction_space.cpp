/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chemist/wavefunction/configuration_interaction_space.hpp>

using namespace chemist::orbital_space;

namespace chemist::wavefunction {

#define CONFIGURATION_INTERACTION_SPACE_TPARAMS \
    template<typename InactiveSpaceType, typename ActiveSpaceType, typename VirtualSpaceType, typename CoreSpaceType>
#define CONFIGURATION_INTERACTION_SPACE ConfigurationInteractionSpace<InactiveSpaceType, ActiveSpaceType, VirtualSpaceType, CoreSpaceType>

// -----------------------------------------------------------------------------
//                                CTors
// -----------------------------------------------------------------------------

CONFIGURATION_INTERACTION_SPACE_TPARAMS
CONFIGURATION_INTERACTION_SPACE::ConfigurationInteractionSpace( size_type na, size_type nb,
                          inactive_orbital_type inact,
                          active_orbital_type act,
                          virtual_orbital_type virt,
                          core_orbital_type core,
                          active_1rdm_type ordm,
                          active_2rdm_type trdm) :
  ConfigurationInteractionSpace(na,nb,
              std::make_shared<inactive_orbital_type>(std::move(inact)),
              std::make_shared<active_orbital_type>(std::move(act)),
              std::make_shared<virtual_orbital_type>(std::move(virt)),
              std::make_shared<core_orbital_type>(std::move(core)),
              std::make_shared<active_1rdm_type>(std::move(ordm)),
              std::make_shared<active_2rdm_type>(std::move(trdm))) {}

CONFIGURATION_INTERACTION_SPACE_TPARAMS
CONFIGURATION_INTERACTION_SPACE::ConfigurationInteractionSpace(size_type na, size_type nb,
                          const_inactive_pointer pinact,
                          const_active_pointer pact,
                          const_virtual_pointer pvir,
                          const_core_pointer pcore,
                          const_active_1rdm_pointer p1rdm,
                          const_active_2rdm_pointer p2rdm) :
  m_n_act_alpha_(na), m_n_act_beta_(nb), 
  m_pinact_(pinact), m_pact_(pact), m_pvirt_(pvir), m_pcore_(pcore),
  m_p1rdm_(p1rdm), m_p2rdm_(p2rdm) {}

// Accessors

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::size_type
CONFIGURATION_INTERACTION_SPACE::n_active_alpha_electrons() const {
    return m_n_act_alpha_;
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::size_type
CONFIGURATION_INTERACTION_SPACE::n_active_beta_electrons() const {
    return m_n_act_beta_;
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::const_inactive_reference
CONFIGURATION_INTERACTION_SPACE::inactive_orbitals() const {
    if(m_pinact_) return *m_pinact_;
    throw std::runtime_error("Space has no inactive orbitals.");
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::const_active_reference
CONFIGURATION_INTERACTION_SPACE::active_orbitals() const {
    if(m_pact_) return *m_pact_;
    throw std::runtime_error("Space has no active orbitals.");
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::const_virtual_reference
CONFIGURATION_INTERACTION_SPACE::virtual_orbitals() const {
    if(m_pvirt_) return *m_pvirt_;
    throw std::runtime_error("Space has no virtual orbitals.");
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::const_core_reference
CONFIGURATION_INTERACTION_SPACE::core_orbitals() const {
    if(m_pcore_) return *m_pcore_;
    throw std::runtime_error("Space has no core orbitals.");
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::const_active_1rdm_reference
CONFIGURATION_INTERACTION_SPACE::active_1rdm() const {
    if(m_p1rdm_) return *m_p1rdm_;
    throw std::runtime_error("Space has no 1RDM.");
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS typename CONFIGURATION_INTERACTION_SPACE::const_active_2rdm_reference
CONFIGURATION_INTERACTION_SPACE::active_2rdm() const {
    if(m_p2rdm_) return *m_p2rdm_;
    throw std::runtime_error("Space has no 2RDM.");
}


// Utility

CONFIGURATION_INTERACTION_SPACE_TPARAMS
bool CONFIGURATION_INTERACTION_SPACE::operator==(const ConfigurationInteractionSpace& rhs) const {

    if( m_n_act_alpha_ != rhs.n_active_alpha_electrons() ) return false;
    if( m_n_act_beta_ != rhs.n_active_beta_electrons() )   return false;

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

    if(m_p1rdm_) {
        if(!rhs.m_p1rdm_) return false;
        if(active_1rdm() != rhs.active_1rdm()) return false;
    } else if(rhs.m_p1rdm_)
        return false;

    if(m_p2rdm_) {
        if(!rhs.m_p2rdm_) return false;
        if(active_2rdm() != rhs.active_2rdm()) return false;
    } else if(rhs.m_p2rdm_)
        return false;

    return true;
}

CONFIGURATION_INTERACTION_SPACE_TPARAMS
void CONFIGURATION_INTERACTION_SPACE::hash(chemist::detail_::Hasher& h) const {
    h(m_n_act_alpha_, m_n_act_beta_, m_pinact_, m_pact_, m_pvirt_, m_pcore_); //, m_inact_fock_, m_act_fock_);
}

#undef CONFIGURATION_INTERACTION_SPACE
#undef CONFIGURATION_INTERACTION_SPACE_TPARAMS

template class ConfigurationInteractionSpace<DerivedSpaceD>;

} // namespace chemist::wavefunction
