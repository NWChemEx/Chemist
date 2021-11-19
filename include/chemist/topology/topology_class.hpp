#pragma once
#include "chemist/topology/distance.hpp"
#include <array>
#include <memory>

namespace chemist::topology {
namespace detail_ {
class TopologyPIMPL;
}

/** @brief Class which holds the topological description of a series of points.
 *
 *
 */
class Topology {
public:
    /// Type of the PIMPL
    using pimpl_type = detail_::TopologyPIMPL;
    /// Type of a pointer to the PIMPL
    using pimpl_ptr = std::unique_ptr<pimpl_type>;
    /// Type used for indexing and offsets
    using size_type = std::size_t;
    /// Type used for pairs of indices
    using pair_type = std::array<size_type, 2>;
    /// Type used for triples of indices
    using triple_type = std::array<size_type, 3>;
    /// Type used for quadruples of inidices
    using quadruple_type = std::array<size_type, 4>;
    /// Type used for storing distances
    using distance_type = Distance;

    Topology() noexcept;

    /// Default no-throw dtor
    ~Topology() noexcept;

private:
    /** @brief Wraps retrieving the PIMPL in a read/write state.
     *
     *  This function is a convenience function for creating a PIMPL (if the
     *  instance does not already have one) and returning the PIMPL.
     *
     *  @return The instance's PIMPL in a read/write state.
     *  @throw std::bad_alloc if the class has no PIMPL and allocating a PIMPL
     *                        fails. Strong throw guarantee.
     */
    pimpl_type& pimpl_();

    /// Holds the actual state and basic implementation of the Topology class
    pimpl_ptr m_pimpl_;
};

} // namespace chemist::topology