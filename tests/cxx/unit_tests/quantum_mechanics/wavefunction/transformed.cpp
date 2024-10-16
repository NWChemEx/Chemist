#include "../../test_helpers.hpp"
#include "../test_qm.hpp"
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <chemist/quantum_mechanics/wavefunction/transformed.hpp>

/// TODO: Probably need to test with other values for the template parameter
TEST_CASE("Transformed") {
    using from_space_type = chemist::wavefunction::AOs;
    using to_space_type   = chemist::wavefunction::Transformed<from_space_type>;
    using tensor_type     = typename to_space_type::transform_type;

    from_space_type aos_defaulted;
    from_space_type aos(test_chemist::h2_basis());

    tensor_type c_default;
    tensor_type c({{1.0, 0.0}, {0.0, 1.0}});

    to_space_type defaulted;
    to_space_type value_defaulted(aos_defaulted, c_default);
    to_space_type value(aos, c);

    SECTION("CTors and assignment") {
        SECTION("Defaulted") {
            REQUIRE(defaulted.from_space() == aos_defaulted);
            REQUIRE(defaulted.transform() == c_default);
        }

        SECTION("Value") {
            REQUIRE(value_defaulted.from_space() == aos_defaulted);
            REQUIRE(value_defaulted.transform() == c_default);

            REQUIRE(value.from_space() == aos);
            REQUIRE(value.transform() == c);
        }

        test_chemist::test_copy_and_move(defaulted, value_defaulted, value);
    }

    SECTION("from_space") {
        REQUIRE(defaulted.from_space() == aos_defaulted);
        REQUIRE(value_defaulted.from_space() == aos_defaulted);
        REQUIRE(value.from_space() == aos);

        // Can write to it.
        defaulted.from_space() = aos;
        REQUIRE(defaulted.from_space() == aos);
    }

    SECTION("from_space() const") {
        REQUIRE(std::as_const(defaulted).from_space() == aos_defaulted);
        REQUIRE(std::as_const(value).from_space() == aos);
    }

    SECTION("transform") {
        REQUIRE(defaulted.transform() == c_default);
        REQUIRE(value.transform() == c);

        // Can write to it
        defaulted.transform() = c;
        REQUIRE(defaulted.transform() == c);
    }

    SECTION("transform() const") {
        REQUIRE(std::as_const(defaulted).transform() == c_default);
        REQUIRE(std::as_const(value).transform() == c);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(value.size() == 2);
    }

    SECTION("clone") {
        REQUIRE(defaulted.clone()->are_equal(defaulted));
        REQUIRE(value.clone()->are_equal(value));
    }

    SECTION("operator==") {
        // Same
        REQUIRE(defaulted == to_space_type(aos_defaulted, c_default));

        // Different from space
        REQUIRE_FALSE(defaulted == to_space_type(aos, c_default));

        // Different transform
        REQUIRE_FALSE(defaulted == to_space_type(aos_defaulted, c));
    }

    SECTION("operator!=") {
        // Just negates operator==, so spot checking is fine if operator== works
        REQUIRE(defaulted != to_space_type(aos, c_default));
        REQUIRE_FALSE(defaulted != to_space_type(aos_defaulted, c_default));
    }
}