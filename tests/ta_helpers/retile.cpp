#include "chemist/ta_helpers/retile.hpp"
#include <catch2/catch.hpp>

using namespace chemist::ta_helpers;
using ei = chemist::sparse_map::ElementIndex;

TEST_CASE("insert_tile_boundaries(TiledRange,vector)") {
    using vector_t = std::vector<ei>;
    TA::TiledRange tr0{{0, 2, 4, 6, 8, 10}};
    TA::TiledRange tr1{{0, 2, 4}, {0, 3, 9}};

    SECTION("Add no boundaries") {
        SECTION("vector") {
            auto r = insert_tile_boundaries(tr0, vector_t{});
            REQUIRE(r == tr0);
        }

        SECTION("matrix") {
            auto r = insert_tile_boundaries(tr1, vector_t{});
            REQUIRE(r == tr1);
        }
    }

    SECTION("Add one boundary") {
        SECTION("vector") {
            auto r = insert_tile_boundaries(tr0, vector_t{ei{1}});
            TA::TiledRange corr{{0, 1, 2, 4, 6, 8, 10}};
            REQUIRE(r == corr);
        }

        SECTION("matrix") {
            auto r = insert_tile_boundaries(tr1, vector_t{ei{1, 1}});
            TA::TiledRange corr{{0, 1, 2, 4}, {0, 1, 3, 9}};
            REQUIRE(r == corr);
        }
    }

    SECTION("Add two boundaries") {
        SECTION("vector") {
            auto r = insert_tile_boundaries(tr0, vector_t{ei{1}, ei{3}});
            TA::TiledRange corr{{0, 1, 2, 3, 4, 6, 8, 10}};
            REQUIRE(r == corr);
        }

        SECTION("matrix") {
            auto r = insert_tile_boundaries(tr1, vector_t{ei{1, 3}, ei{3, 2}});
            TA::TiledRange corr{{0, 1, 2, 3, 4}, {0, 2, 3, 9}};
            REQUIRE(r == corr);
        }
    }
}

TEST_CASE("insert_tile_boundaries(TiledRange, ElementIndex, ...)") {
    TA::TiledRange tr0{{0, 2, 4, 6, 8, 10}};
    TA::TiledRange tr1{{0, 2, 4}, {0, 3, 9}};

    SECTION("Add one boundary") {
        SECTION("vector") {
            auto r = insert_tile_boundaries(tr0, ei{1});
            TA::TiledRange corr{{0, 1, 2, 4, 6, 8, 10}};
            REQUIRE(r == corr);
        }

        SECTION("matrix") {
            auto r = insert_tile_boundaries(tr1, ei{1, 1});
            TA::TiledRange corr{{0, 1, 2, 4}, {0, 1, 3, 9}};
            REQUIRE(r == corr);
        }
    }

    SECTION("Add two boundaries") {
        SECTION("vector") {
            auto r = insert_tile_boundaries(tr0, ei{1}, ei{3});
            TA::TiledRange corr{{0, 1, 2, 3, 4, 6, 8, 10}};
            REQUIRE(r == corr);
        }

        SECTION("matrix") {
            auto r = insert_tile_boundaries(tr1, ei{1, 3}, ei{3, 2});
            TA::TiledRange corr{{0, 1, 2, 3, 4}, {0, 2, 3, 9}};
            REQUIRE(r == corr);
        }
    }
}
