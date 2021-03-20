#include "chemical_system_pimpl.hpp"
#include <stdexcept>

namespace libchemist {

using pimpl_t    = typename ChemicalSystem::pimpl_t;
using molecule_t = typename ChemicalSystem::molecule_t;

ChemicalSystem::ChemicalSystem() : m_pimpl_(std::make_unique<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(const ChemicalSystem& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() :
                            std::unique_ptr<pimpl_t>()) {}

ChemicalSystem::ChemicalSystem(ChemicalSystem&& other) noexcept = default;

ChemicalSystem::ChemicalSystem(molecule_t mol) :
  m_pimpl_(std::make_unique<pimpl_t>(std::move(mol))) {}

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