#pragma once
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/detail_/domain_base.hpp"

#include <tiledarray.h>


namespace libchemist::sparse_map {

/** @brief A set of element coordinate indices.
 *
 *  Practically speaking a domain collects the dependent element indices
 *  associated with a particular independent element index. The SparseMap class
 *  manages the mapping of independent element indices to Domain instances.
 *
 */
template<typename IndexType> class Domain;

template<>
class Domain<ElementIndex> :
  public detail_::DomainBase<Domain<ElementIndex>, ElementIndex> {
private:
    using my_type     = Domain<ElementIndex>;
    using base_type   = detail_::DomainBase<my_type, ElementIndex>;
    using traits_type = detail_::DomainTraits<ElementIndex>;
public:
    /// The type the indices are stored as
    using value_type = typename traits_type::value_type;
    //Pull in base class's ctors
    using detail_::DomainBase<Domain<ElementIndex>, ElementIndex>::DomainBase;

}; // class Domain<IndexType>

template<>
class Domain<TileIndex> :
  public detail_::DomainBase<Domain<TileIndex>, TileIndex> {
private:
    using my_type     = Domain<TileIndex>;
    using base_type   = detail_::DomainBase<my_type, TileIndex>;
    using traits_type = detail_::DomainTraits<TileIndex>;
public:
    /// The type the indices are stored as
    using value_type = typename traits_type::value_type;
    // Pull in base class's ctors
    using detail_::DomainBase<Domain<TileIndex>, TileIndex>::DomainBase;
}; // class Domain<TileIndex>




/// Convenience typedef of a Domain filled with ElementIndex instances
using ElementDomain = Domain<ElementIndex>;

/// Convenience typedef of a Domain filled with TileIndex instances
using TileDomain    = Domain<TileIndex>;


//------------------------------------------------------------------------------
//             Forward Declaration of Template Instantiations
//------------------------------------------------------------------------------

extern template class Domain<ElementIndex>;
extern template class Domain<TileIndex>;

} // namespace libchemist::sparse_map
