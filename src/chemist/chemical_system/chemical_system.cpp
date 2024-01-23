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

#include "chemical_system_pimpl.hpp"
#include <stdexcept>

namespace chemist {

using pimpl_t    = typename ChemicalSystem::pimpl_t;
using molecule_t = typename ChemicalSystem::molecule_t;
using size_type  = typename ChemicalSystem::size_type;

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

bool ChemicalSystem::operator==(const ChemicalSystem& rhs) const noexcept {
    if(!m_pimpl_ && !rhs.m_pimpl_)
        return true;
    else if(!m_pimpl_ || !rhs.m_pimpl_)
        return false;
    return (*m_pimpl_) == (*rhs.m_pimpl_);
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

} // namespace chemist
