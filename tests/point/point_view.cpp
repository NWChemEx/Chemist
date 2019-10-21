#include <catch2/catch.hpp>
#include <libchemist/point/point_view.hpp>

using namespace libchemist;

TEST_CASE("PointView<double> : point ctor") {
    std::vector<double*> corr(3);
    Point<double> p{1.0, 2.0, 3.0};
    for(std::size_t i = 0; i < 3; ++i) corr[i] = &p.coord(i);
    PointView<double> pp(std::move(p));
    SECTION("Is alias") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&pp.coord(i) == corr[i]);
    }
}

TEST_CASE("PointView<const double> : point ctor") {
    std::vector<double*> corr(3);
    Point<double> p{1.0, 2.0, 3.0};
    for(std::size_t i = 0; i < 3; ++i) corr[i] = &p.coord(i);
    PointView<const double> pp(std::move(p));
    SECTION("Is alias") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&pp.coord(i) == corr[i]);
    }
}

TEST_CASE("PointView<double>: coord()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<const double>: coord()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<double>: coord() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<const double>: coord() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<double>: x()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<const double>: x()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<double>: x() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<const double>: x() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<double>: y()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<const double>: y()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<double>: y() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<const double>: y() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<double>: z()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<const double>: z()") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<double>: z() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<const double>: z() const") {
    Point<double> p{1.0, 2.0, 3.0};
    const PointView<const double> pv(p);
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<double>: implicit conversion to Point<double>") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<double> pv(p);
    SECTION("To read-only reference") {
        const Point<double>& ppv = pv;
        REQUIRE(ppv == p);
    }
}

TEST_CASE("PointView<const double>: implicit conversion to Point<double>") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<const double> pv(p);
    SECTION("To read-only reference") {
        const Point<double>& ppv = pv;
        REQUIRE(ppv == p);
    }
}
