#include "chemist/basis_set/primitive.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

using namespace chemist;

TEST_CASE("Primitive<double> : default ctor") {
    Primitive<double> p;
    SECTION("Coefficient") { REQUIRE(p.coefficient() == 0.0); }
    SECTION("Exponent") { REQUIRE(p.exponent() == 0.0); }
    SECTION("Point") { REQUIRE(p == Point<double>(0.0, 0.0, 0.0)); }
}

TEST_CASE("Primitive<double> : value ctor") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    SECTION("Coefficient") { REQUIRE(p.coefficient() == 1.0); }
    SECTION("Exponent") { REQUIRE(p.exponent() == 2.0); }
    SECTION("Point") { REQUIRE(p == Point<double>(3.0, 4.0, 5.0)); }
}

TEST_CASE("Primitive<double> : copy ctor") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    Primitive<double> p2(p);
    SECTION("State") { REQUIRE(p2 == p); }
    SECTION("Deep copy") {
        REQUIRE(&p2.coefficient() != &p.coefficient());
        REQUIRE(&p2.exponent() != &p.exponent());
    }
}

TEST_CASE("Primitive<double> : move ctor") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    Primitive<double> p2(p);
    std::array<double*, 2> pp{&p.coefficient(), &p.exponent()};
    Primitive<double> p3(std::move(p));
    SECTION("State") { REQUIRE(p3 == p2); }
    SECTION("References remain valid") {
        REQUIRE(&p3.coefficient() == pp[0]);
        REQUIRE(&p3.exponent() == pp[1]);
    }
}

TEST_CASE("Primitive<double> : copy assignment") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    Primitive<double> p2;
    auto pp2 = &(p2 = p);
    SECTION("State") { REQUIRE(p == p2); }
    SECTION("Returns this") { REQUIRE(pp2 == &p2); }
    SECTION("Deep copy") {
        REQUIRE(&p2.coefficient() != &p.coefficient());
        REQUIRE(&p2.exponent() != &p.exponent());
    }
}

TEST_CASE("Primitive<double> : move assignment") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    Primitive<double> p2(p);
    std::array<double*, 2> pp{&p.coefficient(), &p.exponent()};
    Primitive<double> p3;
    auto pp3 = &(p3 = std::move(p));
    SECTION("State") { REQUIRE(p3 == p2); }
    SECTION("Returns this") { REQUIRE(pp3 == &p3); }
    SECTION("References remain valid") {
        REQUIRE(&p3.coefficient() == pp[0]);
        REQUIRE(&p3.exponent() == pp[1]);
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

TEST_CASE("Primitive serialization") {
    Primitive<double> p(1.0, 2.0, 3.0, 4.0, 5.0);
    const char* file = "archive.dat";
    Primitive<double> p2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(p);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(p2);
    }
    REQUIRE(p == p2);
}
