#include "libchemist/tensor/detail_/sparse_map_to_shape.hpp"

namespace libchemist::tensor::detail_ {

using shape_type      = TA::SparseShape<double>;
using trange_type     = TA::TiledRange;
using sparse_map_type = libchemist::sparse_map::SparseMapEE;
using index_type      = libchemist::sparse_map::ElementIndex;
using tile_index      = libchemist::sparse_map::TileIndex;

namespace {

// Makes a tiled range for the provided slice
trange_type make_tr(const idx2mode_type& idx2mode, const trange_type& tr) {
    using tr1_type     = TA::TiledRange1;
    using tr1_vec_type = std::vector<tr1_type>;
    using size_type    = typename tr1_vec_type::size_type;

    const auto nidxs = idx2mode.size();
    tr1_vec_type tr1s(nidxs);
    for(size_type i = 0; i < nidxs; ++i) {
        const auto new_mode = idx2mode[i];
        if(new_mode >= tr.rank())
            throw std::runtime_error("Mapped index is too large");
        tr1s[i] = tr.dim(new_mode);
    }

    return trange_type(tr1s.begin(), tr1s.end());
}

// Converts an element-to-element sparse map into a tile-to-tile sparse map
auto ee_to_tt(const sparse_map_type& sm, const idx2mode_type& idx2mode,
              const trange_type& tr) {
    const auto nind = sm.ind_rank();
    const auto ndep = sm.dep_rank();

    if(nind + ndep != idx2mode.size())
        throw std::runtime_error("SparseMap not consistent with idx2mode");

    if(nind + ndep != tr.rank())
        throw std::runtime_error("SparseMap not consistent with TiledRange");

    auto ind_end = idx2mode.begin() + nind;

    idx2mode_type ind(idx2mode.begin(), ind_end);
    idx2mode_type dep(ind_end, idx2mode.end());

    auto ind_tr = make_tr(ind, tr);
    auto dep_tr = make_tr(dep, tr);

    libchemist::sparse_map::SparseMapET smET(dep_tr, sm);
    return libchemist::sparse_map::SparseMapTT(ind_tr, smET);
}

auto get_tile_index(const tile_index& ind, const tile_index& dep,
                    const idx2mode_type& idx2mode) {
    using rv_type   = std::vector<unsigned int>;
    using size_type = typename rv_type::size_type;
    rv_type rv(idx2mode.size());
    const auto nind = ind.size();
    for(size_type i = 0; i < nind; ++i) rv[idx2mode[i]] = ind[i];
    for(size_type i = 0; i < dep.size(); ++i) rv[idx2mode[i + nind]] = dep[i];
    return rv;
}

} // namespace

shape_type sparse_map_to_shape(const sparse_map_type& sm,
                               const idx2mode_type& idx2mode,
                               const trange_type& tr) {
    const auto smTT = ee_to_tt(sm, idx2mode, tr);
    TA::Tensor<double> shape_data(tr.tiles_range(), 0.0);
    for(const auto& [ind_idx, domain] : smTT) {
        for(const auto& dep_idx : domain) {
            auto full_idx        = get_tile_index(ind_idx, dep_idx, idx2mode);
            shape_data[full_idx] = std::numeric_limits<double>::max();
        }
    }
    return shape_type(shape_data, tr);
}

} // namespace libchemist::tensor::detail_
