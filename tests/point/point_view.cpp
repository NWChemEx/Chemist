#include <catch2/catch.hpp>
#include <libchemist/point/point_view.hpp>

using namespace libchemist;

template<typename T, typename U>
static void check_state(T&& view, U&& corr) {
    for(std::size_t i = 0; i < 3; ++i) REQUIRE(&view.coord(i) == &corr[i]);
}

TEST_CASE("PointView<double> : default ctor") {
    PointView<double> p; // basically just making sure it compiles
}

TEST_CASE("PointView<double> : point ctor") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<double> pp(p);
    SECTION("Is alias") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&pp.coord(i) == &p.coord(i));
    }
}

TEST_CASE("PointView<const double> : point ctor") {
    Point<double> p{1.0, 2.0, 3.0};
    PointView<const double> pp(p);
    SECTION("Is alias") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&pp.coord(i) == &p.coord(i));
    }
}

TEST_CASE("PointView<double> : aliasing ctor") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    check_state(pv, v);
}

TEST_CASE("PointView<const double> : aliasing ctor") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    check_state(pv, v);
}

TEST_CASE("PointView<double> : copy ctor") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    PointView<double> pv2(pv);
    check_state(pv2, v);
}

TEST_CASE("PointView<const double> : copy ctor") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    PointView<const double> pv2(pv);
    check_state(pv2, v);
}

TEST_CASE("PointView<double> : move ctor") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    PointView<double> pv2(std::move(pv));
    check_state(pv2, v);
}

TEST_CASE("PointView<const double> : move ctor") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    PointView<const double> pv2(std::move(pv));
    check_state(pv2, v);
}

TEST_CASE("PointView<double> : copy assignment") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    PointView<double> pv2;
    auto ppv2 = &(pv2 = pv);
    check_state(pv2, v);
    SECTION("Returns *this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PointView<const double> : copy assignment") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    PointView<const double> pv2;
    auto ppv2 = &(pv2 = pv);
    check_state(pv2, v);
    SECTION("Returns *this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PointView<double> : move assignment") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    PointView<double> pv2{&v[1], &v[1], &v[1]};
    auto ppv2 = &(pv2 = std::move(pv));
    check_state(pv2, v);
    SECTION("Returns *this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PointView<const double> : move assignment") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    PointView<const double> pv2{&v[1], &v[1], &v[1]};
    auto ppv2 = &(pv2 = std::move(pv));
    check_state(pv2, v);
    SECTION("Returns *this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PointView<double>: coord()") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.coord(0) == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<const double>: coord()") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.coord(0) == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<double>: coord() const") {
    std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.coord(0) == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<const double>: coord() const") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.coord(0) == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.coord(0) == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(0))>);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(pv.coord(3), std::out_of_range);
    }
}

TEST_CASE("PointView<double>: x()") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.x() == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<const double>: x()") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.x() == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<double>: x() const") {
    std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.x() == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<const double>: x() const") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.x() == &v[0]); }
    SECTION("Correct value") { REQUIRE(pv.x() == 1.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PointView<double>: y()") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.y() == &v[1]); }
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<const double>: y()") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.y() == &v[1]); }
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<double>: y() const") {
    std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.y() == &v[1]); }
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<const double>: y() const") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.y() == &v[1]); }
    SECTION("Correct value") { REQUIRE(pv.y() == 2.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PointView<double>: z()") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.z() == &v[2]); }
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<const double>: z()") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.z() == &v[2]); }
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<double>: z() const") {
    std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.z() == &v[2]); }
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<const double>: z() const") {
    const std::vector<double> v{1.0, 2.0, 3.0};
    const PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("Is an alias") { REQUIRE(&pv.z() == &v[2]); }
    SECTION("Correct value") { REQUIRE(pv.z() == 3.0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PointView<double>: implicit conversion to Point<double>") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<double> pv{&v[0], &v[1], &v[2]};
    SECTION("To read-only reference") {
        const Point<double>& ppv = pv;
        check_state(ppv, v);
    }
    SECTION("To non-const value") {
        Point<double> pv2(pv);
        SECTION("Is deep copy") {
            for(std::size_t i = 0; i < 3; ++i)
                REQUIRE(&pv2.coord(i) != &pv.coord(i));
        }
    }
    SECTION("For passing to functions") {
        auto l = [](const Point<double>& input) { return input.x(); };
        REQUIRE(l(pv) == pv.x());
    }
}

TEST_CASE("PointView<const double>: implicit conversion to Point<double>") {
    std::vector<double> v{1.0, 2.0, 3.0};
    PointView<const double> pv{&v[0], &v[1], &v[2]};
    SECTION("To read-only reference") {
        const Point<double>& ppv = pv;
        check_state(ppv, v);
    }
    SECTION("To non-const value") {
        Point<double> pv2(pv);
        SECTION("Is deep copy") {
            for(std::size_t i = 0; i < 3; ++i)
                REQUIRE(&pv2.coord(i) != &pv.coord(i));
        }
    }
    SECTION("For passing to functions") {
        auto l = [](const Point<double>& input) { return input.x(); };
        REQUIRE(l(pv) == pv.x());
    }
}
