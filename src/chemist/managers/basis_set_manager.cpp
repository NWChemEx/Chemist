#include "chemist/managers/basis_set_manager.hpp"
#include "detail_/basis_set_manager_pimpl.hpp"

namespace chemist {

using ao_basis_type = typename BasisSetManager::ao_basis_type;
using size_type     = typename BasisSetManager::size_type;
using ao_basis_map  = typename BasisSetManager::ao_basis_map;

BasisSetManager::BasisSetManager() : m_pimpl_() {}

BasisSetManager::BasisSetManager(const BasisSetManager& rhs) :
  m_pimpl_(rhs.pimpl_().clone()) {}

BasisSetManager::BasisSetManager(BasisSetManager&& rhs) noexcept :
  m_pimpl_(std::move(rhs.m_pimpl_)) {}

BasisSetManager& BasisSetManager::operator=(const BasisSetManager& rhs) {
    return *this = BasisSetManager(rhs);
}

BasisSetManager& BasisSetManager::operator=(BasisSetManager&& rhs) noexcept {
    m_pimpl_ = std::move(rhs.m_pimpl_);

    return *this;
}

BasisSetManager::~BasisSetManager() noexcept = default;

ao_basis_type BasisSetManager::get_basis(const std::string& name,
                                         const size_type& Z) const {
    return pimpl_().get_basis(name, Z);
}

void BasisSetManager::insert(const std::string& name,
                             const ao_basis_map& basis_set) {
    pimpl_().insert(name, basis_set);
}

detail_::BasisSetManagerPIMPL& BasisSetManager::pimpl_() {
    // Create a new pimpl if one does not exist
    if(!this->m_pimpl_) {
        this->m_pimpl_ = std::make_unique<detail_::BasisSetManagerPIMPL>();
    }

    return (*this->m_pimpl_);
}

const detail_::BasisSetManagerPIMPL& BasisSetManager::pimpl_() const {
    // Throw if pimpl does not exist
    if(!this->m_pimpl_) { throw std::runtime_error("PIMPL is not set"); }

    return (*this->m_pimpl_);
}

bool BasisSetManager::operator==(const BasisSetManager& rhs) const {
    return pimpl_() == rhs.pimpl_();
}

bool BasisSetManager::operator!=(const BasisSetManager& rhs) const {
    return !((*this) == rhs);
}

size_type l_converter(char l) {
    l = ::tolower(l);
    const std::string shells("spdfghijklmnoqrtuvwxyzabce");
    auto pos = shells.find(l);
    if(pos == std::string::npos)
        throw std::logic_error("l is not a member of the English alphabet");
    return pos;
}

} // namespace chemist
