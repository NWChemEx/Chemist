#include "chemist/point/point.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

using namespace chemist;

TEST_CASE("Point<double> : default ctor") {
    Point<double> p;
    SECTION("x") { REQUIRE(p.x() == 0.0); }
    SECTION("y") { REQUIRE(p.y() == 0.0); }
    SECTION("z") { REQUIRE(p.z() == 0.0); }
}

TEST_CASE("Point<double> : value ctor") {
    Point<double> p(1.0, 2.0, 3.0);
    SECTION("x") { REQUIRE(p.x() == 1.0); }
    SECTION("y") { REQUIRE(p.y() == 2.0); }
    SECTION("z") { REQUIRE(p.z() == 3.0); }
}

TEST_CASE("Point<double> : copy ctor") {
    Point<double> p(1.0, 2.0, 3.0);
    Point<double> p2(p);
    SECTION("Value") { REQUIRE(p2 == p); }
    SECTION("Is deep copy") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&p2.coord(i) != &p.coord(i));
    }
}

TEST_CASE("Point<double> : move ctor") {
    Point<double> p(1.0, 2.0, 3.0);
    Point<double> p2(p);
    std::array<double*, 3> ptr{&p.coord(0), &p.coord(1), &p.coord(2)};
    Point<double> p3(std::move(p));
    SECTION("Value") { REQUIRE(p3 == p2); }
    SECTION("References remain valid") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&p3.coord(i) == ptr[i]);
    }
}

TEST_CASE("Point<double> : copy assignment") {
    Point<double> p(1.0, 2.0, 3.0);
    Point<double> p2;
    std::array<double*, 3> ptr{&p2.coord(0), &p2.coord(1), &p2.coord(2)};
    auto pp2 = &(p2 = p);
    SECTION("Value") { REQUIRE(p2 == p); }
    SECTION("Returns *this") { REQUIRE(pp2 == &p2); }
    SECTION("Is deep copy") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&p2.coord(i) != &p.coord(i));
    }
    SECTION("Does not invalidate references") {
        REQUIRE(*ptr[0] == 1.0);
        REQUIRE(*ptr[1] == 2.0);
        REQUIRE(*ptr[2] == 3.0);
    }
}

TEST_CASE("Point<double> : move assignment") {
    Point<double> p(1.0, 2.0, 3.0);
    Point<double> p2(p);
    std::array<double*, 3> ptr{&p.coord(0), &p.coord(1), &p.coord(2)};
    Point<double> p3;
    auto pp3 = &(p3 = std::move(p));
    SECTION("Value") { REQUIRE(p3 == p2); }
    SECTION("Returns *this") { REQUIRE(pp3 == &p3); }
    SECTION("Does not invalidate p's references") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(ptr[i] == &p3.coord(i));
    }
}

TEST_CASE("Point<double> : coord()") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.coord(1) == 2.0);
    SECTION("Throws if requested index is out of range") {
        REQUIRE_THROWS_AS(p.coord(3), std::out_of_range);
    }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.coord(0))>);
    }
}

TEST_CASE("Point<double> : coord() const") {
    const Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.coord(1) == 2.0);
    SECTION("Throws if requested index is out of range") {
        REQUIRE_THROWS_AS(p.coord(3), std::out_of_range);
    }
    SECTION("Is read-only ") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.coord(0))>);
    }
}

TEST_CASE("Point<double> : x()") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.x() == 1.0);
    SECTION("Same as calling coord(0)") { REQUIRE(&p.x() == &p.coord(0)); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.x())>);
    }
}

TEST_CASE("Point<double> : x() const") {
    const Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.x() == 1.0);
    SECTION("Same as calling coord(0)") { REQUIRE(&p.x() == &p.coord(0)); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.x())>);
    }
}

TEST_CASE("Point<double> : y()") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.y() == 2.0);
    SECTION("Same as calling coord(1)") { REQUIRE(&p.y() == &p.coord(1)); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.y())>);
    }
}

TEST_CASE("Point<double> : y() const") {
    const Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.y() == 2.0);
    SECTION("Same as calling coord(1)") { REQUIRE(&p.y() == &p.coord(1)); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.y())>);
    }
}

TEST_CASE("Point<double> : z()") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.z() == 3.0);
    SECTION("Same as calling coord(2)") { REQUIRE(&p.z() == &p.coord(2)); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.z())>);
    }
}

TEST_CASE("Point<double> : z() const") {
    const Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.z() == 3.0);
    SECTION("Same as calling coord(2)") { REQUIRE(&p.z() == &p.coord(2)); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.z())>);
    }
}

TEST_CASE("Point<double> : operator==") {
    Point<double> p{1.0, 2.0, 3.0};
    SECTION("Same instance") { REQUIRE(p == p); }
    SECTION("Different instance, same values") {
        Point<double> p2{1.0, 2.0, 3.0};
        REQUIRE(p2 == p);
    }
    SECTION("Different values") {
        Point<double> p2{2.0, 3.0, 4.0};
        REQUIRE_FALSE(p2 == p);
    }
}

TEST_CASE("Point<double> : operator!=") {
    Point<double> p{1.0, 2.0, 3.0};
    SECTION("Same instance") { REQUIRE_FALSE(p != p); }
    SECTION("Different instance, same values") {
        Point<double> p2{1.0, 2.0, 3.0};
        REQUIRE_FALSE(p2 != p);
    }
    SECTION("Different values") {
        Point<double> p2{2.0, 3.0, 4.0};
        REQUIRE(p2 != p);
    }
}

TEST_CASE("Point serialization") {
    Point<double> p{1.0, 2.0, 3.0};
    Point<double> p2;
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

TEST_CASE("Point<double> : operator-") {
    Point<double> p0{1.0, 2.0, 3.0};
    Point<double> p1{4.0, 5.0, 6.0};
    Point<double> corr{3.0, 3.0, 3.0};
    auto diff = p1 - p0;
    REQUIRE(diff == corr);
}

TEST_CASE("Point<double> : magnitude") {
    Point<double> p0{1.0, 2.0, 3.0};
    double corr = 3.741657387;
    REQUIRE(p0.magnitude() == Approx(corr));
}