#pragma once
#include "chemist/sparse_map/sparse_map.hpp"
#include <catch2/catch.hpp>

namespace testing {

using index_type_tuple_t = std::tuple<chemist::sparse_map::ElementIndex,
                                      chemist::sparse_map::TileIndex>;

template<typename T, typename U>
using index_pairs_tuple_t_ = std::tuple<std::pair<T, T>, std::pair<T, U>,
                                        std::pair<U, T>, std::pair<U, U>>;

using index_pairs_tuple_t =
  index_pairs_tuple_t_<chemist::sparse_map::ElementIndex,
                       chemist::sparse_map::TileIndex>;

template<typename IndexType, std::size_t N>
auto make_indices() {
    if constexpr(N == 1) {
        IndexType i0{0}, i1{1}, i2{2}, i3{3}, i4{4};
        return std::make_tuple(i0, i1, i2, i3, i4);
    } else if constexpr(N == 2) {
        IndexType i00{0, 0}, i01{0, 1}, i10{1, 0}, i11{1, 1};
        return std::make_tuple(i00, i01, i10, i11);
    } else {
        static_assert(N == 1, "Only support rank 1 and 2 indices");
    }
}

} // namespace testing
