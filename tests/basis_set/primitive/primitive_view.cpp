#include "chemist/basis_set/primitive_view.hpp"
#include <catch2/catch.hpp>

using namespace chemist;

template<typename T, typename U>
static void compare_state(T&& view, U&& corr) {
    SECTION("coefficient") { REQUIRE(&view.coefficient() == &corr[0]); }
    SECTION("exponent") { REQUIRE(&view.exponent() == &corr[1]); }
    SECTION("x-coordinate") { REQUIRE(&view.x() == &corr[2]); }
    SECTION("y-coordinate") { REQUIRE(&view.y() == &corr[3]); }
    SECTION("z-coordinate") { REQUIRE(&view.z() == &corr[4]); }
}

TEST_CASE("PrimitiveView<double> : default ctor") {
    PrimitiveView<double> p; // Basically just making sure it compiles
}

TEST_CASE("PrimitiveView<double> : aliasing ctor") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    compare_state(pv, v);
}

TEST_CASE("PrimitiveView<const double> : aliasing ctor") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    compare_state(pv, v);
}

TEST_CASE("PrimitiveView<double> : copy ctor") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<double> pv2(pv);
    compare_state(pv2, v);
}

TEST_CASE("PrimitiveView<const double> : copy ctor") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<const double> pv2(pv);
    compare_state(pv2, v);
}

TEST_CASE("PrimitiveView<double> : move ctor") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<double> pv2(std::move(pv));
    compare_state(pv2, v);
}

TEST_CASE("PrimitiveView<const double> : move ctor") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<const double> pv2(std::move(pv));
    compare_state(pv2, v);
}

TEST_CASE("PrimitiveView<double> : copy assignment") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<double> pv2;
    auto ppv2 = &(pv2 = pv);
    compare_state(pv2, v);
    SECTION("Returns this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PrimitiveView<const double> : copy assignment") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<const double> pv2;
    auto ppv2 = &(pv2 = pv);
    compare_state(pv2, v);
    SECTION("Returns this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PrimitiveView<double> : move assignment") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<double> pv2;
    auto ppv2 = &(pv2 = std::move(pv));
    compare_state(pv2, v);
    SECTION("Returns this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PrimitiveView<const double> : move assignment") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<const double> pv2;
    auto ppv2 = &(pv2 = std::move(pv));
    compare_state(pv2, v);
    SECTION("Returns this") { REQUIRE(ppv2 == &pv2); }
}

TEST_CASE("PrimitiveView<double> : coefficient") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coefficient() == &v[0]); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.coefficient())>);
    }
}

TEST_CASE("PrimitiveView<const double> : coefficient") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coefficient() == &v[0]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(
          std::is_same_v<const double&, decltype(pv.coefficient())>);
    }
}

TEST_CASE("PrimitiveView<double> : coefficient const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coefficient() == &v[0]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(
          std::is_same_v<const double&, decltype(pv.coefficient())>);
    }
}

TEST_CASE("PrimitiveView<const double> : coefficient const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coefficient() == &v[0]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(
          std::is_same_v<const double&, decltype(pv.coefficient())>);
    }
}

TEST_CASE("PrimitiveView<double> : exponent") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.exponent() == &v[1]); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.exponent())>);
    }
}

TEST_CASE("PrimitiveView<const double> : exponent") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.exponent() == &v[1]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.exponent())>);
    }
}

TEST_CASE("PrimitiveView<double> : exponent const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.exponent() == &v[1]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.exponent())>);
    }
}

TEST_CASE("PrimitiveView<const double> : exponent const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.exponent() == &v[1]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.exponent())>);
    }
}

TEST_CASE("PrimitiveView<double> : coord") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coord(1) == &v[3]); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.coord(1))>);
    }
}

TEST_CASE("PrimitiveView<const double> : coord") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coord(1) == &v[3]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(1))>);
    }
}

TEST_CASE("PrimitiveView<double> : coord const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coord(1) == &v[3]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(2))>);
    }
}

TEST_CASE("PrimitiveView<const double> : coord const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.coord(1) == &v[3]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.coord(1))>);
    }
}

TEST_CASE("PrimitiveView<double> : x") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.x() == &v[2]); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.x())>);
    }
}

TEST_CASE("PrimitiveView<const double> : x") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.x() == &v[2]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PrimitiveView<double> : x const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.x() == &v[2]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PrimitiveView<const double> : x const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.x() == &v[2]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.x())>);
    }
}

TEST_CASE("PrimitiveView<double> : y") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.y() == &v[3]); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.y())>);
    }
}

TEST_CASE("PrimitiveView<const double> : y") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.y() == &v[3]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PrimitiveView<double> : y const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.y() == &v[3]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PrimitiveView<const double> : y const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.y() == &v[3]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.y())>);
    }
}

TEST_CASE("PrimitiveView<double> : z") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.z() == &v[4]); }
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(pv.z())>);
    }
}

TEST_CASE("PrimitiveView<const double> : z") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.z() == &v[4]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PrimitiveView<double> : z const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.z() == &v[4]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PrimitiveView<const double> : z const") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    const PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    SECTION("Is alias") { REQUIRE(&pv.z() == &v[4]); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(pv.z())>);
    }
}

TEST_CASE("PrimitiveView<double> : implicit conversion") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);

    SECTION("To const Primitive<double>&") {
        const Primitive<double>& p(pv);
        compare_state(p, v);
    }
    SECTION("To Primitive<double>") {
        Primitive<double> p(pv);
        SECTION("State") {
            SECTION("Coefficient") { REQUIRE(p.coefficient() == v[0]); }
            SECTION("Exponent") { REQUIRE(p.exponent() == v[1]); }
            SECTION("x-coordinate") { REQUIRE(p.x() == v[2]); }
            SECTION("y-coordinate") { REQUIRE(p.y() == v[3]); }
            SECTION("z-coordinate") { REQUIRE(p.z() == v[4]); }
        }
        SECTION("Is deep copy") {
            SECTION("Coefficient") { REQUIRE(&p.coefficient() != &v[0]); }
            SECTION("Exponent") { REQUIRE(&p.exponent() != &v[1]); }
            SECTION("x-coordinate") { REQUIRE(&p.x() != &v[2]); }
            SECTION("y-coordinate") { REQUIRE(&p.y() != &v[3]); }
            SECTION("z-coordinate") { REQUIRE(&p.z() != &v[4]); }
        }
    }
}

TEST_CASE("PrimitiveView<const double> : implicit conversion") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);

    SECTION("To const Primitive<double>&") {
        const Primitive<double>& p(pv);
        compare_state(p, v);
    }
    SECTION("To Primitive<double>") {
        Primitive<double> p(pv);
        SECTION("State") {
            SECTION("Coefficient") { REQUIRE(p.coefficient() == v[0]); }
            SECTION("Exponent") { REQUIRE(p.exponent() == v[1]); }
            SECTION("x-coordinate") { REQUIRE(p.x() == v[2]); }
            SECTION("y-coordinate") { REQUIRE(p.y() == v[3]); }
            SECTION("z-coordinate") { REQUIRE(p.z() == v[4]); }
        }
        SECTION("Is deep copy") {
            SECTION("Coefficient") { REQUIRE(&p.coefficient() != &v[0]); }
            SECTION("Exponent") { REQUIRE(&p.exponent() != &v[1]); }
            SECTION("x-coordinate") { REQUIRE(&p.x() != &v[2]); }
            SECTION("y-coordinate") { REQUIRE(&p.y() != &v[3]); }
            SECTION("z-coordinate") { REQUIRE(&p.z() != &v[4]); }
        }
    }
}

TEST_CASE("PrimitiveView<double> : operator==") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    Primitive<double> p{1.0, 2.0, 3.0, 4.0, 5.0};
    SECTION("PrimitiveView<double> == PrimitiveView<double>") {
        REQUIRE(pv == pv);
    }
    SECTION("PrimitiveView<double> == Primitive<double>") { REQUIRE(pv == p); }
    SECTION("Primitive<double> == PrimitiveView<double>") { REQUIRE(p == pv); }
}

TEST_CASE("PrimitiveView<const double> : operator==") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<double> pv2(&v[0], &v[1], &v[2], &v[3], &v[4]);
    Primitive<double> p{1.0, 2.0, 3.0, 4.0, 5.0};
    SECTION("PrimitiveView<const double> == PrimitiveView<const double>") {
        REQUIRE(pv == pv);
    }
    SECTION("PrimitiveView<const double> == PrimitiveView<double>") {
        REQUIRE(pv == pv2);
    }
    SECTION("PrimitiveView<double> == PrimitiveView<const double>") {
        REQUIRE(pv2 == pv);
    }
    SECTION("PrimitiveView<const double> == Primitive<double>") {
        REQUIRE(pv == p);
    }
    SECTION("Primitive<double> == PrimitiveView<const double>") {
        REQUIRE(p == pv);
    }
}

TEST_CASE("PrimitiveView<double> : operator!=") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    Primitive<double> p{1.0, 2.0, 3.0, 4.0, 5.0};
    SECTION("PrimitiveView<double> != PrimitiveView<double>") {
        REQUIRE_FALSE(pv != pv);
    }
    SECTION("PrimitiveView<double> != Primitive<double>") {
        REQUIRE_FALSE(pv != p);
    }
    SECTION("Primitive<double> == PrimitiveView<double>") {
        REQUIRE_FALSE(p != pv);
    }
}

TEST_CASE("PrimitiveView<const double> : operator!=") {
    std::vector<double> v{1.0, 2.0, 3.0, 4.0, 5.0};
    PrimitiveView<const double> pv(&v[0], &v[1], &v[2], &v[3], &v[4]);
    PrimitiveView<double> pv2(&v[0], &v[1], &v[2], &v[3], &v[4]);
    Primitive<double> p{1.0, 2.0, 3.0, 4.0, 5.0};
    SECTION("PrimitiveView<const double> != PrimitiveView<const double>") {
        REQUIRE_FALSE(pv != pv);
    }
    SECTION("PrimitiveView<const double> != PrimitiveView<double>") {
        REQUIRE_FALSE(pv != pv2);
    }
    SECTION("PrimitiveView<double> != PrimitiveView<const double>") {
        REQUIRE_FALSE(pv2 != pv);
    }
    SECTION("PrimitiveView<const double> != Primitive<double>") {
        REQUIRE_FALSE(pv != p);
    }
    SECTION("Primitive<double> != PrimitiveView<const double>") {
        REQUIRE_FALSE(p != pv);
    }
}
