#include "chemist/basis_set/shell/shell_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace chemist::detail_;
using namespace chemist;
using vector_t = std::vector<double>;

template<typename T, typename U, typename V>
static void check_state(T&& pimpl, ShellType p, int l, std::size_t size, U&& cs,
                        V&& es) {
    SECTION("Purity") { REQUIRE(pimpl.purity() == p); }
    SECTION("Angular momentum") { REQUIRE(pimpl.l() == l); }
    SECTION("Size") { REQUIRE(pimpl.size() == size); }
    SECTION("CGTOs") {
        for(std::size_t i = 0; i < size; ++i) {
            auto cgto = pimpl.at(i);
            for(std::size_t j = 0; j < cs.size(); ++j) {
                REQUIRE(cgto->at(j)->coef() == cs[j]);
                REQUIRE(cgto->at(j)->exp() == es[j]);
            }
        }
    }
}

TEST_CASE("ShellPIMPL : default ctor") {
    ShellPIMPL<double> p;
    check_state(p, ShellType::cartesian, 0, 1, vector_t{}, vector_t{});
}

TEST_CASE("ShellPIMPL : copy ctor") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    ShellPIMPL<double> p(ShellType::pure, 0, cs, es);
    ShellPIMPL<double> p2(p);
    check_state(p2, ShellType::pure, 0, 1, cs, es);
    SECTION("Is copy") {
        REQUIRE(&p2.purity() != &p.purity());
        REQUIRE(&p2.l() != &p.l());
        REQUIRE(&p2.at(0)->at(0)->coef() != &p.at(0)->at(0)->coef());
        REQUIRE(&p2.at(0)->at(0)->exp() != &p.at(0)->at(0)->exp());
    }
}

TEST_CASE("ShellPIMPL : value ctor") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    ShellType pure = ShellType::cartesian;
    std::size_t l  = 1;
    SECTION("Copy version") {
        ShellPIMPL<double> p(pure, l, cs, es);
        check_state(p, ShellType::cartesian, 1, 3, cs, es);
        for(std::size_t i = 0; i < cs.size(); ++i) {
            REQUIRE(&p.at(0)->at(i)->coef() != &cs[i]);
            REQUIRE(&p.at(0)->at(i)->exp() != &es[i]);
        }
    }
    SECTION("Alias version") {
        ShellPIMPL<double> p(&pure, &l, &cs, &es);
        check_state(p, ShellType::cartesian, 1, 3, cs, es);
        REQUIRE(&p.purity() == &pure);
        REQUIRE(&p.l() == &l);
        for(std::size_t i = 0; i < cs.size(); ++i) {
            REQUIRE(&p.at(0)->at(i)->coef() == &cs[i]);
            REQUIRE(&p.at(0)->at(i)->exp() == &es[i]);
        }
    }
}

TEST_CASE("ShellPIMPL : purity()") {
    SECTION("Cartesian") {
        ShellPIMPL<double> p(ShellType::cartesian, 0, vector_t{}, vector_t{});
        SECTION("Value") { REQUIRE(p.purity() == ShellType::cartesian); }
        SECTION("Is read/write-able") {
            STATIC_REQUIRE(std::is_same_v<ShellType&, decltype(p.purity())>);
        }
    }
    SECTION("Spherical") {
        ShellPIMPL<double> p(ShellType::pure, 0, vector_t{}, vector_t{});
        SECTION("Value") { REQUIRE(p.purity() == ShellType::pure); }
        SECTION("Is read/write-able") {
            STATIC_REQUIRE(std::is_same_v<ShellType&, decltype(p.purity())>);
        }
    }
}

TEST_CASE("ShellPIMPL : purity() const") {
    SECTION("Cartesian") {
        const ShellPIMPL<double> p(ShellType::cartesian, 0, vector_t{},
                                   vector_t{});
        SECTION("Value") { REQUIRE(p.purity() == ShellType::cartesian); }
        SECTION("Is read-only") {
            STATIC_REQUIRE(
              std::is_same_v<const ShellType&, decltype(p.purity())>);
        }
    }
    SECTION("Spherical") {
        const ShellPIMPL<double> p(ShellType::pure, 0, vector_t{}, vector_t{});
        SECTION("Value") { REQUIRE(p.purity() == ShellType::pure); }
        SECTION("Is read-only") {
            STATIC_REQUIRE(
              std::is_same_v<const ShellType&, decltype(p.purity())>);
        }
    }
}

TEST_CASE("ShellPIMPL : l()") {
    ShellPIMPL<double> p(ShellType::pure, 0, vector_t{}, vector_t{});
    SECTION("Value") { REQUIRE(p.l() == 0); }
    SECTION("Is read/write-able") {
        STATIC_REQUIRE(std::is_same_v<std::size_t&, decltype(p.l())>);
    }
}

TEST_CASE("ShellPIMPL : l() const") {
    const ShellPIMPL<double> p(ShellType::pure, 0, vector_t{}, vector_t{});
    SECTION("Value") { REQUIRE(p.l() == 0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const std::size_t&, decltype(p.l())>);
    }
}

TEST_CASE("ShellPIMPL : size") {
    SECTION("Pure") {
        SECTION("l == 0") {
            ShellPIMPL<double> p(ShellType::pure, 0, vector_t{}, vector_t{});
            REQUIRE(p.size() == 1);
        }
        SECTION("l == 1") {
            ShellPIMPL<double> p(ShellType::pure, 1, vector_t{}, vector_t{});
            REQUIRE(p.size() == 3);
        }
        SECTION("l == 2") {
            ShellPIMPL<double> p(ShellType::pure, 2, vector_t{}, vector_t{});
            REQUIRE(p.size() == 5);
        }
        SECTION("l == 8") {
            ShellPIMPL<double> p(ShellType::pure, 8, vector_t{}, vector_t{});
            REQUIRE(p.size() == 17);
        }
    }
    SECTION("Cartesian") {
        SECTION("l == 0") {
            ShellPIMPL<double> p(ShellType::cartesian, 0, vector_t{},
                                 vector_t{});
            REQUIRE(p.size() == 1);
        }
        SECTION("l == 1") {
            ShellPIMPL<double> p(ShellType::cartesian, 1, vector_t{},
                                 vector_t{});
            REQUIRE(p.size() == 3);
        }
        SECTION("l == 2") {
            ShellPIMPL<double> p(ShellType::cartesian, 2, vector_t{},
                                 vector_t{});
            REQUIRE(p.size() == 6);
        }
        SECTION("l == 8") {
            ShellPIMPL<double> p(ShellType::cartesian, 8, vector_t{},
                                 vector_t{});
            REQUIRE(p.size() == 45);
        }
    }
}

TEST_CASE("ShellPIMPL : at") {
    vector_t c{1.0}, e{2.0};
    const ShellPIMPL<double> p(ShellType::pure, 0, c, e);
    REQUIRE(p.at(0)->at(0)->coef() == 1.0);
    REQUIRE(p.at(0)->at(0)->exp() == 2.0);
}
