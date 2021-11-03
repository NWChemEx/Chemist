// #include "libchemist/sparse_map/sparsify.hpp"
// #include "libchemist/ta_helpers/get_block_idx.hpp"

// namespace libchemist::sparse_map {

// using sparse_map = SparseMap<ElementIndex, ElementIndex>;

// type::tensor sparsify_basis(const type::tensor& C, const sparse_map& sm) {
//     const auto& C_ta = C.get<TA::TSpArrayD>();
//     const auto& tr   = C_ta.trange();
//     TA::TiledRange row_tr{tr.dim(0)}, col_tr{tr.dim(1)};

//     SparseMap<ElementIndex, TileIndex> smet(row_tr, sm);
//     SparseMap<TileIndex, TileIndex> smtt(col_tr, smet);

//     auto l = [&](auto&& out_tile, auto&& in_tile) {
//         auto idx = ta_helpers::get_block_idx(C_ta, in_tile);
//         TileIndex row(idx[0]), col(idx[1]);

//         // Assume independent indices are on the columns, dependent on the
//         rows const bool col_good = smtt.count(col); const bool row_good =
//         col_good ? smtt.at(col).count(row) : false; if(!col_good ||
//         !row_good) { return 0.0; } out_tile = in_tile; return
//         out_tile.norm();
//     };

//     return type::tensor(TA::foreach(C_ta, l));
// }

// } // namespace libchemist::sparse_map
