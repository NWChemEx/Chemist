#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/detail_/condense_tensor_range.hpp>
#include <set>

using namespace libchemist::detail_;
using index_set  = std::set<std::size_t>;
using range_type = std::pair<std::size_t, std::size_t>;
using range_map  = std::map<std::size_t, range_type>;

TEST_CASE("condense_tensor_range_(T, TiledRange1)"){
    index_set indices{1, 3};
    TA::TiledRange1 trange{0, 2, 4, 6, 8};

    auto [offsets, total_range] = condense_tensor_range_(indices, trange);

    SECTION("Total Range"){
        range_type corr{0ul, 4ul};
        bool good = total_range == corr;
        REQUIRE(good);
    }

    SECTION("Tile Conversions") {
        range_map corr{
            {1ul, range_type{0ul, 2ul}},
            {3ul, range_type{2ul, 4ul}}
        };
        bool good = offsets == corr;
        REQUIRE(good);
    }
}

TEST_CASE("condense_tensor_range(T, TiledRange, index_set)"){

    SECTION("Vector"){
        TA::TiledRange trange{{0, 2, 4, 6, 8}};

        SECTION("No injection"){
            std::vector<index_set> indices{index_set{1, 3}};
            auto [offsets, bounds] = condense_tensor_range_(indices, trange);

            SECTION("Offsets") {
                std::vector<range_map> corr{
                    range_map{{1, range_type{0, 2}},
                              {3, range_type{2, 4}}}
                };
                bool good = offsets == corr;
                REQUIRE(good);
            }

            SECTION("Bounds"){
                std::vector<range_type> corr{range_type{0, 4}};
                bool good = (bounds == corr);
                REQUIRE(good);
            }
        }

        SECTION("Injection mode 0"){
            std::vector<index_set> indices{};
            auto [offsets, bounds] =
              condense_tensor_range_(indices, trange, index_set{0});

            SECTION("Offsets") {
                std::vector<range_map> corr{};
                bool good = offsets == corr;
                REQUIRE(good);
            }

            SECTION("Bounds"){
                std::vector<range_type> corr{range_type{0, 8}};
                bool good = bounds == corr;
                REQUIRE(good);
            }
        }
    }

    SECTION("Matrix") {
        TA::TiledRange trange{{0, 2, 4, 6, 8}, {0, 2, 5, 9, 14}};

        SECTION("No injection"){
            std::vector indices{index_set{0, 3}, index_set{1, 3}};

            auto [offsets, range] = condense_tensor_range_(indices, trange);

            SECTION("Range") {
                std::vector corr{range_type{0, 4}, range_type{0, 8}};
                bool good = range == corr;
                REQUIRE(good);
            }

            SECTION("Offsets") {
                std::vector<range_map> corr{
                  range_map{{0, range_type{0, 2}}, {3, range_type{2, 4}}},
                  range_map{{1, range_type{0, 3}}, {3, range_type{3, 8}}}};
                bool good = offsets == corr;
                REQUIRE(good);
            }
        }

        SECTION("Injection mode 0"){
            std::vector indices{index_set{1, 3}};

            auto [offsets, range] =
              condense_tensor_range_(indices, trange, index_set{0});

            SECTION("Range") {
                std::vector corr{range_type{0, 8}, range_type{0, 8}};
                bool good = range == corr;
                REQUIRE(good);
            }

            SECTION("Offsets") {
                std::vector<range_map> corr{
                  range_map{{1, range_type{0, 3}}, {3, range_type{3, 8}}}};
                bool good = offsets == corr;
                REQUIRE(good);
            }
        }

        SECTION("Injection mode 1"){
            std::vector indices{index_set{0, 3}};

            auto [offsets, range] =
              condense_tensor_range_(indices, trange, index_set{1});

            SECTION("Range") {
                std::vector corr{range_type{0, 4}, range_type{0, 14}};
                bool good = range == corr;
                REQUIRE(good);
            }

            SECTION("Offsets") {
                std::vector<range_map> corr{
                  range_map{{0, range_type{0, 2}}, {3, range_type{2, 4}}},
                };
                bool good = offsets == corr;
                REQUIRE(good);
            }
        }

        SECTION("Injection modes 0 and 1"){
            std::vector<index_set> indices{};

            auto [offsets, range] =
              condense_tensor_range_(indices, trange, index_set{0, 1});

            SECTION("Range") {
                std::vector corr{range_type{0, 8}, range_type{0, 14}};
                bool good = range == corr;
                REQUIRE(good);
            }

            SECTION("Offsets") {
                std::vector<range_map> corr{};
                bool good = offsets == corr;
                REQUIRE(good);
            }
        }
    }
}
