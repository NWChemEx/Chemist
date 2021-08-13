#include "libchemist/electrons/electron.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Electron") {
    libchemist::Electron i, j;

    SECTION("hash") {
        REQUIRE(pluginplay::hash_objects(i) == pluginplay::hash_objects(j));
    }

    SECTION("Comparisons") {
        REQUIRE(i == j);
        REQUIRE_FALSE(i != j);
    }
}
