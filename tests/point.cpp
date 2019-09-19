#include <catch2/catch.hpp>
#include <libchemist/point.hpp>

using namespace libchemist;

template<typename T, typename U>
static void check_state(T&& point, U&& corr) {
    for(std::size_t i = 0; i < 3; ++i) REQUIRE(point.coord(i) == corr[i]);
}

TEST_CASE("Point<double> : default ctor") {
    Point<double> p;
    check_state(p, std::vector{0.0, 0.0, 0.0});
}

TEST_CASE("Point<double> : value ctor") {
    Point<double> p(1.0, 2.0, 3.0);
    check_state(p, std::vector{1.0, 2.0, 3.0});
}

TEST_CASE("Point<double> : coord") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.coord(1) == 2.0);
    SECTION("Throws if requested index is out of range") {
        REQUIRE_THROWS_AS(p.coord(3), std::out_of_range);
    }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.coord(0))>);
    }
}

TEST_CASE("Point<double> : coord const") {
    const Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.coord(1) == 2.0);
    SECTION("Throws if requested index is out of range") {
        REQUIRE_THROWS_AS(p.coord(3), std::out_of_range);
    }
    SECTION("Is read-only ") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.coord(0))>);
    }
}

TEST_CASE("Point<double> : x") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.x() == 1.0);
    SECTION("Same as calling coord(0)") { REQUIRE(&p.x() == &p.coord(0)); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.x())>);
    }
}

TEST_CASE("Point<double> : x const") {
    const Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.x() == 1.0);
    SECTION("Same as calling coord(0)") { REQUIRE(&p.x() == &p.coord(0)); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.x())>);
    }
}

TEST_CASE("Point<double> : y") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.y() == 2.0);
    SECTION("Same as calling coord(1)") { REQUIRE(&p.y() == &p.coord(1)); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.y())>);
    }
}

TEST_CASE("Point<double> : y const") {
    const Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.y() == 2.0);
    SECTION("Same as calling coord(1)") { REQUIRE(&p.y() == &p.coord(1)); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.y())>);
    }
}

TEST_CASE("Point<double> : z") {
    Point<double> p(1.0, 2.0, 3.0);
    REQUIRE(p.z() == 3.0);
    SECTION("Same as calling coord(2)") { REQUIRE(&p.z() == &p.coord(2)); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(p.z())>);
    }
}

TEST_CASE("Point<double> : z const") {
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
