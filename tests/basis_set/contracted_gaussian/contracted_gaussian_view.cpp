#include "chemist/basis_set/contracted_gaussian_view.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using prim_t = Primitive<double>;

/* Testing strategy
 *
 * Testing the IndexableContainerBase class also tests most of
 * ContractedGaussianView's API. Most of ContractedGaussianView's implementation
 * is tested for us by virtue of wrapping a ContractedGaussian instance. We need
 * to still test:
 *
 * - ctors
 * - implicit-conversions
 * - const-correctness
 */

template<typename T, typename U>
static void compare_state(T&& view, U&& corr) {
    SECTION("Size") { REQUIRE(view.size() == corr.size()); }
    SECTION("State") {
        for(std::size_t i = 0; i < view.size(); ++i) {
            REQUIRE(view[i].coefficient() == corr[i].coefficient());
            REQUIRE(view[i].exponent() == corr[i].exponent());
            REQUIRE(view[i] == corr[i]);
        }
    }
}

template<typename T>
static auto make_ctgo() {
    prim_t p0{1.0, 2.0, 7.0, 8.0, 9.0};
    prim_t p1{3.0, 4.0, 7.0, 8.0, 9.0};
    prim_t p2{5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<double> cs{1.0, 3.0, 5.0};
    std::vector<double> es{2.0, 4.0, 6.0};
    ContractedGaussian<double> g(cs, es, 7.0, 8.0, 9.0);
    ContractedGaussianView<T> g2(std::move(g));
    return std::make_pair(std::array{p0, p1, p2}, g2);
}

TEST_CASE("ContractedGaussianView<double> : default ctor") {
    ContractedGaussianView<double> v; // Basically just making sure it compiles
}

TEST_CASE("ContractedGaussianView<double> : value ctor") {
    auto&& [prims, g] = make_ctgo<double>();
    compare_state(prims, g);
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<PrimitiveView<double>, decltype(g[0])>);
    }
}

TEST_CASE("ContractedGaussianView<const double> : alias ctor") {
    auto&& [prims, g] = make_ctgo<double>();
    compare_state(prims, g);
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<PrimitiveView<double>, decltype(g[0])>);
    }
}

TEST_CASE("ContractedGaussianView<double> : implicit conversion") {
    auto&& [prims, g] = make_ctgo<double>();
    const ContractedGaussian<double>& g2(g);
    compare_state(g2, prims);
}

TEST_CASE("ContractedGaussianView<double> : operator==") {
    auto [ps, g] = make_ctgo<double>();
    ContractedGaussian<double> g2;
    SECTION("View == Value") { REQUIRE_FALSE(g == g2); }
    SECTION("Value == View") { REQUIRE_FALSE(g2 == g); }
    SECTION("View == View") { REQUIRE(g == g); }
}

TEST_CASE("ContractedGaussianView<double> : operator!=") {
    auto [ps, g] = make_ctgo<double>();
    ContractedGaussian<double> g2;
    SECTION("View != Value") { REQUIRE(g != g2); }
    SECTION("Value != View") { REQUIRE(g2 != g); }
    SECTION("View != View") { REQUIRE_FALSE(g != g); }
}
