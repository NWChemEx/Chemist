#include "chemist/electrons/many_electrons.hpp"
#include <catch2/catch.hpp>

TEST_CASE("ManyElectrons") {
    using chemist::ManyElectrons;
    ManyElectrons i, j{2};

    SECTION("Size") {
        SECTION("Defaulted") { REQUIRE(i.size == 0); }
        SECTION("Non-defaulted") { REQUIRE(j.size == 2); }
    }

    SECTION("hash") {
        SECTION("LHS == default") {
            auto lhs = pz::hash_objects(i);
            SECTION("RHS == default") {
                REQUIRE(lhs == pz::hash_objects(ManyElectrons{}));
            }
            SECTION("RHS == Non-default") {
                REQUIRE(lhs != pz::hash_objects(j));
            }
        }
    }

    SECTION("Comparisons") {
        REQUIRE(i == chemist::ManyElectrons{});
        REQUIRE(i != j);
    }
}
