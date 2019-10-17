#include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::detail_;
using vector_t = std::vector<double>;
using array_t  = std::array<vector_t, 2>;

template<typename T, typename U>
static void check_state(T&& cgto, U&& corr) {
    SECTION("size") { REQUIRE(cgto.size() == corr[0].size()); }
    SECTION("coefficients") {
        for(std::size_t i = 0; i < cgto.size(); ++i)
            REQUIRE(cgto.coef(i) == corr[0][i]);
    }
    SECTION("exponents") {
        for(std::size_t i = 0; i < cgto.size(); ++i)
            REQUIRE(cgto.exp(i) == corr[1][i]);
    }
}

TEST_CASE("CGTOPIMPL<double> : default ctor") {
    CGTOPIMPL<double> p;
    check_state(p, array_t{});
}

TEST_CASE("CGTOPIMPL<double> : value ctor") {
    vector_t corr_cs{1.0, 2.0, 3.0};
    vector_t corr_es{4.0, 5.0, 6.0};
    utilities::MathSet<double> cs(corr_cs.begin(), corr_cs.end());
    utilities::MathSet<double> es(corr_es.begin(), corr_es.end());
    SECTION("Owning") {
        CGTOPIMPL<double> p{cs, es};
        check_state(p, array_t{corr_cs, corr_es});
    }
    SECTION("Aliasing") {
        CGTOPIMPL<double> p{&cs, &es};
        check_state(p, array_t{corr_cs, corr_es});
        SECTION("Are aliases") {
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&p.coef(i) == &cs[i]);
                REQUIRE(&p.exp(i) == &es[i]);
            }
        }
    }
}

TEST_CASE("CGTOPIMPL<double> : size()") {
    vector_t corr_cs{1.0, 2.0, 3.0};
    vector_t corr_es{4.0, 5.0, 6.0};
    utilities::MathSet<double> cs(corr_cs.begin(), corr_cs.end());
    utilities::MathSet<double> es(corr_es.begin(), corr_es.end());
    SECTION("Owning") {
        CGTOPIMPL<double> p{cs, es};
        REQUIRE(p.size() == 3);
    }
    SECTION("Aliasing") {
        CGTOPIMPL<double> p{&cs, &es};
        REQUIRE(p.size() == 3);
    }
}

TEST_CASE("CGTOPIMPL<double> : coef()") {
    vector_t corr_cs{1.0, 2.0, 3.0};
    vector_t corr_es{4.0, 5.0, 6.0};
    utilities::MathSet<double> cs(corr_cs.begin(), corr_cs.end());
    utilities::MathSet<double> es(corr_es.begin(), corr_es.end());
    SECTION("Owning") {
        CGTOPIMPL<double> p{cs, es};
        SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.coef(0)), double&>);
        }
        SECTION("Is copy") { REQUIRE(&p.coef(0) != &cs[0]); }
    }
    SECTION("Aliasing") {
        CGTOPIMPL<double> p{&cs, &es};
        SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.coef(0)), double&>);
        }
        SECTION("Is alias") { REQUIRE(&p.coef(0) == &cs[0]); }
    }
}

TEST_CASE("CGTOPIMPL<double> : coef() const") {
    vector_t corr_cs{1.0, 2.0, 3.0};
    vector_t corr_es{4.0, 5.0, 6.0};
    utilities::MathSet<double> cs(corr_cs.begin(), corr_cs.end());
    utilities::MathSet<double> es(corr_es.begin(), corr_es.end());
    SECTION("Owning") {
        const CGTOPIMPL<double> p{cs, es};
        SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.coef(0)), const double&>);
        }
        SECTION("Is copy") { REQUIRE(&p.coef(0) != &cs[0]); }
    }
    SECTION("Aliasing") {
        const CGTOPIMPL<double> p{&cs, &es};
        SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.coef(0)), const double&>);
        }
        SECTION("Is alias") { REQUIRE(&p.coef(0) == &cs[0]); }
    }
}

TEST_CASE("CGTOPIMPL<double> : exp()") {
    vector_t corr_cs{1.0, 2.0, 3.0};
    vector_t corr_es{4.0, 5.0, 6.0};
    utilities::MathSet<double> cs(corr_cs.begin(), corr_cs.end());
    utilities::MathSet<double> es(corr_es.begin(), corr_es.end());
    SECTION("Owning") {
        CGTOPIMPL<double> p{cs, es};
        SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.exp(0)), double&>);
        }
        SECTION("Is copy") { REQUIRE(&p.exp(0) != &es[0]); }
    }
    SECTION("Aliasing") {
        CGTOPIMPL<double> p{&cs, &es};
        SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.exp(0)), double&>);
        }
        SECTION("Is alias") { REQUIRE(&p.exp(0) == &es[0]); }
    }
}

TEST_CASE("CGTOPIMPL<double> : exp() const") {
    vector_t corr_cs{1.0, 2.0, 3.0};
    vector_t corr_es{4.0, 5.0, 6.0};
    utilities::MathSet<double> cs(corr_cs.begin(), corr_cs.end());
    utilities::MathSet<double> es(corr_es.begin(), corr_es.end());
    SECTION("Owning") {
        const CGTOPIMPL<double> p{cs, es};
        SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.exp(0)), const double&>);
        }
        SECTION("Is copy") { REQUIRE(&p.exp(0) != &es[0]); }
    }
    SECTION("Aliasing") {
        const CGTOPIMPL<double> p{&cs, &es};
        SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
        SECTION("Is reference") {
            STATIC_REQUIRE(std::is_same_v<decltype(p.exp(0)), const double&>);
        }
        SECTION("Is alias") { REQUIRE(&p.exp(0) == &es[0]); }
    }
}
