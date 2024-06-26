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

#include "chemist/topology/connectivity_table.hpp"
#include "connectivity_table_pimpl.hpp"

namespace chemist::topology {

using pimpl_type       = typename ConnectivityTable::pimpl_type;
using offset_pair_list = typename ConnectivityTable::offset_pair_list;
using size_type        = typename ConnectivityTable::size_type;

//---------------------------------- CTors -------------------------------------

ConnectivityTable::ConnectivityTable() noexcept = default;

ConnectivityTable::ConnectivityTable(size_type natoms) :
  m_pimpl_(std::make_unique<pimpl_type>(natoms)) {}

ConnectivityTable::ConnectivityTable(const ConnectivityTable& other) :
  m_pimpl_(std::make_unique<pimpl_type>(other.m_pimpl_ ? *other.m_pimpl_ :
                                                         pimpl_type{})) {}

ConnectivityTable::ConnectivityTable(ConnectivityTable&& other) noexcept =
  default;

ConnectivityTable& ConnectivityTable::operator=(const ConnectivityTable& rhs) {
    if(this != &rhs) ConnectivityTable(rhs).swap(*this);
    return *this;
}

ConnectivityTable& ConnectivityTable::operator=(
  ConnectivityTable&& rhs) noexcept = default;

ConnectivityTable::~ConnectivityTable() noexcept = default;

void ConnectivityTable::swap(ConnectivityTable& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

void ConnectivityTable::set_n_atoms(size_type natoms) {
    pimpl_().set_natoms(natoms);
}

void ConnectivityTable::add_bond(size_type i, size_type j) {
    pimpl_().add_bond(i, j);
}

size_type ConnectivityTable::natoms() const noexcept {
    if(m_pimpl_) return m_pimpl_->natoms();
    return 0;
}

size_type ConnectivityTable::nbonds() const noexcept {
    if(m_pimpl_) return m_pimpl_->nbonds();
    return 0;
}

offset_pair_list ConnectivityTable::bonds() const {
    if(m_pimpl_) return m_pimpl_->bonds();
    return offset_pair_list{};
}

typename ConnectivityTable::atom_indices_set ConnectivityTable::bonded_atoms(
  size_type i) const {
    const auto n = natoms();
    if(i >= n)
        throw std::out_of_range("i = " + std::to_string(i) +
                                " is not in the range [0, " +
                                std::to_string(n) + ").");
    std::set<size_type> rv;
    const auto& p = *m_pimpl_;
    for(size_type j = 0; j < n; ++j) {
        if(i == j) continue;
        if(p.are_bonded(i, j)) rv.insert(j);
    }
    return rv;
}

pimpl_type& ConnectivityTable::pimpl_() {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<pimpl_type>();
    return *m_pimpl_;
}

bool operator==(const ConnectivityTable& lhs, const ConnectivityTable& rhs) {
    if(lhs.nbonds() != rhs.nbonds()) return false;
    if(lhs.natoms() != rhs.natoms()) return false;
    if(lhs.bonds() != rhs.bonds()) return false;
    return true;
}

} // namespace chemist::topology
