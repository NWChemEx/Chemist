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

#pragma once
#include <chemist/nucleus/fragmented_nuclei.hpp>

namespace chemist::detail_ {

class FragmentedNucleiPIMPL {
public:
    using parent_type = FragmentedNuclei;

    using supersystem_type = parent_type::supersystem_type;

    using const_supersystem_pointer = std::shared_ptr<const supersystem_type>;

    using size_type = supersystem_type::size_type;

    explicit FragmentedNucleiPIMPL(const_supserystem_pointer supersystem) :
      m_supersystem_(supersystem) {}

private:
    const_supserystem_pointer m_supersystem_;

    std::vector<std::set<size_type>> m_frags_;
};

} // namespace chemist::detail_