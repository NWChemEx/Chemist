#pragma once
#include <map>

namespace libchemist::sparse_map {

// Forward declare classes needed to specialize
template<typename IndexType> class Domain;
template<typename IndIndex, typename DepIndex> class SparseMap;


namespace detail_ {

/** @brief Primary template for SparseMapTraits.
 *
 *  The primary template for SparseMapTraits is not defined so as to force users
 *  to instantiate specializations for their class.
 *
 *  @tparam T The type of the class the SparseMapTraits is defining the types
 *            for.
 */
template<typename T> struct SparseMapTraits;

/** @brief Partial specialization of SparseMapTraits for SparseMap classes.
 *
 *  @tparam IndIndex The type of the independent indices. Assumed to be either
 *                   ElementIndex or TileIndex.
 *  @tparam DepIndex The type of the dependent indices. Assumed to be either
 *                   ElementIndex or TileIndex.
 */
template<typename IndIndex, typename DepIndex>
struct SparseMapTraits<SparseMap<IndIndex, DepIndex>> {
    /// Type used for counting and offsets
    using size_type   = std::size_t;

    /// Type of the independent indices in the SparseMap
    using key_type    = IndIndex;

    /// Type of the Domains of dependent indices
    using mapped_type = Domain<DepIndex>;

    /// Type of a key-value pair
    using value_type  = std::pair<const key_type, mapped_type>;
private:
    /// Internal type used to store key-value pairs. Needed for iterators.
    using map_t = std::map<key_type, mapped_type>;

public:
    /// Type of a read/write iterator over the SparseMap
    using iterator       = typename map_t::iterator;

    /// Type of a read-only iterator over the SparseMap
    using const_iterator = typename map_t::const_iterator;
}; // struct SparseMapTraits

} // namespace detail_
} // namespace libchemist::sparse_map
