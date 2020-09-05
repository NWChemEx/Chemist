#pragma once
#include <bphash/Hasher.hpp>
#include <tiledarray.h>

namespace TiledArray {
//------------------------------------------------------------------------------
// Hashing for TiledArray Classes
//------------------------------------------------------------------------------

/** @brief Enables hashing for TA Range class.
 *
 * Free function to enable hashing with BPHash library.
 *
 * @param[in] R Range object
 * @param[in, out] h bphash::Hasher object.
 */
inline void hash_object(const TA::Range& R, bphash::Hasher& h) {
    const char* mytype = "TA::Range";
    h(mytype); // To create a unique hash for this type with the default
               // constructor
    //  h(typeid(tr).hash_code()); // Alternative to above but no guarantee for
    //  reproducibility
    for(std::size_t i = 0; i < R.rank(); ++i) {
        h(R.dim(i).first);
        h(R.dim(i).second);
    }
}

/** @brief Enables hashing for TA TiledRange1 class.
 *
 * Free function to enable hashing with BPHash library.
 *
 * @param[in] tr TiledRange1 object
 * @param[in, out] h bphash::Hasher object.
 */
inline void hash_object(const TA::TiledRange1& tr, bphash::Hasher& h) {
    const char* mytype = "TA::TiledRange1";
    h(mytype);
    for(const auto& tile : tr) h(tile.first);
    h(tr.extent());
}

/** @brief Enables hashing for TA TiledRange class.
 *
 * Free function to enable hashing with BPHash library.
 *
 * @param[in] tr TiledRange object
 * @param[in, out] h bphash::Hasher object.
 */
inline void hash_object(const TiledArray::TiledRange& tr, bphash::Hasher& h) {
    const char* mytype = "TA::TiledRange";
    h(mytype);
    for(std::size_t i = 0; i < tr.rank(); ++i) h(tr.dim(i));
}

/** @brief Enables hashing for TA Pmap class.
 *
 * Free function to enable hashing with BPHash library.
 *
 * @param[in] p Pmap object
 * @param[in, out] h bphash::Hasher object.
 */
inline void hash_object(const TiledArray::Pmap& p, bphash::Hasher& h) {
    const char* mytype = "TA::Pmap";
    h(mytype);
    h(p.rank());
    h(p.size());
}

/** @brief Enables hashing for TA Tensor class.
 *
 * Free function to enable hashing with BPHash library.
 *
 * @tparam ValueType Type of value for the @p A tensor.
 * @tparam AllocatorType Type of allocator for the @p A tensor.
 * @param[in] A Tensor object
 * @param[in, out] h bphash::Hasher object.
 */
template<typename ValueType, typename AllocatorType>
void hash_object(const TA::Tensor<ValueType, AllocatorType>& A,
                 bphash::Hasher& h) {
    const char* mytype    = "TA::Tensor";
    ValueType myvaluetype; // Relying on type casting 1 into ValueType
    h(mytype, myvaluetype);
    h(A.range());
    const auto n = A.range().volume();
    for(auto i = 0ul; i < n; ++i) h(A[i]);
}

/** @brief Enables hashing for TA DistArray class.
 *
 * Free function to enable hashing with BPHash library.
 *
 * @tparam TileType Type of tiles in @p A.
 * @tparam PolicyType Type of policy for @p A. Either DensePolicy or
 * SparsePolicy.
 * @param[in] A DistArray object
 * @param[in, out] h bphash::Hasher object.
 */
template<typename TileType, typename PolicyType>
// void hash_object(const TA::DistArray<TileType, PolicyType>& A,
void hash_object(
  const TA::DistArray<TA::Tensor<TileType, Eigen::aligned_allocator<TileType>>,
                      PolicyType>& A,
  bphash::Hasher& h) {
    const char* mytype = "TA::DistArray";
    TileType mytiletype; // Relying on type casting for int,long,float,double
    h(mytype, mytiletype);
    h(A.range());
    h(A.pmap().get());
    for(auto it = A.begin(); it != A.end(); ++it) {
        auto tile = A.find(it.index()).get();
        h(tile);
    }
}
} // namespace TiledArray
