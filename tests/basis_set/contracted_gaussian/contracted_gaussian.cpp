#include <catch2/catch.hpp>
#include <libchemist/basis_set/primitive/primitive.hpp>

using namespace libchemist;

template<typename T, typename U>
static void check_state(T&& prim, U&& corr) {
    SECTION("Coefficient") { REQUIRE(prim.coefficient() == corr[0]); }
    SECTION("Exponent") { REQUIRE(prim.exponent() == corr[1]); }
    SECTION("x-coordinate") { REQUIRE(prim.x() == corr[2]); }
    SECTION("y-coordinate") { REQUIRE(prim.y() == corr[3]); }
    SECTION("z-coordinate") { REQUIRE(prim.z() == corr[4]); }
}

TEST_CASE("Primitive<double> : default ctor") {
    Primitive<double> p;
    check_state(p, std::vector<double>(5, 0.0));
}

TEST_CASE("Primitive<double> : value ctor") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    check_state(p, std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0});
}

TEST_CASE("Primitive<double> : copy ctor") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    Primitive<double> p2(p);
    check_state(p2, std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0});
    SECTION("Deep copy") {
        REQUIRE(&p2.coefficient() != &p.coefficient());
        REQUIRE(&p2.exponent() != &p.exponent());
    }
}

TEST_CASE("Primitive<double> : move ctor") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    std::array<double*, 2> pp{&p.coefficient(), &p.exponent()};
    Primitive<double> p2(std::move(p));
    check_state(p2, std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0});
    SECTION("References remain valid") {
        REQUIRE(&p2.coefficient() == pp[0]);
        REQUIRE(&p2.exponent() == pp[1]);
    }
}

TEST_CASE("Primitive<double> : copy assignment") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    Primitive<double> p2;
    auto pp2 = &(p2 = p);
    check_state(p2, std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0});
    SECTION("Returns this") { REQUIRE(pp2 == &p2); }
    SECTION("Deep copy") {
        REQUIRE(&p2.coefficient() != &p.coefficient());
        REQUIRE(&p2.exponent() != &p.exponent());
    }
}

TEST_CASE("Primitive<double> : move assignment") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    std::array<double*, 2> pp{&p.coefficient(), &p.exponent()};
    Primitive<double> p2;
    auto pp2 = &(p2 = std::move(p));
    check_state(p2, std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0});
    SECTION("Returns this") { REQUIRE(pp2 == &p2); }
    SECTION("References remain valid") {
        REQUIRE(&p2.coefficient() == pp[0]);
        REQUIRE(&p2.exponent() == pp[1]);
    }
}

TEST_CASE("Primitive<double> : coefficient") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    REQUIRE(p.coefficient() == 1.0);
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<decltype(p.coefficient()), double&>);
    }
}

TEST_CASE("Primitive<double> : coefficient const") {
    const Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    REQUIRE(p.coefficient() == 1.0);
    SECTION("Is read-only") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(p.coefficient()), const double&>);
    }
}

TEST_CASE("Primitive<double> : exponent") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    REQUIRE(p.exponent() == 2.0);
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<decltype(p.exponent()), double&>);
    }
}

TEST_CASE("Primitive<double> : exponent const") {
    const Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    REQUIRE(p.exponent() == 2.0);
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<decltype(p.exponent()), const double&>);
    }
}

TEST_CASE("Primitive<double> : operator==") {
    Primitive<double> p;
    SECTION("Same instance") { REQUIRE(p == p); }
    SECTION("Different instance, same value") {
        Primitive<double> p2;
        REQUIRE(p == p2);
    }
    SECTION("Different coefficient") {
        Primitive<double> p2(1.0, 0.0, 0.0, 0.0, 0.0);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different exponent") {
        Primitive<double> p2(0.0, 1.0, 0.0, 0.0, 0.0);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different Point") {
        Primitive<double> p2(0.0, 0.0, 1.0, 2.0, 3.0);
        REQUIRE_FALSE(p == p2);
    }
}

TEST_CASE("Primitive<double> : operator!=") {
    Primitive<double> p;
    SECTION("Same instance") { REQUIRE_FALSE(p != p); }
    SECTION("Different instance, same value") {
        Primitive<double> p2;
        REQUIRE_FALSE(p != p2);
    }
    SECTION("Different coefficient") {
        Primitive<double> p2(1.0, 0.0, 0.0, 0.0, 0.0);
        REQUIRE(p != p2);
    }
    SECTION("Different exponent") {
        Primitive<double> p2(0.0, 1.0, 0.0, 0.0, 0.0);
        REQUIRE(p != p2);
    }
    SECTION("Different Point") {
        Primitive<double> p2(0.0, 0.0, 1.0, 2.0, 3.0);
        REQUIRE(p != p2);
    }
}
