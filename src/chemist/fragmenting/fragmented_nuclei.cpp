/*
 * Copyright 2023 NWChemEx-Project
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

#include "../chemical_system/nucleus/detail_/nuclei_subset.hpp"
#include <boost/container/flat_set.hpp>
#include <chemist/fragmenting/fragmented_nuclei.hpp>
#include <utility>

namespace chemist::fragmenting {
namespace detail_ {

class FragmentedNucleiPIMPL {
public:
    // Type of the class *this implements
    using parent_type = FragmentedNuclei;

    // Type of a view of a fragment
    using reference = parent_type::reference;

    // Type of the container of fragments
    using fragment_container_type = std::vector<reference>;

    // Type of a pointer to the FragmentedNuclei PIMPL
    using pimpl_pointer = parent_type::pimpl_pointer;

    // Type of a pointer to a fragment's PIMPL
    using fragment_pimpl_pointer = parent_type::fragment_pimpl_pointer;

    // The actual fragments
    fragment_container_type m_fragments;

    void insert(fragment_pimpl_pointer pimpl) {
        m_fragments.emplace_back(reference(std::move(pimpl)));
    }

    pimpl_pointer clone() const {
        return std::make_unique<FragmentedNucleiPIMPL>(*this);
    }
};

} // namespace detail_

// -----------------------------------------------------------------------------
// --- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------
FragmentedNuclei::FragmentedNuclei() noexcept = default;

FragmentedNuclei::FragmentedNuclei(value_type supersystem) noexcept :
  base_type(std::move(supersystem)) {}

FragmentedNuclei::FragmentedNuclei(const FragmentedNuclei& other) :
  base_type(other),
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

FragmentedNuclei& FragmentedNuclei::operator=(const FragmentedNuclei& rhs) {
    if(this != &rhs) FragmentedNuclei(rhs).swap(*this);
    return *this;
}

FragmentedNuclei::FragmentedNuclei(FragmentedNuclei&& other) noexcept = default;

FragmentedNuclei& FragmentedNuclei::operator=(
  FragmentedNuclei&& other) noexcept = default;

FragmentedNuclei::~FragmentedNuclei() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters/Setters
// -----------------------------------------------------------------------------

void FragmentedNuclei::add_fragment(std::initializer_list<index_type> il) {
    make_pimpl_();
    auto psupersystem = supersystem_data();
    auto begin_itr    = il.begin();
    auto end_itr      = il.end();
    using pimpl_t     = chemist::detail_::NucleiSubset<value_type>;
    m_pimpl_->insert(
      std::make_unique<pimpl_t>(psupersystem, begin_itr, end_itr));
}

// -----------------------------------------------------------------------------
// --- Utility methods
// -----------------------------------------------------------------------------

void FragmentedNuclei::swap(FragmentedNuclei& other) noexcept {
    base_type::swap(other);
    m_pimpl_.swap(other.m_pimpl_);
}

bool FragmentedNuclei::operator==(const FragmentedNuclei& rhs) const noexcept {
    if(!base_type::operator==(rhs)) return false;
    if(size() != rhs.size()) return false;
    if(size() == 0) return true;
    return m_pimpl_->m_fragments == rhs.m_pimpl_->m_fragments;
}

// -----------------------------------------------------------------------------
// --- Private methods
// -----------------------------------------------------------------------------

bool FragmentedNuclei::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

void FragmentedNuclei::make_pimpl_() {
    if(has_pimpl_()) return;
    m_pimpl_ = std::make_unique<pimpl_type>();
}

// n.b., IndexableContainerBase implements bounds checking, i.e., these wont'
// be called unless i is a valid index (implying we have a PIMPL too)
FragmentedNuclei::reference FragmentedNuclei::at_(size_type i) noexcept {
    return m_pimpl_->m_fragments[i];
}

FragmentedNuclei::const_reference FragmentedNuclei::at_(
  size_type i) const noexcept {
    return m_pimpl_->m_fragments[i];
}

FragmentedNuclei::size_type FragmentedNuclei::size_() const noexcept {
    if(!has_pimpl_()) return 0;
    return m_pimpl_->m_fragments.size();
}

} // namespace chemist::fragmenting
