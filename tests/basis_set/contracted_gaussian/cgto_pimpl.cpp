#include "chemist/basis_set/contracted_gaussian/cgto_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace chemist::detail_;
using vector_t = std::vector<double>;
using array_t  = std::array<vector_t, 2>;

template<typename T, typename U>
static void check_state(T&& cgto, U&& corr) {
    SECTION("size") { REQUIRE(cgto.size() == corr[0].size()); }
    SECTION("coefficients") {
        for(std::size_t i = 0; i < cgto.size(); ++i)
            REQUIRE(cgto.at(i)->coef() == corr[0][i]);
    }
    SECTION("exponents") {
        for(std::size_t i = 0; i < cgto.size(); ++i)
            REQUIRE(cgto.at(i)->exp() == corr[1][i]);
    }
}

static inline auto make_ctgo() {
    vector_t corr_cs{1.0, 2.0, 3.0};
    vector_t corr_es{4.0, 5.0, 6.0};
    CGTOPIMPL<double> p(corr_cs, corr_es);
    return std::make_pair(p, std::array{corr_cs, corr_es});
}

TEST_CASE("CGTOPIMPL<double> : default ctor") {
    CGTOPIMPL<double> p;
    check_state(p, array_t{});
}

TEST_CASE("CGTOPIMPL<double> : value ctor") {
    auto [p, corr] = make_ctgo();
    SECTION("Owning") { check_state(p, corr); }
    SECTION("Aliasing") {
        CGTOPIMPL<double> p2(&corr[0], &corr[1]);
        check_state(p2, corr);
        SECTION("Are aliases") {
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&p2.at(i)->coef() == &corr[0][i]);
                REQUIRE(&p2.at(i)->exp() == &corr[1][i]);
            }
        }
    }
}

TEST_CASE("CGTOPIMPL<double> : size()") {
    auto [p, corr] = make_ctgo();
    SECTION("Owning") { REQUIRE(p.size() == 3); }
    SECTION("Aliasing") {
        CGTOPIMPL<double> p2(&corr[0], &corr[1]);
        REQUIRE(p2.size() == 3);
    }
}

TEST_CASE("CGTOPIMPL<double> : at() const") {
    auto [p, corr] = make_ctgo();
    SECTION("Value") {
        for(std::size_t i = 0; i < p.size(); ++i) {
            REQUIRE(p.at(i)->coef() == corr[0][i]);
            REQUIRE(p.at(i)->exp() == corr[1][i]);
        }
    }
}
