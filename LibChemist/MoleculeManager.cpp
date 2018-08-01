#include "LibChemist/Implementations/MoleculeManagerPIMPL.hpp"
#include "LibChemist/MoleculeManager.hpp"

namespace LibChemist {

using key_type   = typename MoleculeManager::key_type;
using value_type = typename MoleculeManager::value_type;

MoleculeManager::MoleculeManager() : pimpl_(detail_::nwx_default_mols()) {}
MoleculeManager::MoleculeManager(const MoleculeManager& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}
MoleculeManager::MoleculeManager(MoleculeManager&& rhs) noexcept = default;
MoleculeManager& MoleculeManager::operator=(const MoleculeManager& rhs) {
    return *this = std::move(MoleculeManager(rhs));
}
MoleculeManager& MoleculeManager::operator=(MoleculeManager&& rhs) noexcept =
  default;
MoleculeManager::~MoleculeManager() noexcept = default;

value_type MoleculeManager::at(const key_type& name) const {
    return pimpl_->at(name);
}

} // namespace LibChemist
