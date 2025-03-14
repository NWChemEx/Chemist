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

#include "detail_/molecule_pimpl.hpp"
#include <chemist/molecule/molecule_class.hpp>
#include <iostream> //For std::endl

namespace chemist {

// Unpack types
using reference              = typename Molecule::reference;
using const_reference        = typename Molecule::const_reference;
using pimpl_pointer          = typename Molecule::pimpl_pointer;
using nuclei_reference       = typename Molecule::nuclei_reference;
using const_nuclei_reference = typename Molecule::const_nuclei_reference;
using size_type              = typename Molecule::size_type;
using charge_type            = typename Molecule::charge_type;

// -- CTors --------------------------------------------------------------------

Molecule::Molecule() noexcept : m_pimpl_(nullptr) {}

Molecule::Molecule(const Molecule& rhs) :
  m_pimpl_(rhs.has_pimpl_() ? std::make_unique<pimpl_type>(*rhs.m_pimpl_) :
                              nullptr) {}

Molecule::Molecule(Molecule&& rhs) noexcept = default;

Molecule& Molecule::operator=(const Molecule& rhs) {
    Molecule(rhs).m_pimpl_.swap(m_pimpl_);
    return *this;
}

Molecule& Molecule::operator=(Molecule&& rhs) noexcept = default;

Molecule::Molecule(std::initializer_list<atom_type> atoms) :
  m_pimpl_(make_pimpl_()) {
    for(auto ai : atoms) push_back(ai);

    set_multiplicity_();
    set_charge_();
}

Molecule::Molecule(charge_type charge, size_type multiplicity,
                   std::initializer_list<atom_type> atoms) :
  Molecule(atoms) {
    set_charge(charge);
    set_multiplicity(multiplicity);
}

Molecule::Molecule(charge_type charge, size_type multiplicity,
                   nuclei_type nuclei) :
  m_pimpl_(make_pimpl_()) {
    m_pimpl_->nuclei() = std::move(nuclei);
    set_charge(charge);
    set_multiplicity(multiplicity);
}

Molecule::~Molecule() noexcept = default;

// -- Getters/Setters ----------------------------------------------------------

nuclei_reference Molecule::nuclei() {
    if(!has_pimpl_()) m_pimpl_ = make_pimpl_();
    return nuclei_reference(m_pimpl_->nuclei());
}

const_nuclei_reference Molecule::nuclei() const {
    return has_pimpl_() ? std::as_const(*m_pimpl_).nuclei() :
                          const_nuclei_reference{};
}

size_type Molecule::n_electrons() const noexcept {
    if(!has_pimpl_()) return 0;

    size_type n = 0;
    for(const auto& x : *this) n += x.Z();
    if(charge() < 0)
        n += size_type(-1 * charge());
    else if(charge() > 0)
        n -= charge();

    return n;
}

charge_type Molecule::charge() const noexcept {
    return (!has_pimpl_()) ? 0 : m_pimpl_->m_charge;
}

void Molecule::set_charge(charge_type n) {
    if(!has_pimpl_()) m_pimpl_ = make_pimpl_();
    if(charge_type(size()) < n)
        throw std::out_of_range("Did you forget to add atoms?");
    m_pimpl_->m_charge = n;
    set_multiplicity_();
}

typename Molecule::charge_pointer Molecule::charge_data() noexcept {
    return has_pimpl_() ? &(m_pimpl_->m_charge) : nullptr;
}

typename Molecule::const_charge_pointer Molecule::charge_data() const noexcept {
    return has_pimpl_() ? &(std::as_const(*m_pimpl_).m_charge) : nullptr;
}

typename Molecule::multiplicity_type Molecule::multiplicity() const noexcept {
    return (has_pimpl_()) ? m_pimpl_->m_mult : 1;
}

void Molecule::set_multiplicity(multiplicity_type mult) {
    if(!has_pimpl_()) m_pimpl_ = make_pimpl_();
    m_pimpl_->m_mult = mult;
}

typename Molecule::multiplicity_pointer Molecule::multiplicity_data() noexcept {
    return has_pimpl_() ? &(m_pimpl_->m_mult) : nullptr;
}

typename Molecule::const_multiplicity_pointer Molecule::multiplicity_data()
  const noexcept {
    return has_pimpl_() ? &(std::as_const(*m_pimpl_).m_mult) : nullptr;
}

void Molecule::push_back(atom_type atom) {
    if(!has_pimpl_()) m_pimpl_ = make_pimpl_();
    m_pimpl_->nuclei().push_back(atom.nucleus());

    set_multiplicity_();
    set_charge_();
}

// -- Utility methods ----------------------------------------------------------

void Molecule::swap(Molecule& other) noexcept { m_pimpl_.swap(other.m_pimpl_); }

// -- Private methods ----------------------------------------------------------

reference Molecule::at_(size_type i) noexcept { return m_pimpl_->nuclei()[i]; }

const_reference Molecule::at_(size_type i) const noexcept {
    return std::as_const(*m_pimpl_).nuclei()[i];
}

size_type Molecule::size_() const noexcept {
    if(!has_pimpl_()) return 0;
    return m_pimpl_->nuclei().size();
}

pimpl_pointer Molecule::make_pimpl_() { return std::make_unique<pimpl_type>(); }

bool Molecule::has_pimpl_() const noexcept { return m_pimpl_ != nullptr; }

void Molecule::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("Molecule has no PIMPL. Was it default "
                             "constructed or moved from?");
}

void Molecule::set_multiplicity_() {
    // if n_electrons is odd, we can't have a multiplicity of 1, so set it to 2
    set_multiplicity(n_electrons() % 2 ? 2 : 1);
}

void Molecule::set_charge_() {
    // TODO: adjust electrons when Atom lets you set the number of electrons
}

// -- Free functions -----------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const chemist::Molecule& mol) {
    using atom_t = typename chemist::Molecule::atom_type;
    for(const auto& n : mol) {
        atom_t ai(n.name(), n.Z(), n.mass(), n.x(), n.y(), n.z(), n.charge());
        os << ai << std::endl;
    }
    return os;
}

bool operator==(const Molecule& lhs, const Molecule& rhs) noexcept {
    // Check for equivalent sizes before iterating through all atoms
    if(lhs.size() != rhs.size()) return false;
    if(!lhs.size()) return true; // Both empty
    if(lhs.charge() != rhs.charge()) return false;
    if(lhs.multiplicity() != rhs.multiplicity()) return false;

    // LLVM Clang doesn't like this at the moment.
    // TODO: figure out why Clang doesn't work with this.
    // return std::equal(lhs.begin(), lhs.end(), rhs.begin());

    for(size_type i = 0; i < lhs.size(); ++i) {
        if(lhs[i] != rhs[i]) return false;
    }
    return true;
}

} // namespace chemist
