#include "libchemist/operator/many_electrons.hpp"
#include <catch2/catch.hpp>

TEST_CASE("ManyElectrons") {
    using libchemist::operators::ManyElectrons;
    ManyElectrons i, j{2};

    SECTION("Size"){
      SECTION("Defaulted") {
          REQUIRE(i.size == 0);
      }
      SECTION("Non-defaulted") {
          REQUIRE(j.size == 2);
      }
    }

    SECTION("hash") {
        SECTION("LHS == default") {
            auto lhs = pluginplay::hash_objects(i);
            SECTION("RHS == default") {
                REQUIRE(lhs == pluginplay::hash_objects(ManyElectrons{}));
            }
            SECTION("RHS == Non-default") {
                REQUIRE(lhs != pluginplay::hash_objects(j));
            }
        }
    }

    SECTION("Comparisons") {
        REQUIRE(i == libchemist::operators::ManyElectrons{});
        REQUIRE(i != j);
    }
}
