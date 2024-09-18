#include "../../test_helpers.hpp"
#include <chemist/chemical_system/electron/many_electrons.hpp>

TEST_CASE("ManyElectrons") {
    using chemist::ManyElectrons;
    using electron_type = typename ManyElectrons::value_type;

    ManyElectrons defaulted;
    ManyElectrons value(2);
    electron_type e;

    SECTION("Ctors and assignment") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }

        SECTION("Value") {
            REQUIRE(value.size() == 2);
            REQUIRE(value[0] == e);
            REQUIRE(value[1] == e);
        }

        SECTION("Copy/Move") {
            test_chemist::test_copy_and_move(defaulted, value);
        }
    }

    SECTION("at_") {
        REQUIRE(value[0] == e);
        REQUIRE(value[1] == e);
    }

    SECTION("at_() const") {
        REQUIRE(std::as_const(value)[0] == e);
        REQUIRE(std::as_const(value)[1] == e);
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(value.size() == 2);
    }

    SECTION("operator==") {
        // Default vs. default
        REQUIRE(defaulted == ManyElectrons{});

        // Default vs. non-default
        REQUIRE_FALSE(defaulted == value);

        // Value vs same value
        REQUIRE(value == ManyElectrons{2});

        // Value vs. different value
        REQUIRE_FALSE(value == ManyElectrons{3});
    }

    SECTION("operator!=") {
        // Just negates operator== so spot checking is fine
        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != ManyElectrons{});
    }
}