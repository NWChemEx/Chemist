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

#include "chemist/managers/molecule_manager.hpp"
#include "detail_/molecule_manager_pimpl.hpp"

namespace chemist {

using key_type   = typename MoleculeManager::key_type;
using value_type = typename MoleculeManager::value_type;

MoleculeManager::MoleculeManager() : m_pimpl_() {}

MoleculeManager::MoleculeManager(const MoleculeManager& rhs) :
  m_pimpl_(rhs.pimpl_().clone()) {}

MoleculeManager::MoleculeManager(MoleculeManager&& rhs) noexcept :
  m_pimpl_(std::move(rhs.m_pimpl_)) {}

MoleculeManager& MoleculeManager::operator=(const MoleculeManager& rhs) {
    return *this = MoleculeManager(rhs);
}

MoleculeManager& MoleculeManager::operator=(MoleculeManager&& rhs) noexcept {
    m_pimpl_ = std::move(rhs.m_pimpl_);

    return *this;
}

MoleculeManager::~MoleculeManager() noexcept = default;

value_type MoleculeManager::at(const key_type& name) const {
    return pimpl_().at(name);
}

void MoleculeManager::insert(const key_type& name, const value_type& molecule) {
    pimpl_().insert(name, molecule);
}

bool MoleculeManager::operator==(const MoleculeManager& rhs) const {
    return pimpl_() == rhs.pimpl_();
}

bool MoleculeManager::operator!=(const MoleculeManager& rhs) const {
    return !(*this == rhs);
}

detail_::MoleculeManagerPIMPL& MoleculeManager::pimpl_() {
    // Create a new pimpl if one does not exist
    if(!this->m_pimpl_) {
        this->m_pimpl_ = std::make_unique<detail_::MoleculeManagerPIMPL>();
    }

    return (*this->m_pimpl_);
}

const detail_::MoleculeManagerPIMPL& MoleculeManager::pimpl_() const {
    // Throw if pimpl does not exist
    if(!this->m_pimpl_) { throw std::runtime_error("PIMPL is not set"); }

    return (*this->m_pimpl_);
}

} // namespace chemist
