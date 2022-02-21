#include "chemical_system_pimpl.hpp"
#include <stdexcept>

namespace chemist {

using pimpl_t    = typename ChemicalSystem::pimpl_t;
using molecule_t = typename ChemicalSystem::molecule_t;
using epot_t     = typename ChemicalSystem::epot_t;
using size_type  = typename ChemicalSystem::size_type;

namespace {
auto sum_z(const Molecule& mol) {
    size_type n = 0;
    for(const auto& atom : mol) n += atom.Z();
    return n;
}
} // namespace

ChemicalSystem::ChemicalSystem() : m_pimpl_(std::make_unique<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(const ChemicalSystem& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() :
                            std::unique_ptr<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(ChemicalSystem&& other) noexcept = default;

ChemicalSystem::ChemicalSystem(molecule_t mol, epot_t v) :
  ChemicalSystem(std::move(mol), 0, std::move(v)) {
    nelectrons() = sum_z(molecule());
}

ChemicalSystem::ChemicalSystem(molecule_t mol, size_type nelectrons, epot_t v) :
  m_pimpl_(
    std::make_unique<pimpl_t>(std::move(mol), nelectrons, std::move(v))) {}

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

size_type& ChemicalSystem::nelectrons() { return pimpl_().nelectrons(); }

size_type ChemicalSystem::nelectrons() const { return pimpl_().nelectrons(); }

typename ChemicalSystem::charge_type ChemicalSystem::charge() const noexcept {
    if(!m_pimpl_) return charge_type{0};
    std::size_t neutral = 0;
    const auto nes      = nelectrons();
    for(const auto& atomi : molecule()) neutral += atomi.Z();
    if(neutral < nes) return -1 * charge_type(nes - neutral);
    return charge_type(neutral - nes);
}

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

void ChemicalSystem::hash(detail_::Hasher& h) const { h(m_pimpl_); }

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

} // namespace chemist
