#include "libchemist/basis_set_manager.hpp"
#include "libchemist/detail_/basis_set_manager_pimpl.hpp"

namespace libchemist {

using size_type = std::size_t;

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

AOBasisSet BasisSetManager::get_basis(const std::string& name,
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
