#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/detail_/condense_tensor_range.hpp>
#include <set>

using namespace libchemist::detail_;
using index_set  = std::set<std::size_t>;
using range_type = std::pair<std::size_t, std::size_t>;
using range_map  = std::map<std::size_t, range_type>;

TEST_CASE("condense_tensor_range(T, TiledRange1)"){
    index_set indices{1, 3};
    TA::TiledRange1 trange{0, 2, 4, 6, 8};

    auto [offsets, total_range] = condense_tensor_range(indices, trange);

    SECTION("Total Range"){
        range_type corr{0ul, 4ul};
        REQUIRE(nelem == corr);
    }
    SECTION("Tile Conversions") {
        range_map corr{
            {0ul, range_type{0ul, 2ul}},
            {1ul, range_type{2ul, 4ul}}
        };
        REQUIRE(offsets == corr);
    }
}

TEST_CASE("condense_tensor_range(T, TiledRange)"){

    SECTION("Matrix") {
        std::vector indices{index_set{0, 3}, index_set{1, 3}};
        std::vector trange{TA::TiledRange1{0, 2, 4, 6, 8},
                           TA::TiledRange1{0, 2, 5, 9, 14}};
        auto [offsets, range] = condense_tensor_range(indices, trange);

        SECTION("Range") {
            std::vector corr{range_type{0, 4}, range_type{0, 8}};
            REQUIRE(range == corr);
        }

        SECTION("Offsets") {
            std::vector<range_map> corr{
              range_map{{0, range_type{0, 2}}, {3, range_type{2, 4}}},
              range_map{{1, range_type{0, 3}}, {3, range_type{3, 8}}}};
            REQUIRE(offsets == corr);
        }
    }
}

TEST_CASE("condense_tensor_range(T, TiledRange)"){

    SECTION("Matrix") {
        std::vector indices{index_set{0, 3}, index_set{1, 3}};
        std::vector trange{TA::TiledRange1{0, 2, 4, 6, 8},
                           TA::TiledRange1{0, 2, 5, 9, 14}};
        auto [offsets, range] = condense_tensor_range(indices, trange);

        SECTION("Range") {
            std::vector corr{range_type{0, 4}, range_type{0, 8}};
            REQUIRE(range == corr);
        }

        SECTION("Offsets") {
            std::vector<range_map> corr{
              range_map{{0, range_type{0, 2}}, {3, range_type{2, 4}}},
              range_map{{1, range_type{0, 3}}, {3, range_type{3, 8}}}};
            REQUIRE(offsets == corr);
        }
    }
}
