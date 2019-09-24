#include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::detail_;
using owning_t = OwningCGTOPIMPL<double>;
using alias_t  = AliasingCGTOPIMPL<double>;
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

TEST_CASE("OwningCGTOPIPML<double> : default ctor") {
    owning_t p;
    check_state(p, array_t{});
}

TEST_CASE("AliasingCGTOPIPML<double> : default ctor") {
    alias_t p;
    check_state(p, array_t{});
}

TEST_CASE("OwningCGTOPIMPL<double> : value ctor") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    owning_t p{cs, es};
    check_state(p, array_t{cs, es});
}

TEST_CASE("AliasingCGTOPIMPL<double> : value ctor") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    std::vector pcs{(&cs[0]), (&cs[1]), (&cs[2])};
    std::vector pes{(&es[0]), (&es[1]), (&es[2])};
    alias_t p{pcs, pes};
    check_state(p, array_t{cs, es});
    SECTION("Are aliases") {
        for(std::size_t i = 0; i < 3; ++i) {
            REQUIRE(&p.coef(i) == &cs[i]);
            REQUIRE(&p.exp(i) == &es[i]);
        }
    }
}

TEST_CASE("OwningCGTOPIMPL<double> : size") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    owning_t p{cs, es};
    REQUIRE(p.size() == 3);
}

TEST_CASE("AliasingCGTOPIMPL<double> : size") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    std::vector pcs{(&cs[0]), (&cs[1]), (&cs[2])};
    std::vector pes{(&es[0]), (&es[1]), (&es[2])};
    alias_t p{pcs, pes};
    REQUIRE(p.size() == 3);
}

TEST_CASE("OwningCGTOPIMPL<double> : coef") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    owning_t p{cs, es};
    SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.coef(3), std::out_of_range);
    }
}

TEST_CASE("AliasingCGTOPIMPL<double> : coef") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    std::vector pcs{(&cs[0]), (&cs[1]), (&cs[2])};
    std::vector pes{(&es[0]), (&es[1]), (&es[2])};
    alias_t p{pcs, pes};
    SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
    SECTION("Is alias") { REQUIRE(&p.coef(0) == &cs[0]); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.coef(3), std::out_of_range);
    }
}

TEST_CASE("OwningCGTOPIMPL<double> : coef const") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    const owning_t p{cs, es};
    SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.coef(3), std::out_of_range);
    }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.coef(1))>);
    }
}

TEST_CASE("AliasingCGTOPIMPL<double> : coef const") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    std::vector pcs{(&cs[0]), (&cs[1]), (&cs[2])};
    std::vector pes{(&es[0]), (&es[1]), (&es[2])};
    const alias_t p{pcs, pes};
    SECTION("Value") { REQUIRE(p.coef(0) == 1.0); }
    SECTION("Is alias") { REQUIRE(&p.coef(0) == &cs[0]); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.coef(3), std::out_of_range);
    }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.coef(1))>);
    }
}

TEST_CASE("OwningCGTOPIMPL<double> : exp") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    owning_t p{cs, es};
    SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.exp(3), std::out_of_range);
    }
}

TEST_CASE("AliasingCGTOPIMPL<double> : exp") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    std::vector pcs{(&cs[0]), (&cs[1]), (&cs[2])};
    std::vector pes{(&es[0]), (&es[1]), (&es[2])};
    alias_t p{pcs, pes};
    SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
    SECTION("Is alias") { REQUIRE(&p.exp(0) == &es[0]); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.exp(3), std::out_of_range);
    }
}

TEST_CASE("OwningCGTOPIMPL<double> : exp const") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    const owning_t p{cs, es};
    SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.exp(3), std::out_of_range);
    }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.exp(1))>);
    }
}

TEST_CASE("AliasingCGTOPIMPL<double> : exp const") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    std::vector pcs{(&cs[0]), (&cs[1]), (&cs[2])};
    std::vector pes{(&es[0]), (&es[1]), (&es[2])};
    const alias_t p{pcs, pes};
    SECTION("Value") { REQUIRE(p.exp(0) == 4.0); }
    SECTION("Is alias") { REQUIRE(&p.exp(0) == &es[0]); }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(p.exp(3), std::out_of_range);
    }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.exp(1))>);
    }
}
