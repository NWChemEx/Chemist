#include "chemical_system_pimpl.hpp"
#include <stdexcept>

namespace libchemist {

using pimpl_t    = typename ChemicalSystem::pimpl_t;
using molecule_t = typename ChemicalSystem::molecule_t;
using basis_t    = typename ChemicalSystem::ao_basis_t;
using epot_t     = typename ChemicalSystem::epot_t;

ChemicalSystem::ChemicalSystem() : m_pimpl_(std::make_unique<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(const ChemicalSystem& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() :
                            std::unique_ptr<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(ChemicalSystem&& other) noexcept = default;

ChemicalSystem::ChemicalSystem(molecule_t mol, ao_basis_t aos, epot_t v) :
  m_pimpl_(std::make_unique<pimpl_t>(std::move(mol), std::move(aos), std::move(v))) {}

ChemicalSystem::~ChemicalSystem() noexcept = default;

ChemicalSystem& ChemicalSystem::operator=(const ChemicalSystem& rhs) {
    if(&rhs == this) return *this;
    if(rhs.m_pimpl_)
        rhs.m_pimpl_->clone().swap(m_pimpl_);
    else
        m_pimpl_.reset();
    return *this;
}

ChemicalSystem& ChemicalSystem::operator=(ChemicalSystem&& rhs) noexcept =
  default;

// ---------------------------- Accessors --------------------------------------

molecule_t& ChemicalSystem::molecule() { return pimpl_().molecule(); }

const molecule_t& ChemicalSystem::molecule() const {
    return pimpl_().molecule();
}

basis_t& ChemicalSystem::basis_set() { return pimpl_().basis_set(); }

const basis_t& ChemicalSystem::basis_set() const { return pimpl_().basis_set(); }

epot_t& ChemicalSystem::external_electrostatic_potential() {
    return pimpl_().external_electrostatic_potential();
}

const epot_t& ChemicalSystem::external_electrostatic_potential() const {
    return pimpl_().external_electrostatic_potential();
}

bool ChemicalSystem::operator==(const ChemicalSystem& rhs) const noexcept {
    if(!m_pimpl_ && !rhs.m_pimpl_)
        return true;
    else if(!m_pimpl_ || !rhs.m_pimpl_)
        return false;
    return (*m_pimpl_) == (*rhs.m_pimpl_);
}

void ChemicalSystem::hash(bphash::Hasher& h) const { h(m_pimpl_); }

// ---------------------- Private Member Functions -----------------------------

pimpl_t& ChemicalSystem::pimpl_() {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<pimpl_t>();
    return *m_pimpl_;
}

const pimpl_t& ChemicalSystem::pimpl_() const {
    if(m_pimpl_) return *m_pimpl_;
    const auto msg =
      "ChemicalSystem is in NULL state. Did you move from this instance?";
    throw std::runtime_error(msg);
}

} // namespace libchemist
