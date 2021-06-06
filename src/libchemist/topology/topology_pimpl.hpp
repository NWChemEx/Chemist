#pragma once
#include "libchemist/topology/topology.hpp"

namespace libchemist::topology::detail_ {

struct TopologyPIMPL {
    using pair_type     = typename Topology::pair_type;
    using distance_type = typename Topology::distance_type;

    std::map<pair_type, distance_type> m_distances;
};

} // namespace libchemist::topology::detail_