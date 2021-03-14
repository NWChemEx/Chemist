#include "libchemist/point/point_charge.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

TEMPLATE_TEST_CASE("PointCharge", "", double, float) {
    using point_charge_t = libchemist::PointCharge<TestType>;

    SECTION("typedefs") {
        SECTION("scalar_type") {
            using corr_type = TestType;
            using type      = typename point_charge_t::scalar_type;
            STATIC_REQUIRE(std::is_same_v<type, corr_type>);
        }
    }

    SECTION("Default ctor") {
        point_charge_t q;
        REQUIRE(q.charge() == 0.0);
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(q.coord(i) == 0.0);
    }

    SECTION("Copy ctor") {
        SECTION("other is defaulted") {
            point_charge other;
            point_charge q(other);
            REQUIRE(q == other);
        }

        SECTION("other is non-default") {
            point_charge other(1.0, 2.0, 3.0, 4.0);
            point_charge q(other);
            REQUIRE(q == other);
        }
    }
}

/* Unit testing notes.
 *
 * We assume that the Point base class has been unit tested and that it's
 * comparison operator works. If that is the case then we need to consider what
 * happens if the additional members of the PointCharge class are different and
 * if the base classes differ at all (we do not need to consider all possible
 * ways that the base classes differ since PointCharge deffers to Point's
 * comparison operator).
 */
TEMPLATE_TEST_CASE("PointCharge comparisons", "", double, float) {
    using point_charge_t = libchemist::PointCharge<TestType>;

    SECTION("LHS == defaulted") {
        point_charge_t lhs;

        SECTION("RHS == defaulted") {
            point_charge_t rhs;
            REQUIRE(lhs == rhs);
            REQUIRE_FALSE(lhs != rhs);
        }

        SECTION("RHS has different charge") {
            point_charge_t rhs;
            rhs.charge() = 1.1;
            REQUIRE_FALSE(lhs == rhs);
            REQUIRE(lhs != rhs);
        }

        SECTION("RHS has different origin") {
            point_charge_t rhs;
            rhs.coord(1) = 1.1;
            REQUIRE_FALSE(lhs == rhs);
        }
    }

    SECTION("different type") {
        using rhs_t =
          std::conditional_t<std::is_same_v<TestType, float>, double, float>;
        point_charge_t lhs;
        libchemist::PointCharge<rhs_t> rhs;
        REQUIRE_FALSE(lhs == rhs);
        REQUIRE(lhs != rhs);
    }
}