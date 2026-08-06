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

#include <chemist/fragmenting/capping/cap_set.hpp>

namespace chemist::fragmenting {

typename CapSet::index_set_type CapSet::get_cap_indices(
  const index_set_type& fragment_indices) const {
    index_set_type rv;
    for(size_type i = 0; i < m_caps_.size(); ++i) {
        const auto& cap = m_caps_[i];
        if(cap == Cap{}) continue;
        auto anchor             = cap.get_anchor_index();
        auto replaced           = cap.get_replaced_index();
        const bool has_anchor   = fragment_indices.count(anchor);
        const bool has_replaced = fragment_indices.count(replaced);
        const bool broken_bond  = has_anchor && !has_replaced;
        if(broken_bond) rv.insert(i);
    }
    return rv;
}

template<typename ReturnSetType, typename IndexSetType, typename CapSetType>
auto cap_nuclei_guts_(IndexSetType&& is, CapSetType&& caps) {
    if(caps.size() == 0) return ReturnSetType{};
    auto cap_indices = caps.get_cap_indices(std::forward<IndexSetType>(is));

    // Still No caps? Early abort
    if(cap_indices.empty()) return ReturnSetType{};

    typename ReturnSetType::reference_container cap_nuclei;

    for(auto i : cap_indices) {
        auto&& cap_i = caps[i];
        for(decltype(cap_i.size()) j = 0; j < cap_i.size(); ++j)
            cap_nuclei.push_back(cap_i.at(j));
    }

    return ReturnSetType(std::move(cap_nuclei));
}

typename CapSet::nuclei_reference CapSet::get_cap_nuclei(
  const index_set_type& fragment_indices) {
    return cap_nuclei_guts_<nuclei_reference>(fragment_indices, *this);
}

typename CapSet::const_nuclei_reference CapSet::get_cap_nuclei(
  const index_set_type& fragment_indices) const {
    return cap_nuclei_guts_<const_nuclei_reference>(fragment_indices, *this);
}

} // namespace chemist::fragmenting
