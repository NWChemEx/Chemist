#include "libchemist/topology/topology.hpp"
#include "topology_pimpl.hpp"

namespace libchemist::topology {

Topology::Topology() noexcept = default;

Topology::~Topology() noexcept = default;

typename Topology::pimpl_type& Topology::pimpl_() {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<pimpl_type>();
    return *m_pimpl_;
}

} // namespace libchemist::topology