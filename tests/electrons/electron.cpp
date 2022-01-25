#include "chemist/electrons/electron.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Electron") {
    chemist::Electron i, j;

    SECTION("hash") { REQUIRE(pz::hash_objects(i) == pz::hash_objects(j)); }

    SECTION("Comparisons") {
        REQUIRE(i == j);
        REQUIRE_FALSE(i != j);
    }
}
