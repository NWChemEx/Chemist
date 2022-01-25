#include "chemist/potentials/electrostatic.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Electrostatic") {
    using epot_t = chemist::potentials::Electrostatic;

    SECTION("Typedefs") {
        SECTION("scalar_type") {
            using t    = typename epot_t::scalar_type;
            using corr = double;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("size_type") {
            using t    = typename epot_t::size_type;
            using corr = std::size_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("charge_type") {
            using t    = typename epot_t::charge_type;
            using corr = chemist::PointCharge<double>;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("charge_reference") {
            using t    = typename epot_t::charge_reference;
            using corr = chemist::PointCharge<double>&;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("const_charge_reference") {
            using t    = typename epot_t::const_charge_reference;
            using corr = const chemist::PointCharge<double>&;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }
    }

    SECTION("Default ctor") {
        epot_t e;
        REQUIRE(e.ncharges() == 0);
    }

    using q_t = typename epot_t::charge_type;
    q_t q0, q1(1.0, 2.0, 3.0, 4.0);

    SECTION("add_charge") {
        epot_t e;
        e.add_charge(q0);
        REQUIRE(e.ncharges() == 1);
        REQUIRE(e.charge(0) == q0);
    }

    SECTION("charge()") {
        epot_t e;

        SECTION("Can access charge") {
            e.add_charge(q0);
            REQUIRE(e.ncharges() == 1);
            REQUIRE(e.charge(0) == q0);
        }

        SECTION("Is read/write") {
            e.add_charge(q0);
            e.charge(0) = q1;
            REQUIRE(e.charge(0) == q1);
        }

        SECTION("Throws if out of bounds ") {
            REQUIRE_THROWS_AS(e.charge(0), std::out_of_range);
        }
    }

    SECTION("charge() const") {
        epot_t e;

        SECTION("Can access charge") {
            e.add_charge(q0);
            REQUIRE(e.ncharges() == 1);
            REQUIRE(std::as_const(e).charge(0) == q0);
        }

        SECTION("Throws if out of bounds ") {
            REQUIRE_THROWS_AS(std::as_const(e).charge(0), std::out_of_range);
        }
    }

    SECTION("ncharges()") {
        epot_t e;

        SECTION("No charges") { REQUIRE(e.ncharges() == 0); }

        SECTION("One charge") {
            e.add_charge(q0);
            REQUIRE(e.ncharges() == 1);
        }

        SECTION("Two charges") {
            e.add_charge(q0);
            e.add_charge(q1);
            REQUIRE(e.ncharges() == 2);
        }
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS is default") {
            epot_t lhs;
            auto lhs_hash = hash_objects(lhs);

            SECTION("RHS is default") {
                epot_t rhs;
                REQUIRE(lhs_hash == hash_objects(rhs));
            }

            SECTION("RHS has a point charge") {
                epot_t rhs;
                rhs.add_charge(q0);
                REQUIRE(lhs_hash != hash_objects(rhs));
            }
        }

        SECTION("LHS has a point charge") {
            epot_t lhs;
            lhs.add_charge(q0);
            auto lhs_hash = hash_objects(lhs);

            SECTION("RHS is default") {
                epot_t rhs;
                REQUIRE(lhs_hash != hash_objects(rhs));
            }

            SECTION("RHS has same point charge") {
                epot_t rhs;
                rhs.add_charge(q0);
                REQUIRE(lhs_hash == hash_objects(rhs));
            }

            SECTION("RHS has a different point charge") {
                epot_t rhs;
                rhs.add_charge(q1);
                REQUIRE(lhs_hash != hash_objects(rhs));
            }

            SECTION("RHS is a superset") {
                epot_t rhs;
                rhs.add_charge(q0);
                rhs.add_charge(q1);
                REQUIRE(lhs_hash != hash_objects(rhs));
            }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS is default") {
            epot_t lhs;

            SECTION("RHS is default") {
                epot_t rhs;
                REQUIRE(lhs == rhs);
                REQUIRE_FALSE(lhs != rhs);
            }

            SECTION("RHS has a point charge") {
                epot_t rhs;
                rhs.add_charge(q0);
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }
        }

        SECTION("LHS has a point charge") {
            epot_t lhs;
            lhs.add_charge(q0);

            SECTION("RHS is default") {
                epot_t rhs;
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("RHS has same point charge") {
                epot_t rhs;
                rhs.add_charge(q0);
                REQUIRE(lhs == rhs);
                REQUIRE_FALSE(lhs != rhs);
            }

            SECTION("RHS has a different point charge") {
                epot_t rhs;
                rhs.add_charge(q1);
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("RHS is a superset") {
                epot_t rhs;
                rhs.add_charge(q0);
                rhs.add_charge(q1);
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }
        }
    }
}
