#include <catch2/catch.hpp>
#include <tiledarray.h>

using namespace libchemist;

template<typename InnerTensor, typename Tensor>
auto make_condensed_tensors(const sparse_map::SparseMap& sm, Tensor&& t) {
    using size_type    = typename sparse_map::SparseMap::size_type;
    using key_type     = typename sparse_map::SparseMap::key_type;

    const auto indices = sm.indices();
    const auto& trange = t.trange();

    for(const auto& [ind, idx_set] : indices){
        auto [offsets, r] = condense_tensor_range(idx_set, trange);
        InnerTensor t(r);

        for(const auto& block : sm.at(ind)){
            typename Tensor::value_type tile = t.find(block);

            std::vector<std::pair<size_type, size_type>> dims;
            for(size_type i = 0; i < trange.rank(); ++i)
                dims.push_back(dims[i][block[i]]);
            const auto& bounds = dims[0][block[0]];
            for(auto i = bounds.first; i < bounds.second; ++i){
                tile(i)
            }
        }
    }
}
