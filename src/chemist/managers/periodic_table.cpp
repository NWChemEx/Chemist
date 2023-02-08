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

#include "chemist/managers/periodic_table.hpp"
#include "detail_/periodic_table_pimpl.hpp"

namespace chemist {

using size_type        = typename PeriodicTable::size_type;
using Z_list           = typename PeriodicTable::Z_list;
using isotope_list     = typename PeriodicTable::isotope_list;
using elec_conf_t      = typename PeriodicTable::elec_conf_t;
using elec_conf_full_t = typename PeriodicTable::elec_conf_full_t;

PeriodicTable::PeriodicTable() : m_pimpl_() {}

PeriodicTable::PeriodicTable(const PeriodicTable& rhs) :
  m_pimpl_(rhs.pimpl_().clone()) {}

PeriodicTable::PeriodicTable(PeriodicTable&& rhs) noexcept :
  m_pimpl_(std::move(rhs.m_pimpl_)) {}

PeriodicTable& PeriodicTable::operator=(const PeriodicTable& rhs) {
    return *this = PeriodicTable(rhs);
}

PeriodicTable& PeriodicTable::operator=(PeriodicTable&& rhs) noexcept {
    m_pimpl_ = std::move(rhs.m_pimpl_);

    return *this;
}

PeriodicTable::~PeriodicTable() noexcept = default;

void PeriodicTable::insert(size_type Z, const Atom& atom) {
    pimpl_().insert(Z, atom);
}

void PeriodicTable::add_isotope(size_type Z, size_type mass_number,
                                const Atom& isotope) {
    pimpl_().add_isotope(Z, mass_number, isotope);
}

void PeriodicTable::add_elec_config(size_type Z,
                                    const elec_conf_t& elec_config) {
    pimpl_().add_elec_config(Z, elec_config);
}

isotope_list PeriodicTable::isotopes(size_type Z) const {
    return pimpl_().isotopes(Z);
}

size_type PeriodicTable::sym_2_Z(const std::string& sym) const {
    return pimpl_().sym_2_Z(sym);
}

elec_conf_full_t PeriodicTable::reduced_2_full_conf(
  const elec_conf_t& r_conf) const {
    return pimpl_().reduced_2_full_conf(r_conf);
}

size_type PeriodicTable::max_Z() const noexcept { return pimpl_().max_Z(); }

Atom PeriodicTable::get_atom(size_type Z) const { return pimpl_().get_atom(Z); }
Atom PeriodicTable::get_isotope(size_type Z, size_type mass_num) const {
    return pimpl_().get_isotope(Z, mass_num);
}

elec_conf_t PeriodicTable::get_elec_conf(size_type Z) const {
    return pimpl_().get_elec_conf(Z);
}

bool PeriodicTable::operator==(const PeriodicTable& rhs) const {
    return pimpl_() == rhs.pimpl_();
}

bool PeriodicTable::operator!=(const PeriodicTable& rhs) const {
    return !(*this == rhs);
}

detail_::PeriodicTablePIMPL& PeriodicTable::pimpl_() {
    // Create a new pimpl if one does not exist
    if(!this->m_pimpl_) {
        this->m_pimpl_ = std::make_unique<detail_::PeriodicTablePIMPL>();
    }

    return (*this->m_pimpl_);
}

const detail_::PeriodicTablePIMPL& PeriodicTable::pimpl_() const {
    // Throw if pimpl does not exist
    if(!this->m_pimpl_) { throw std::runtime_error("PIMPL is not set"); }

    return (*this->m_pimpl_);
}

} // namespace chemist
