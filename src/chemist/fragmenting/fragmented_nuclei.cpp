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

#include <boost/container/flat_set.hpp>
#include <chemist/fragmenting/fragmented_nuclei.hpp>

namespace {

template<typename... Args>
auto make_pimpl(Args&&... args) {
    using pimpl_type = chemist::fragmenting::detail_::FragmentedNucleiPIMPL;
    return std::make_unique<pimpl_type>(std::forward<Args>(args)...)
}

} // namespace

namespace chemist::fragmenting {
namespace detail_ {

class FragmentedNucleiPIMPL {
public:
    // Type used to index nuclei
    using index_type = FragmentedNuclei::index_type;

    // Type of each fragment
    using index_set_type = boost::container::flat_set<index_type>;

    // Type of the container of fragments
    using fragment_container_type = std::vector<index_set_type>;

    // Type of a pointer to a PIMPL
    using pimpl_pointer = FragmentedNuclei::pimpl_pointer;

    // The actual fragments
    fragment_container_type m_fragments;

    pimpl_pointer clone() const { return make_pimpl(*this); }
};

} // namespace detail_

// -----------------------------------------------------------------------------
// --- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------
FragmentedNuclei::FragmentedNuclei() noexcept = default;

FragmentedNuclei::FragmentedNuclei(supersystem_type supersystem) :
  base_type(std::move(supersystem)), m_pimpl_(make_pimpl()) {}

FragmentedNuclei::FragmentedNuclei(const FragmentedNuclei& other) :
  base_type(other),
  m_pimpl_(other.has_pimpl() ? other.m_pimpl_->clone() : nullptr) {}

FragmentedNuclei& FragmentedNuclei::operator=(const FragmentedNuclei& other) {
    if(this != &rhs) FragmentedNuclei(other).swap(*this);
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
    if(!has_pimpl()) m_pimpl_.swap(make_pimpl());
    pimpl_type::index_set temp(il.begin(), il.end());
    m_pimpl_.m_fragments.emplace_back(std::move(temp));
}

// -----------------------------------------------------------------------------
// --- Utility methods
// -----------------------------------------------------------------------------

void FragmentedNuclei::swap(FragmentedNuclei& other) noexcept {
    base_type::swap(other);
    m_pimpl_.swap(other.m_pimpl_);
}

// -----------------------------------------------------------------------------
// --- Private methods
// -----------------------------------------------------------------------------

} // namespace chemist::fragmenting
