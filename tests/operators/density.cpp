#include "libchemist/operators/density.hpp"
#include "test_operator.hpp"

TEMPLATE_LIST_TEST_CASE("Density", "", testing::density_types) {
    using density_type = TestType;
    using value_type   = typename density_type::value_type;

    SECTION("Typedefs") {
        SECTION("value_type") {
            using corr = libchemist::type::tensor;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }
    }

    density_type defaulted;
    auto a_tensor = testing::generate_tensor(2);
    density_type has_value(a_tensor);

    SECTION("CTors") {
        SECTION("Default") { REQUIRE(defaulted.value() == value_type{}); }

        SECTION("Value") { REQUIRE(has_value.value() == a_tensor); }

        SECTION("Copy") {
            density_type copy(has_value);
            REQUIRE(copy.value() == a_tensor);
        }

        SECTION("Move") {
            density_type moved(std::move(has_value));
            REQUIRE(moved.value() == a_tensor);
        }
    }

    SECTION("Assignment") {
        SECTION("Copy") {
            density_type copy;
            auto pcopy = &(copy = has_value);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.value() == a_tensor);
        }

        SECTION("Move") {
            density_type moved;
            auto pmoved = &(moved = std::move(has_value));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.value() == a_tensor);
        }
    }

    SECTION("value") { REQUIRE(has_value.value() == a_tensor); }

    SECTION("hash") {
        auto default_hash = pluginplay::hash_objects(defaulted);
        auto value_hash   = pluginplay::hash_objects(has_value);

        SECTION("Both default") {
            density_type rhs;
            REQUIRE(default_hash == pluginplay::hash_objects(rhs));
        }

        SECTION("Both have same value") {
            density_type rhs(a_tensor);
            REQUIRE(value_hash == pluginplay::hash_objects(rhs));
        }

        SECTION("Different tensors") { REQUIRE(value_hash != default_hash); }
    }

    SECTION("Comparison") {
        SECTION("Both default") {
            density_type rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }

        SECTION("Both have same value") {
            density_type rhs(a_tensor);
            REQUIRE(has_value == rhs);
            REQUIRE_FALSE(has_value != rhs);
        }

        SECTION("Different tensors") {
            REQUIRE(defaulted != has_value);
            REQUIRE_FALSE(defaulted == has_value);
        }
    }
}
