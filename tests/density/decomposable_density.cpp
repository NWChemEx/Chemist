#include "chemist/density/decomposable_density.hpp"
#include "test_density.hpp"

// Known Densities
using chemist::Decomposable1EDensity;

// Tuple containing the known densities
using density_types = std::tuple<Decomposable1EDensity>;

TEMPLATE_LIST_TEST_CASE("DecomposableDensity", "", density_types) {
    using density_type = TestType;
    using base_type    = typename density_type::base_type;
    using space_type   = typename density_type::space_type;
    using value_type   = typename density_type::value_type;
    using aos_type     = typename density_type::aos_type;

    SECTION("Typedefs") {
        SECTION("base_type") {
            using corr = chemist::OneElectronDensity;
            STATIC_REQUIRE(std::is_same_v<base_type, corr>);
        }

        SECTION("space_type") {
            using corr = chemist::orbital_space::DerivedSpaceD;
            STATIC_REQUIRE(std::is_same_v<space_type, corr>);
        }

        SECTION("value_type") {
            using corr = chemist::type::tensor;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }

        SECTION("aos_type") {
            using corr = chemist::orbital_space::AOSpaceD;
            STATIC_REQUIRE(std::is_same_v<aos_type, corr>);
        }
    }

    density_type defaulted;
    auto a_tensor = testing::generate_tensor(2);
    auto space    = testing::non_default_space<space_type>();
    auto aos      = testing::non_default_space<aos_type>();

    density_type has_value(a_tensor, space);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.value() == value_type{});
            REQUIRE(defaulted.space() == space_type{});
            // Default DerivedSpace throws error when accessing from_space
            REQUIRE_THROWS_AS(defaulted.basis_set(), std::runtime_error);
        }

        SECTION("Value") {
            REQUIRE(has_value.value() == a_tensor);
            REQUIRE(has_value.basis_set() == aos);
            REQUIRE(has_value.space() == space);
        }

        SECTION("Copy") {
            density_type copy(has_value);
            REQUIRE(copy.value() == a_tensor);
            REQUIRE(copy.basis_set() == aos);
            REQUIRE(copy.space() == space);
        }

        SECTION("Move") {
            density_type moved(std::move(has_value));
            REQUIRE(moved.value() == a_tensor);
            REQUIRE(moved.basis_set() == aos);
            REQUIRE(moved.space() == space);
        }
    }

    SECTION("Assignment") {
        SECTION("Copy") {
            density_type copy;
            auto pcopy = &(copy = has_value);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.value() == a_tensor);
            REQUIRE(copy.basis_set() == aos);
            REQUIRE(copy.space() == space);
        }

        SECTION("Move") {
            density_type moved;
            auto pmoved = &(moved = std::move(has_value));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.value() == a_tensor);
            REQUIRE(moved.basis_set() == aos);
            REQUIRE(moved.space() == space);
        }
    }

    SECTION("value") { REQUIRE(has_value.value() == a_tensor); }

    SECTION("basis_set") { REQUIRE(has_value.basis_set() == aos); }

    SECTION("space") { REQUIRE(has_value.space() == space); }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        auto default_hash = hash_objects(defaulted);
        auto value_hash   = hash_objects(has_value);

        SECTION("Both default") {
            density_type rhs;
            REQUIRE(default_hash == hash_objects(rhs));
        }

        SECTION("Both have same value") {
            density_type rhs(a_tensor, space);
            REQUIRE(value_hash == hash_objects(rhs));
        }

        SECTION("Different tensors") {
            density_type rhs(value_type{}, space);
            REQUIRE(value_hash != hash_objects(rhs));
        }

        SECTION("Different AOs") {
            density_type rhs(a_tensor, space_type{});
            REQUIRE(value_hash != hash_objects(rhs));
        }
    }

    SECTION("Comparison") {
        SECTION("Both default") {
            density_type rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }

        SECTION("Both have same value") {
            density_type rhs(a_tensor, space);
            REQUIRE(has_value == rhs);
            REQUIRE_FALSE(has_value != rhs);
        }

        SECTION("Different tensors") {
            density_type rhs(value_type{}, space);
            REQUIRE(rhs != has_value);
            REQUIRE_FALSE(rhs == has_value);
        }

        SECTION("Different AOs") {
            density_type rhs(a_tensor, space_type{});
            REQUIRE(rhs != has_value);
            REQUIRE_FALSE(rhs == has_value);
        }

        SECTION("With base_type") {
            base_type base_with_value(a_tensor, aos);
            REQUIRE(has_value == base_with_value);
            REQUIRE_FALSE(has_value != base_with_value);
        }
    }
}
