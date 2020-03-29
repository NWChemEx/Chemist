#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/condense_tensor.hpp>

using namespace libchemist;

using index_type  = std::vector<std::size_t>;
using range_map   = std::map<index_type, TA::Range>;
using inner_tile  = TA::Tensor<double>;
using result_tile = TA::Tensor<inner_tile>;

TEST_CASE("add_to_inner_tile"){
    SECTION("vector") {
        TA::TiledRange trange{{0, 1, 2, 3, 4}};
        TA::TSpArrayD t(TA::get_default_world(), trange, {1, 2, 3, 4});
        range_map r{{index_type{1}, TA::Range{{0}, {1}}},
                    {index_type{3}, TA::Range{{1}, {2}}}};
        TA::Range new_range{{0}, {2}};
        TA::TensorD new_tile(new_range, 0);
        auto result = detail_::add_to_inner_tile_(std::move(new_tile), r, t);
        TA::TensorD corr(new_range, {2, 4});
        REQUIRE(result == corr);
    }
}

TEST_CASE("condense_tensor"){
    SECTION("vector") {
        index_type i0{0}, i1{1}, i2{2}, i3{3};
        sparse_map::SparseMap sm{{index_type{0}, {i1, i3}}};
        TA::TiledRange trange{{0, 1, 2, 3, 4}};
        TA::TSpArrayD t(TA::get_default_world(), trange, {1, 2, 3, 4});
        TA::TiledRange outer_trange{{0}, {1}};

        auto result = condense_tensor(sm, t, outer_trange);
        std::cout << result << std::endl;
    }
}
