#include "chemist/electrons/electron.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Electron") {
    chemist::Electron i, j;

    SECTION("hash") {
        REQUIRE(pluginplay::hash_objects(i) == pluginplay::hash_objects(j));
    }

    SECTION("Comparisons") {
        REQUIRE(i == j);
        REQUIRE_FALSE(i != j);
    }
}
