#include "chemist/point/point_view.hpp"
#include <catch2/catch.hpp>

/* Testing Strategy:
 *
 * The PointView class works by wrapping an instance that implements the Point
 * API around aliased data. Most of the machinery is actually implemented by the
 * ViewBase class so all we need to test is that the Point-specific functions
 * are implemented correctly.
 *
 */

using namespace chemist;

using point_t      = Point<double>;
using view_t       = PointView<double, point_t>;
using const_view_t = PointView<const double, point_t>;

TEST_CASE("PointView: coord()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    double* p1 = &p.y();
    double* p2 = &p.z();
    view_t pv(std::move(p));
    SECTION("Correct values") {
        REQUIRE(pv.coord(0) == 1.0);
        REQUIRE(pv.coord(1) == 2.0);
        REQUIRE(pv.coord(2) == 3.0);
    }
    SECTION("Are aliases") {
        REQUIRE(&pv.coord(0) == p0);
        REQUIRE(&pv.coord(1) == p1);
        REQUIRE(&pv.coord(2) == p2);
    }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("const PointView: coord()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    double* p1 = &p.y();
    double* p2 = &p.z();
    const_view_t pv(std::move(p));
    SECTION("Correct values") {
        REQUIRE(pv.coord(0) == 1.0);
        REQUIRE(pv.coord(1) == 2.0);
        REQUIRE(pv.coord(2) == 3.0);
    }
    SECTION("Are aliases") {
        REQUIRE(&pv.coord(0) == p0);
        REQUIRE(&pv.coord(1) == p1);
        REQUIRE(&pv.coord(2) == p2);
    }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView: coord() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    double* p1 = &p.y();
    double* p2 = &p.z();
    const view_t pv(std::move(p));
    SECTION("Correct values") {
        REQUIRE(pv.coord(0) == 1.0);
        REQUIRE(pv.coord(1) == 2.0);
        REQUIRE(pv.coord(2) == 3.0);
    }
    SECTION("Are aliases") {
        REQUIRE(&pv.coord(0) == p0);
        REQUIRE(&pv.coord(1) == p1);
        REQUIRE(&pv.coord(2) == p2);
    }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("const PointView: coord() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    double* p1 = &p.y();
    double* p2 = &p.z();
    const const_view_t pv(std::move(p));
    SECTION("Correct values") {
        REQUIRE(pv.coord(0) == 1.0);
        REQUIRE(pv.coord(1) == 2.0);
        REQUIRE(pv.coord(2) == 3.0);
    }
    SECTION("Are aliases") {
        REQUIRE(&pv.coord(0) == p0);
        REQUIRE(&pv.coord(1) == p1);
        REQUIRE(&pv.coord(2) == p2);
    }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView: x()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is an alias") { REQUIRE(&pv.x() == p0); }
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.x())>);
    }
}

TEST_CASE("const PointView: x()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    const_view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is an alias") { REQUIRE(&pv.x() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView: x() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    const view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is an alias") { REQUIRE(&pv.x() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("const PointView: x() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.x();
    const const_view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is an alias") { REQUIRE(&pv.x() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView: y()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.y();
    view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is an alias") { REQUIRE(&pv.y() == p0); }
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.y())>);
    }
}

TEST_CASE("const PointView: y()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.y();
    const_view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is an alias") { REQUIRE(&pv.y() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView: y() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.y();
    const view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is an alias") { REQUIRE(&pv.y() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("const PointView: y() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.y();
    const const_view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is an alias") { REQUIRE(&pv.y() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView: z()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.z();
    view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is an alias") { REQUIRE(&pv.z() == p0); }
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.z())>);
    }
}

TEST_CASE("const PointView: z()") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.z();
    const_view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is an alias") { REQUIRE(&pv.z() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView: z() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.z();
    const view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is an alias") { REQUIRE(&pv.z() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("const PointView: z() const") {
    Point<double> p{1.0, 2.0, 3.0};
    double* p0 = &p.z();
    const const_view_t pv(std::move(p));
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is an alias") { REQUIRE(&pv.z() == p0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}
