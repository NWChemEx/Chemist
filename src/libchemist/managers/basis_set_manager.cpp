#include "libchemist/managers/basis_set_manager.hpp"
#include "libchemist/managers/detail_/basis_set_manager_pimpl.hpp"

namespace libchemist {

using ao_basis_type = typename BasisSetManager::ao_basis_type;
using size_type     = typename BasisSetManager::size_type;

BasisSetManager::BasisSetManager() : pimpl_(detail_::nwx_default_bs()) {}
BasisSetManager::BasisSetManager(const BasisSetManager& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}
BasisSetManager::BasisSetManager(BasisSetManager&& rhs) noexcept = default;
BasisSetManager& BasisSetManager::operator=(const BasisSetManager& rhs) {
    return *this = std::move(BasisSetManager(rhs));
}
BasisSetManager& BasisSetManager::operator=(BasisSetManager&& rhs) noexcept =
  default;
BasisSetManager::~BasisSetManager() noexcept = default;

ao_basis_type BasisSetManager::get_basis(const std::string& name,
                                         size_type Z) const {
    return pimpl_->get_basis(name, Z);
}

size_type l_converter(char l) {
    l = ::tolower(l);
    const std::string shells("spdfghijklmnoqrtuvwxyzabce");
    auto pos = shells.find(l);
    if(pos == std::string::npos)
        throw std::logic_error("l is not a member of the English alphabet");
    return pos;
}

} // namespace libchemist
