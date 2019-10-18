#include <catch2/catch.hpp>
#include <libchemist/basis_set/shell/detail_/shell_pimpl.hpp>

using namespace libchemist::detail_;
using vector_t = std::vector<double>;
using set_t    = utilities::MathSet<double>;

template<typename T, typename U, typename V>
static void check_state(T&& pimpl, bool p, int l, std::size_t size, U&& cs,
                        V&& es) {
    SECTION("Purity") { REQUIRE(pimpl.purity() == p); }
    SECTION("Angular momentum") { REQUIRE(pimpl.l() == l); }
    SECTION("Size") { REQUIRE(pimpl.size() == size); }
    SECTION("CGTOs") {
        for(std::size_t i = 0; i < size; ++i) {
            auto cgto = pimpl.at(i);
            for(std::size_t j = 0; j < cs.size(); ++j) {
                REQUIRE(cgto->coef(j) == cs[j]);
                REQUIRE(cgto->exp(j) == es[j]);
            }
        }
    }
}

TEST_CASE("ShellPIMPL : default ctor") {
    ShellPIMPL<double> p;
    check_state(p, false, 0, 1, vector_t{}, vector_t{});
}

TEST_CASE("ShellPIMPL : copy ctor") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    set_t cs_set(cs.begin(), cs.end());
    set_t es_set(es.begin(), es.end());
    ShellPIMPL<double> p(true, 0, cs_set, es_set);
    ShellPIMPL<double> p2(p);
    check_state(p2, true, 0, 1, cs, es);
    SECTION("Is copy") {
        REQUIRE(&p2.purity() != &p.purity());
        REQUIRE(&p2.l() != &p.l());
        REQUIRE(&p2.at(0)->coef(0) != &p.at(0)->coef(0));
        REQUIRE(&p2.at(0)->exp(0) != &p.at(0)->exp(0));
    }
}

TEST_CASE("ShellPIMPL : value ctor") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    set_t cs_set(cs.begin(), cs.end());
    set_t es_set(es.begin(), es.end());
    SECTION("Copy version") {
        ShellPIMPL<double> p(true, 0, cs_set, es_set);
        check_state(p, true, 0, 1, cs, es);
        for(std::size_t i = 0; i < cs.size(); ++i) {
            REQUIRE(&p.at(0)->coef(i) != &cs_set[i]);
            REQUIRE(&p.at(0)->exp(i) != &es_set[i]);
        }
    }
    SECTION("Alias version") {
        bool pure     = false;
        std::size_t l = 1;
        ShellPIMPL<double> p(&pure, &l, &cs_set, &es_set);
        check_state(p, false, 1, 3, cs, es);
        REQUIRE(&p.purity() == &pure);
        REQUIRE(&p.l() == &l);
        for(std::size_t i = 0; i < cs.size(); ++i) {
            REQUIRE(&p.at(0)->coef(i) == &cs_set[i]);
            REQUIRE(&p.at(0)->exp(i) == &es_set[i]);
        }
    }
}

TEST_CASE("ShellPIMPL : purity()") {
    SECTION("Cartesian") {
        ShellPIMPL<double> p(false, 0, set_t{}, set_t{});
        SECTION("Value") { REQUIRE_FALSE(p.purity()); }
        SECTION("Is read/write-able") {
            STATIC_REQUIRE(std::is_same_v<bool&, decltype(p.purity())>);
        }
    }
    SECTION("Spherical") {
        ShellPIMPL<double> p(true, 0, set_t{}, set_t{});
        SECTION("Value") { REQUIRE(p.purity()); }
        SECTION("Is read/write-able") {
            STATIC_REQUIRE(std::is_same_v<bool&, decltype(p.purity())>);
        }
    }
}

TEST_CASE("ShellPIMPL : purity() const") {
    SECTION("Cartesian") {
        const ShellPIMPL<double> p(false, 0, set_t{}, set_t{});
        SECTION("Value") { REQUIRE_FALSE(p.purity()); }
        SECTION("Is read-only") {
            STATIC_REQUIRE(std::is_same_v<const bool&, decltype(p.purity())>);
        }
    }
    SECTION("Spherical") {
        const ShellPIMPL<double> p(true, 0, set_t{}, set_t{});
        SECTION("Value") { REQUIRE(p.purity()); }
        SECTION("Is read-only") {
            STATIC_REQUIRE(std::is_same_v<const bool&, decltype(p.purity())>);
        }
    }
}

TEST_CASE("ShellPIMPL : l()") {
    ShellPIMPL<double> p(true, 0, set_t{}, set_t{});
    SECTION("Value") { REQUIRE(p.l() == 0); }
    SECTION("Is read/write-able") {
        STATIC_REQUIRE(std::is_same_v<std::size_t&, decltype(p.l())>);
    }
}

TEST_CASE("ShellPIMPL : l() const") {
    const ShellPIMPL<double> p(true, 0, set_t{}, set_t{});
    SECTION("Value") { REQUIRE(p.l() == 0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const std::size_t&, decltype(p.l())>);
    }
}

TEST_CASE("ShellPIMPL : size") {
    SECTION("Pure") {
        SECTION("l == 0") {
            ShellPIMPL<double> p(true, 0, set_t{}, set_t{});
            REQUIRE(p.size() == 1);
        }
        SECTION("l == 1") {
            ShellPIMPL<double> p(true, 1, set_t{}, set_t{});
            REQUIRE(p.size() == 3);
        }
        SECTION("l == 2") {
            ShellPIMPL<double> p(true, 2, set_t{}, set_t{});
            REQUIRE(p.size() == 5);
        }
        SECTION("l == 8") {
            ShellPIMPL<double> p(true, 8, set_t{}, set_t{});
            REQUIRE(p.size() == 17);
        }
    }
    SECTION("Cartesian") {
        SECTION("l == 0") {
            ShellPIMPL<double> p(false, 0, set_t{}, set_t{});
            REQUIRE(p.size() == 1);
        }
        SECTION("l == 1") {
            ShellPIMPL<double> p(false, 1, set_t{}, set_t{});
            REQUIRE(p.size() == 3);
        }
        SECTION("l == 2") {
            ShellPIMPL<double> p(false, 2, set_t{}, set_t{});
            REQUIRE(p.size() == 6);
        }
        SECTION("l == 8") {
            ShellPIMPL<double> p(false, 8, set_t{}, set_t{});
            REQUIRE(p.size() == 45);
        }
    }
}

TEST_CASE("ShellPIMPL : at") {
    vector_t c{1.0}, e{2.0};
    set_t cs(c.begin(), c.end()), es(e.begin(), e.end());
    const ShellPIMPL<double> p(true, 0, cs, es);
    REQUIRE(p.at(0)->coef(0) == 1.0);
    REQUIRE(p.at(0)->exp(0) == 2.0);
}
