#pragma once
#include "chemist/topology/topology.hpp"

namespace chemist::topology::detail_ {

struct TopologyPIMPL {
    using pair_type     = typename Topology::pair_type;
    using distance_type = typename Topology::distance_type;

    std::map<pair_type, distance_type> m_distances;
};

} // namespace chemist::topology::detail_