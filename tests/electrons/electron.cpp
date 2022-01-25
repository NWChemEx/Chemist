#include "chemist/electrons/electron.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Electron") {
    chemist::Electron i, j;

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        REQUIRE(hash_objects(i) == hash_objects(j));
    }

    SECTION("Comparisons") {
        REQUIRE(i == j);
        REQUIRE_FALSE(i != j);
    }
}
