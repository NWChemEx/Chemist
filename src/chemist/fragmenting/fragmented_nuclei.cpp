/*
 * Copyright 2024 NWChemEx-Project
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

#include <chemist/fragmenting/fragmented_nuclei.hpp>

namespace chemist::fragmenting {
namespace detail_ {

class FragmentedNucleiPIMPL {
public:
    /// Type *this implements
    using parent_type = FragmentedNuclei;

    /// How we store the fragments
    using fragment_map_type = typename parent_type::fragment_map_type;

    /// Type holding the caps
    using cap_set_type = typename parent_type::cap_set_type;

    /// Type used for indexing and offsets
    using size_type = typename parent_type::size_type;

    FragmentedNucleiPIMPL(fragment_map_type frags, cap_set_type caps) :
      m_frags_(std::move(frags)), m_caps_(std::move(caps)) {}

    auto& frag(size_type i) { return m_frags_[i]; }

    const auto& frag(size_type i) const { return m_frags_[i]; }

private:
    fragment_map_type m_frags_;

    cap_set_type m_caps_;
};

} // namespace detail_

// -- CTors, assignment, and dtor

FragmentedNuclei::FragmentedNuclei() noexcept = default;

FragmentedNuclei::FragmentedNuclei(supersystem_type supersystem) :
  FragmentedNuclei(std::move(supersystem), fragment_map_type{}) {}

FragmentedNuclei::FragmentedNuclei(supersystem_type supersystem,
                                   fragment_map_type frags) :
  FragmentedNuclei(std::move(supersystem), std::move(frags), cap_set_type{}) {}

FragmentedNuclei::FragmentedNuclei(supersystem_type supersystem,
                                   fragment_map_type frags, cap_set_type caps) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(frags), std::move(caps))),
  base_type(std::move(supersystem)) {}

// -- protected and private members

FragmentedNuclei::reference FragmentedNuclei::at_(size_type i) {
    return reference(supersystem(), m_pimpl_->frag(i));
}

FragmentedNuclei::const_reference FragmentedNuclei::at_(size_type i) const {
    return const_reference(supersystem(), std : as_const(*m_pimpl_).frag(i));
}

FragmentedNuclei::size_type FragmentedNuclei::size_() const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

} // namespace chemist::fragmenting
