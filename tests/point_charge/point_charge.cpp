#include "chemist/point_charge/point_charge.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

TEMPLATE_TEST_CASE("PointCharge", "", double, float) {
    using point_charge_t = chemist::PointCharge<TestType>;

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
            point_charge_t other;
            point_charge_t q(other);
            REQUIRE(q == other);
        }

        SECTION("other is non-default") {
            point_charge_t other(1.0, 2.0, 3.0, 4.0);
            point_charge_t q(other);
            REQUIRE(q == other);
        }
    }

    SECTION("Move ctor") {
        SECTION("other is defaulted") {
            point_charge_t other, corr;
            point_charge_t q(std::move(other));
            REQUIRE(q == corr);
        }

        SECTION("other is non-default") {
            point_charge_t other(1.0, 2.0, 3.0, 4.0);
            point_charge_t corr(1.0, 2.0, 3.0, 4.0);
            point_charge_t q(std::move(other));
            REQUIRE(q == corr);
        }
    }

    SECTION("Value ctor") {
        point_charge_t q(1.0, 2.0, 3.0, 4.0);
        REQUIRE(q.charge() == 1.0);
        REQUIRE(q.coord(0) == 2.0);
        REQUIRE(q.coord(1) == 3.0);
        REQUIRE(q.coord(2) == 4.0);
    }

    SECTION("Copy assignment") {
        SECTION("other is defaulted") {
            point_charge_t other, q;
            auto pq = &(q = other);
            REQUIRE(pq == &q);
            REQUIRE(q == other);
        }

        SECTION("other is non-default") {
            point_charge_t q, other(1.0, 2.0, 3.0, 4.0);
            auto pq = &(q = other);
            REQUIRE(pq == &q);
            REQUIRE(q == other);
        }
    }

    SECTION("Move assignment") {
        SECTION("other is defaulted") {
            point_charge_t other, q, corr;
            auto pq = &(q = std::move(other));
            REQUIRE(pq == &q);
            REQUIRE(q == corr);
        }

        SECTION("other is non-default") {
            point_charge_t other(1.0, 2.0, 3.0, 4.0), q;
            point_charge_t corr(1.0, 2.0, 3.0, 4.0);
            auto pq = &(q = std::move(other));
            REQUIRE(pq == &q);
            REQUIRE(q == corr);
        }
    }

    SECTION("charge()") {
        SECTION("Defaulted") {
            point_charge_t q;
            REQUIRE(q.charge() == 0.0);
        }

        SECTION("Non-defaulted") {
            point_charge_t q(1.0, 2.0, 3.0, 4.0);
            REQUIRE(q.charge() == 1.0);
        }

        SECTION("Is read/writeable") {
            point_charge_t q;
            q.charge() = 1.0;
            REQUIRE(q.charge() == 1.0);
        }
    }

    SECTION("charge() const") {
        SECTION("Defaulted") {
            const point_charge_t q;
            REQUIRE(q.charge() == 0.0);
        }

        SECTION("Non-defaulted") {
            const point_charge_t q(1.0, 2.0, 3.0, 4.0);
            REQUIRE(q.charge() == 1.0);
        }
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == defaulted") {
            point_charge_t lhs;
            auto lhs_hash = hash_objects(lhs);

            SECTION("RHS == defaulted") {
                point_charge_t rhs;
                REQUIRE(lhs_hash == hash_objects(rhs));
            }

            SECTION("RHS has different charge") {
                point_charge_t rhs;
                rhs.charge() = 1.1;
                REQUIRE(lhs_hash != hash_objects(rhs));
            }

            SECTION("RHS has different origin") {
                point_charge_t rhs;
                rhs.coord(1) = 1.1;
                REQUIRE(lhs_hash != hash_objects(rhs));
            }
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
    using point_charge_t = chemist::PointCharge<TestType>;

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
        constexpr bool test_type_is_float = std::is_same_v<TestType, float>;
        using rhs_t = std::conditional_t<test_type_is_float, double, float>;

        point_charge_t lhs;
        chemist::PointCharge<rhs_t> rhs;
        REQUIRE_FALSE(lhs == rhs);
        REQUIRE(lhs != rhs);
    }
}
