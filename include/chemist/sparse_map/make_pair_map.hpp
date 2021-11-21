#pragma once
#include "chemist/sparse_map/sparse_map/sparse_map.hpp"

namespace chemist::sparse_map {

/** @brief Wraps the creation of a sparse map from a pair of independent indices
 *         to the union of the dependent indices.
 *
 *  Given a sparse map @f$L@f$, which goes from @f$i@f$ to @f$a@f$, and a
 *  sparse map @f$M@f$, which goes from @f$i@f$ to @f$i@f$, this function will
 *  make a new sparse map which goes from @f$ij@f$ to @f$a@f$. Only pairs
 *  consistent with @f$M@f$ are made.
 *
 *  This function essentially does a direct product on the independent indices
 *  and a union on the dependent indices, where the direct product is sparse.
 *
 *  @tparam IndIndexType The type of the independent indices in both sparse
 *                       maps. Expected to be either TileIndex or ElementIndex.
 *  @tparam DepIndexType The type of the dependent indices in @p L_ia. Expected
 *                       to be either TileIndex or ElementIndex.
 *
 *  @param[in] L_ia The sparse map we are turning into a pair map.
 *  @param[in] L_ij The sparsity relationship among the independent indices of
 *                  @p L_ia.
 *
 *  @return The sparse map resulting from the direct product of the independent
 *          indices in @p L_ia (subject to the sparsity relationship @p L_ij)
 *          and the union of the dependent indices.
 *
 *  @throw std::out_of_range if an independent or dependent index of @p L_ij is
 *                           not in @p L_ia. Strong throw guarantee.
 *  @throw std::runtime_error if the indepdendent (or dependent) indices in
 *                            @p L_ij are not of the same rank as the
 *                            independent indices in @p L_ia. Strong throw
 *                            guarantee.
 *  @throw std::runtime_error if the independent rank of @p L_ia is not 1.
 *                            Strong throw guarantee.
 */
template<typename IndIndexType, typename DepIndexType>
auto make_pair_map(const SparseMap<IndIndexType, DepIndexType>& L_ia,
                   const SparseMap<IndIndexType, IndIndexType>& L_ij) {
    if(L_ia.ind_rank() != L_ij.ind_rank()) {
        std::string msg = "Independent rank of L_ia (" +
                          std::to_string(L_ia.ind_rank()) + ") does not match" +
                          " the independent rank of L_ij (" +
                          std::to_string(L_ij.ind_rank()) + ").";
        throw std::runtime_error(msg);
    }

    if(L_ia.ind_rank() != L_ij.dep_rank()) {
        std::string msg = "Independent rank of L_ia (" +
                          std::to_string(L_ia.ind_rank()) + ") does not match" +
                          " the dependent rank of L_ij (" +
                          std::to_string(L_ij.dep_rank()) + ").";
        throw std::runtime_error(msg);
    }

    if(L_ia.ind_rank() != 1)
        throw std::runtime_error("Can only handle rank 1 independent indices");

    SparseMap<IndIndexType, DepIndexType> rv;

    for(const auto& [i, domain_i] : L_ij) {
        for(const auto& j : domain_i) {
            IndIndexType ij{i[0], j[0]};
            for(const auto& a : L_ia.at(i)) rv.add_to_domain(ij, a);
            for(const auto& a : L_ia.at(j)) rv.add_to_domain(ij, a);
        }
    }
    return rv;
}

} // namespace chemist::sparse_map
