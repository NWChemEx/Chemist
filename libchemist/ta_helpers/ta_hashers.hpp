#pragma once
#include <tiledarray.h>
#include <bphash/Hasher.hpp>

namespace TiledArray
{
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
inline void hash_object(const TA::Range &R, bphash::Hasher &h)
{
    for (std::size_t i = 0; i < R.rank(); ++i)
    {
        h(R.dim(i).first);
        h(R.dim(i).second);
    }
    //h(R.rank());
}

/** @brief Enables hashing for TA TiledRange1 class.
 * 
 * Free function to enable hashing with BPHash library.
 *
 * @param[in] tr TiledRange1 object
 * @param[in, out] h bphash::Hasher object.
 */
inline void hash_object(const TA::TiledRange1 &tr, bphash::Hasher &h)
{
    for (const auto &tile : tr)
        h(tile.first);
    h(tr.extent());
}

/** @brief Enables hashing for TA TiledRange class.
 * 
 * Free function to enable hashing with BPHash library.
 *
 * @param[in] tr TiledRange object
 * @param[in, out] h bphash::Hasher object.
 */
inline void hash_object(const TiledArray::TiledRange &tr, bphash::Hasher &h)
{
    for (std::size_t i = 0; i < tr.rank(); ++i)
        h(tr.dim(i));
}

/** @brief Enables hashing for TA Pmap class.
 * 
 * Free function to enable hashing with BPHash library.
 *
 * @param[in] p Pmap object
 * @param[in, out] h bphash::Hasher object.
 */
inline void hash_object(const TiledArray::Pmap &p, bphash::Hasher &h)
{
    h(p.rank());
    h(p.size());
}

/** @brief Enables hashing for TA Tensor class.
 * 
 * Free function to enable hashing with BPHash library.
 *
 * @tparam t1 Value type for the @p A tensor.
 * @tparam t2 Allocator type for the data of @p A.
 * @param[in] A Tensor object
 * @param[in, out] h bphash::Hasher object.
 */
template <typename t1, typename t2>
void hash_object(const TA::Tensor<t1, t2> &A, bphash::Hasher &h)
{
    h(A.size());
    const auto n = A.range().volume();
    for (auto i = 0ul; i < n; ++i)
        h(A[i]);
}

/** @brief Enables hashing for TA DistArray class.
 * 
 * Free function to enable hashing with BPHash library.
 *
 * @tparam TileType Type of tiles in @p A.
 * @tparam PolicyType Type of policy for @p A. Either DensePolicy or SparsePolicy.
 * @param[in] A DistArray object
 * @param[in, out] h bphash::Hasher object.
 */
template <typename TileType, typename PolicyType>
void hash_object(const TA::DistArray<TileType, PolicyType> &A, bphash::Hasher &h)
{
    h(A.size());
    //h(A.shape();//Error
    h(A.pmap().get());
    for (auto it = A.begin(); it != A.end(); ++it)
    {
        TA::TensorD tile = A.find(it.index()).get();
        h(tile);
    }
}
} // namespace TiledArray