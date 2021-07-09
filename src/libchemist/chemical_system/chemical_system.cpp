#include "chemical_system_pimpl.hpp"
#include <stdexcept>

namespace libchemist {

using pimpl_t    = typename ChemicalSystem::pimpl_t;
using molecule_t = typename ChemicalSystem::molecule_t;
using charge_type = typename ChemicalSystem::charge_type;
using mult_type = typename ChemicalSystem::multiplicity_type;
using epot_t     = typename ChemicalSystem::epot_t;

ChemicalSystem::ChemicalSystem() : m_pimpl_(std::make_unique<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(const ChemicalSystem& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() :
                            std::unique_ptr<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(ChemicalSystem&& other) noexcept = default;

ChemicalSystem::ChemicalSystem(molecule_t mol,
                               mult_type mult,
                               charge_type charge,
                               epot_t v) :
  m_pimpl_(std::make_unique<pimpl_t>(std::move(mol), mult, charge, std::move(v))) {}

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

mult_type& ChemicalSystem::multiplicity() { return pimpl_().multiplicity(); }

mult_type ChemicalSystem::multiplicity() const { return pimpl_().multiplicity(); }

charge_type& ChemicalSystem::charge() { return pimpl_().charge(); }

charge_type ChemicalSystem::charge() const { return pimpl_().charge(); }

// size_type Molecule::nelectrons() const noexcept {
//     size_type n = 0;
//     for(const auto& atom : *this) n += atom.Z();
//     return n - std::lround(charge());
// }
// size_type Molecule::nalpha() const noexcept {
//     const size_type nopen   = multiplicity() - 1;
//     const size_type nclosed = nelectrons() - nopen;
//     return nclosed / 2 + nopen;
// }
// size_type Molecule::nbeta() const noexcept {
//     const size_type nopen   = multiplicity() - 1;
//     const size_type nclosed = nelectrons() - nopen;
//     return nclosed / 2;
// }


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
