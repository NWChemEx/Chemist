#include "libchemist/fields/electric_fiedl.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Electric") {
    using efield_t = libchemist::field::Electric;

    SECTION("Typedefs") {
        SECTION("scalar_type") {
            using t    = typename efield_t::scalar_type;
            using corr = double;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("size_type") {
            using t    = typename efield_t::size_type;
            using corr = std::size_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("charge_type") {
            using t    = typename efield_t::charge_type;
            using corr = PointCharge<double>;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }
    }

    SECTION("Default ctor") {
        efield_t e;
        REQUIRE(e.ncharges() == 0);
    }

    using q_t = typename efield_t::charge_type;
    q_t q0, q1(1.0, 2.0, 3.0, 4.0);

    SECTION("add_charge") {
        efield_t e;
        e.add_field(q0);
        REQUIRE(e.ncharges() == 1);
        REQUIRE(e.charge(0) == q0);
    }
}