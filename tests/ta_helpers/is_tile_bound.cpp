#include "chemist/ta_helpers/is_tile_bound.hpp"
#include <catch2/catch.hpp>

using namespace chemist::ta_helpers;
using chemist::sparse_map::ElementIndex;
using index_type = typename ElementIndex::index_type;

TEST_CASE("is_tile_lower_bound") {
    TA::TiledRange tr0{{0, 2, 4, 6, 8, 10}};
    TA::TiledRange tr1{{0, 2, 4}, {0, 3, 6}};

    SECTION("is a lower bound") {
        SECTION("vector tiling") {
            for(std::size_t i : {0, 2, 4, 6, 8}) {
                REQUIRE(is_tile_lower_bound(tr0, ElementIndex{i}));
            }
        }

        SECTION("matrix tiling") {
            for(std::size_t i : {0, 2})
                for(std::size_t j : {0, 3})
                    REQUIRE(is_tile_lower_bound(tr1, ElementIndex{i, j}));
        }
    }

    SECTION("is not a lower bound") {
        SECTION("vector tiling") {
            for(std::size_t i : {1, 3, 5, 7, 9, 10}) {
                REQUIRE_FALSE(is_tile_lower_bound(tr0, ElementIndex{i}));
            }
        }

        SECTION("matrix tiling") {
            SECTION("mode 0 is lower bound, mode 1 isn't") {
                for(std::size_t i : {0, 2}) {
                    for(std::size_t j : {1, 2, 4, 5, 6}) {
                        const ElementIndex eij{i, j};
                        REQUIRE_FALSE(is_tile_lower_bound(tr1, eij));
                    }
                }
            }

            SECTION("mode 0 isn't, but mode 1 is") {
                for(std::size_t i : {1, 3, 4}) {
                    for(std::size_t j : {0, 3}) {
                        const ElementIndex eij{i, j};
                        REQUIRE_FALSE(is_tile_lower_bound(tr1, eij));
                    }
                }
            }

            SECTION("neither are") {
                for(std::size_t i : {1, 3, 4}) {
                    for(std::size_t j : {1, 2, 4, 5, 6}) {
                        const ElementIndex eij{i, j};
                        REQUIRE_FALSE(is_tile_lower_bound(tr1, eij));
                    }
                }
            }
        }
    }
}

TEST_CASE("is_tile_upper_bound") {
    TA::TiledRange tr0{{0, 2, 4, 6, 8, 10}};
    TA::TiledRange tr1{{0, 2, 4}, {0, 3, 6}};

    SECTION("is a upper bound") {
        SECTION("vector tiling") {
            for(std::size_t i : {2, 4, 6, 8, 10}) {
                REQUIRE(is_tile_upper_bound(tr0, ElementIndex{i}));
            }
        }

        SECTION("matrix tiling") {
            for(std::size_t i : {2, 4})
                for(std::size_t j : {3, 6})
                    REQUIRE(is_tile_upper_bound(tr1, ElementIndex{i, j}));
        }
    }

    SECTION("is not an upper bound") {
        SECTION("vector tiling") {
            for(std::size_t i : {0, 1, 3, 5, 7, 9, 11}) {
                REQUIRE_FALSE(is_tile_upper_bound(tr0, ElementIndex{i}));
            }
        }

        SECTION("matrix tiling") {
            SECTION("mode 0 is upper bound, mode 1 isn't") {
                for(std::size_t i : {2, 4}) {
                    for(std::size_t j : {0, 1, 2, 4, 5, 7}) {
                        const ElementIndex eij{i, j};
                        REQUIRE_FALSE(is_tile_upper_bound(tr1, eij));
                    }
                }
            }

            SECTION("mode 0 isn't, but mode 1 is") {
                for(std::size_t i : {0, 1, 3, 5}) {
                    for(std::size_t j : {3, 6}) {
                        const ElementIndex eij{i, j};
                        REQUIRE_FALSE(is_tile_upper_bound(tr1, eij));
                    }
                }
            }

            SECTION("neither are") {
                for(std::size_t i : {0, 1, 3, 5}) {
                    for(std::size_t j : {0, 1, 2, 4, 5, 7}) {
                        const ElementIndex eij{i, j};
                        REQUIRE_FALSE(is_tile_upper_bound(tr1, eij));
                    }
                }
            }
        }
    }
}
