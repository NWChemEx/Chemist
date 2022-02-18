#include "chemist/basis_set/contracted_gaussian.hpp"
#include "chemist/basis_set/contracted_gaussian/cgto_pimpl.hpp"
#include "chemist/point/point_pimpl.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

/* Testing Strategy:
 *
 * Prior testing of the Point<T> and utilities::IndexableContainerBase classes
 * ensures that most of the ContractedGaussian's API works. We need to test:
 *
 * - the ctors and assignment operators
 * - size_(), at_() are hooked up correctly, and
 * - comparisons
 */

using namespace chemist;
using vector_t = std::vector<double>;
using prim_t   = Primitive<double>;

template<typename T, typename U>
static void check_state(T&& ao, U&& corr) {
    SECTION("size") { REQUIRE(ao.size() == corr.size()); }
    SECTION("primitives") {
        for(std::size_t i = 0; i < ao.size(); ++i) REQUIRE(ao[i] == corr[i]);
    }
}

static auto make_ctgo() {
    prim_t p0{1.0, 2.0, 7.0, 8.0, 9.0};
    prim_t p1{3.0, 4.0, 7.0, 8.0, 9.0};
    prim_t p2{5.0, 6.0, 7.0, 8.0, 9.0};
    vector_t cs{1.0, 3.0, 5.0};
    vector_t es{2.0, 4.0, 6.0};
    ContractedGaussian<double> g(cs, es, 7.0, 8.0, 9.0);
    return std::make_pair(std::array{p0, p1, p2}, g);
}

TEST_CASE("ContractedGaussian<double> : default ctor") {
    ContractedGaussian<double> g;
    check_state(g, std::array<prim_t, 0>{});
}

TEST_CASE("ContractedGaussian<double> : value ctor") {
    auto [prims, g] = make_ctgo();
    check_state(g, prims);
}

TEST_CASE("ContractedGaussian<double> : copy ctor") {
    auto [prims, g] = make_ctgo();
    ContractedGaussian<double> g2(g);
    check_state(g2, prims);
    SECTION("Is deep copy") {
        for(std::size_t i = 0; i < g.size(); ++i) {
            REQUIRE(&g2[i].coefficient() != &g[i].coefficient());
            REQUIRE(&g2[i].exponent() != &g[i].exponent());
        }
    }
}

TEST_CASE("ContractedGaussian<double> : move ctor") {
    auto [prims, g] = make_ctgo();
    ContractedGaussian<double> g2(std::move(g));
    check_state(g2, prims);
}

TEST_CASE("ContractedGaussian<double> : copy assignment") {
    auto [prims, g] = make_ctgo();
    ContractedGaussian<double> g2;
    auto pg2 = &(g2 = g);
    check_state(g2, prims);
    SECTION("Returns this") { REQUIRE(&g2 == pg2); }
    SECTION("Is deep copy") {
        for(std::size_t i = 0; i < g.size(); ++i) {
            REQUIRE(&g2[i].coefficient() != &g[i].coefficient());
            REQUIRE(&g2[i].exponent() != &g[i].exponent());
        }
    }
}

TEST_CASE("ContractedGaussian<double> : move assignment") {
    auto [prims, g] = make_ctgo();
    ContractedGaussian<double> g2;
    auto pg2 = &(g2 = std::move(g));
    check_state(g2, prims);
    SECTION("Returns this") { REQUIRE(&g2 == pg2); }
}

TEST_CASE("ContractedGaussian<double> : pimpl ctor") {
    vector_t cs{1.0, 3.0, 5.0};
    vector_t as{2.0, 4.0, 6.0};
    std::array<double, 3> qs{7.0, 8.0, 9.0};
    auto p1 = std::make_unique<detail_::CGTOPIMPL<double>>(&cs, &as);
    auto p2 =
      std::make_unique<detail_::PointPIMPL<double>>(&qs[0], &qs[1], &qs[2]);
    ContractedGaussian<double> cgto(std::move(p1), std::move(p2));
    auto [prims, g] = make_ctgo();
    check_state(cgto, prims);
}

TEST_CASE("ContractedGaussian<double> : size_") {
    auto [prims, g] = make_ctgo();
    REQUIRE(g.size() == 3);
}

TEST_CASE("ContractedGaussian<double> : at_") {
    auto [prims, g] = make_ctgo();
    auto prim0      = g[0];
    SECTION("State") { REQUIRE(static_cast<const prim_t&>(prim0) == prims[0]); }
    SECTION("Is read/write-able") {
        STATIC_REQUIRE(std::is_same_v<double&, decltype(prim0.coefficient())>);
        STATIC_REQUIRE(std::is_same_v<double&, decltype(prim0.exponent())>);
    }
    SECTION("Throws for out of range") {
        REQUIRE_THROWS_AS(g[9], std::out_of_range);
    }
}

TEST_CASE("ContractedGaussian<double> : at_() const") {
    const auto [prims, g] = make_ctgo();
    auto prim0            = g[0];
    SECTION("State") { REQUIRE(static_cast<const prim_t&>(prim0) == prims[0]); }
    SECTION("Is read-only") {
        using constd = const double&;
        STATIC_REQUIRE(std::is_same_v<constd, decltype(prim0.coefficient())>);
        STATIC_REQUIRE(std::is_same_v<constd, decltype(prim0.exponent())>);
    }
    SECTION("Throws for out of range") {
        REQUIRE_THROWS_AS(g[9], std::out_of_range);
    }
}

TEST_CASE("ContractedGaussian<double> : operator==") {
    const auto [prims, g] = make_ctgo();
    SECTION("Same") { REQUIRE(g == g); }
    SECTION("Different primitives") {
        vector_t cs{1.0, 3.0, 5.0};
        vector_t es{1.0, 2.0, 3.0};
        ContractedGaussian<double> g2(cs, es, 7.0, 8.0, 9.0);
        REQUIRE_FALSE(g == g2);
    }
    SECTION("Different center") {
        vector_t cs{1.0, 3.0, 5.0};
        vector_t es{2.0, 4.0, 6.0};
        ContractedGaussian<double> g2(cs, es, 1.0, 2.0, 3.0);
        REQUIRE_FALSE(g == g2);
    }
}

TEST_CASE("ContractedGaussian serialization") {
    auto [prims, g] = make_ctgo();
    ContractedGaussian<double> g2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(g);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(g2);
    }
    REQUIRE(g == g2);
}
