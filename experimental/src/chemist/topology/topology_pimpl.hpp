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

#pragma once
#include "chemist/topology/topology.hpp"
#include <map>

namespace chemist::topology::detail_ {

struct TopologyPIMPL {
    using pair_type     = typename Topology::pair_type;
    using distance_type = typename Topology::distance_type;

    std::map<pair_type, distance_type> m_distances;
};

} // namespace chemist::topology::detail_
