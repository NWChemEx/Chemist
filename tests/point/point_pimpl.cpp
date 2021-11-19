#include "chemist/point/point_pimpl.hpp"
#include <catch2/catch.hpp>

using pimpl_t = chemist::detail_::PointPIMPL<double>;

template<typename T, typename U>
static void compare_coords(T&& pimpl, U&& corr) {
    SECTION("Value correctness") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(pimpl.coord(i) == corr[i]);
    }
}

TEST_CASE("PointPIMPL<double> : default ctor") {
    pimpl_t p;
    compare_coords(p, std::vector{0, 0, 0});
}

TEST_CASE("PointPIMPL<double> : value ctor") {
    pimpl_t p(1.0, 2.0, 3.0);
    compare_coords(p, std::vector{1, 2, 3});
}

TEST_CASE("PointPIMPL<double> : alias ctor") {
    std::array<double, 3> v{1, 2, 3};
    pimpl_t p(&v[0], &v[1], &v[2]);
    compare_coords(p, v);
    SECTION("Are aliases") {
        for(std::size_t i = 0; i < 3; ++i) REQUIRE(&p.coord(i) == &v[i]);
    }
}

TEST_CASE("PointPIMPL<double> : copy ctor") {
    std::array<double, 3> v{1, 2, 3};
    pimpl_t p(&v[0], &v[1], &v[2]);
    pimpl_t p2(p);
    compare_coords(p2, v);
    SECTION("Is deep copy") {
        for(std::size_t i = 0; i < 3; ++i)
            REQUIRE_FALSE(&p.coord(i) == &p2.coord(i));
    }
}

TEST_CASE("PointPIMPL<double> : copy assignment") {
    std::array<double, 3> v{1, 2, 3};
    pimpl_t p(&v[0], &v[1], &v[2]);
    pimpl_t p2;
    auto pp2 = &(p2 = p);
    SECTION("Returns *this") { REQUIRE(pp2 == &p2); }
    compare_coords(p2, v);
    SECTION("Is deep copy") {
        for(std::size_t i = 0; i < 3; ++i)
            REQUIRE_FALSE(&p.coord(i) == &p2.coord(i));
    }
}

TEST_CASE("PointPIMPL<double> : move ctor") {
    SECTION("Move from owning") {
        pimpl_t p(1.0, 2.0, 3.0);
        pimpl_t p2(std::move(p));
        compare_coords(p2, std::vector{1, 2, 3});
    }
    SECTION("Move from alias") {
        std::array<double, 3> v{1, 2, 3};
        pimpl_t p(&v[0], &v[1], &v[2]);
        pimpl_t p2(std::move(p));
        compare_coords(p2, v);
        SECTION("Is still alias") {
            for(std::size_t i = 0; i < 3; ++i) REQUIRE(&p2.coord(i) == &v[i]);
        }
    }
}

TEST_CASE("PointPIMPL<double> : move assignment") {
    SECTION("Move from owning") {
        pimpl_t p(1.0, 2.0, 3.0);
        pimpl_t p2;
        auto pp2 = &(p2 = std::move(p));
        SECTION("Returns this") { REQUIRE(pp2 == &p2); }
        compare_coords(p2, std::vector{1, 2, 3});
    }
    SECTION("Move assign from alias") {
        std::array<double, 3> v{1, 2, 3};
        pimpl_t p(&v[0], &v[1], &v[2]);
        pimpl_t p2;
        auto pp2 = &(p2 = std::move(p));
        SECTION("Returns *this") { REQUIRE(pp2 == &p2); }
        compare_coords(p2, v);
        SECTION("Is still alias") {
            for(std::size_t i = 0; i < 3; ++i) REQUIRE(&p2.coord(i) == &v[i]);
        }
    }
}

TEST_CASE("PointPIMPL<double> : coord") {
    pimpl_t p{1.0, 2.0, 3.0};
    SECTION("Valid index") { REQUIRE(p.coord(1) == 2.0); }
    SECTION("Invalid index") {
        REQUIRE_THROWS_AS(p.coord(3), std::out_of_range);
    }
    SECTION("Is read/write") {
        using r_type = std::remove_reference_t<decltype(p.coord(0))>;
        STATIC_REQUIRE_FALSE(std::is_const_v<r_type>);
    }
}

TEST_CASE("PointPIMPL<double> : coord const") {
    const pimpl_t p{1.0, 2.0, 3.0};
    SECTION("Valid index") { REQUIRE(p.coord(1) == 2.0); }
    SECTION("Invalid index") {
        REQUIRE_THROWS_AS(p.coord(3), std::out_of_range);
    }
    SECTION("Is read-only") {
        using r_type = std::remove_reference_t<decltype(p.coord(0))>;
        STATIC_REQUIRE(std::is_const_v<r_type>);
    }
}
