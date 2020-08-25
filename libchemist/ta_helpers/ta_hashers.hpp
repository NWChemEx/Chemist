#pragma once
#include "mpi.h"
#include <bphash/Hasher.hpp>
#include <sde/detail_/memoization.hpp>
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
    const char* mytype = "TA::Tensor";
    h(A.range());
    const auto n = A.range().volume();
    for(auto i = 0ul; i < n; ++i) h(A[i]);
}

/** @brief Returns a combined hash string for TA::DistArray object.
 *
 * Returned string length is equal to number of tiles *
 * length of each hash string (32 chars)
 * We can downsample the strings for performance. (not implemented)
 * Involves all-to-all communication (MPI_Allgather)
 *
 * @tparam TileType Type of tiles in @p A.
 * @tparam PolicyType Type of policy for @p A. Either DensePolicy or
 * SparsePolicy.
 * @param[in] A DistArray object
 * @return std::string representing hash string gathered from all tilles from
 * all ranks
 */
template<typename TileType, typename PolicyType>
std::string get_tile_hash_str(
  const TA::DistArray<TA::Tensor<TileType, Eigen::aligned_allocator<TileType>>,
                      PolicyType>& A) {
    auto& madworld = TA::get_default_world();
    auto& mpiworld = madworld.mpi.comm().Get_mpi_comm();
    int size       = madworld.size();
    int rank       = madworld.rank();
    std::string myhash, totalhash;

    for(auto it = A.begin(); it != A.end(); ++it) {
        auto tile = A.find(it.index()).get();
        myhash += sde::hash_objects(tile);
    }
    int mylen       = myhash.length();
    int* recvcounts = nullptr;
    recvcounts      = new int[size];

    MPI_Allgather(&mylen, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);
    int totlen        = 0;
    int* displs       = nullptr;
    char* totalstring = nullptr;

    displs    = new int[size];
    displs[0] = 0;
    totlen += recvcounts[0] + 1;

    for(int i = 1; i < size; i++) {
        totlen += recvcounts[i];
        displs[i] = displs[i - 1] + recvcounts[i - 1];
    }

    totalstring             = new char[totlen];
    totalstring[totlen - 1] = '\0';

    MPI_Allgatherv(myhash.c_str(), mylen, MPI_CHAR, totalstring, recvcounts,
                   displs, MPI_CHAR, mpiworld);

    totalhash = std::string(totalstring);
    delete[] recvcounts, displs, totalstring;
    return totalhash;
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
void hash_object(
  const TA::DistArray<TA::Tensor<TileType, Eigen::aligned_allocator<TileType>>,
                      PolicyType>& A,
  bphash::Hasher& h) {
    const char* mytype = "TA::DistArray";
    auto& world        = TA::get_default_world();
    h(mytype);
    h(A.range());
    auto hashstr = get_tile_hash_str(A);
    h(hashstr);
}
} // namespace TiledArray