#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/condense_tensor_range.hpp>

using namespace libchemist;
using sparse_map::SparseMap;
using index_type = typename SparseMap::key_type;
using range_map  = std::map<index_type, TA::Range>;
using return_t   = std::map<index_type, range_map>;

TEST_CASE("condense_tensor_range"){
    SECTION("Vector"){
        TA::TiledRange trange{{0, 2, 4, 6, 8}};

        SECTION("No independent mode injection"){
            SparseMap sm{{index_type{0}, {index_type{0}, index_type{2}}},
                         {index_type{1}, {index_type{1}, index_type{3}}}};
            auto [new_tile_ranges, new_range] =
              condense_tensor_range(sm, trange);

            SECTION("new_tile_ranges"){
                return_t corr{
                    {index_type{0}, range_map{{index_type{0}, TA::Range{{0}, {2}}},
                                              {index_type{2}, TA::Range{{2}, {4}}}}},
                    {index_type{1}, range_map{{index_type{1}, TA::Range{{0}, {2}}},
                                              {index_type{3}, TA::Range{{2}, {4}}}}}
                };

                bool good = new_tile_ranges == corr;
                REQUIRE(new_tile_ranges == corr);
            }

            SECTION("new_range"){
                range_map corr{
                  {index_type{0}, TA::Range{{0}, {4}}},
                  {index_type{1}, TA::Range{{0}, {4}}}};

                bool good = new_range == corr;
                REQUIRE(good);
            }
        }
    }
}
