#include <catch2/catch.hpp>
#include <libchemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp>

using namespace libchemist;
using prim_t = Primitive<double>;

/* Testing strategy
 *
 * The IndexableContainerBase class tests most of the ContractedGaussianView's
 * API for us. What it doesn't test are the ctors, whether or not we compute
 * the size and i-th elements correctly, and whether we can implicitly convert
 * to a ContractedGaussian correctly. We need to consider const-correctness too.
 */

template<typename T, typename U>
static void compare_state(T&& view, U&& corr) {
    SECTION("Size") { REQUIRE(view.size() == corr.size()); }
    SECTION("State") {
        for(std::size_t i = 0; i < view.size(); ++i) {
            REQUIRE(&view[i].coefficient() == &corr[i].coefficient());
            REQUIRE(&view[i].exponent() == &corr[i].exponent());
            REQUIRE(view[i] == corr[i]);
        }
    }
}

TEST_CASE("ContractedGaussianView<double> : default ctor") {
    ContractedGaussianView<double> v; // Basically just making sure it compiles
}

TEST_CASE("ContractedGaussianView<double> : alias ctor") {
    prim_t p0{1.0, 2.0, 7.0, 8.0, 9.0};
    prim_t p1{3.0, 4.0, 7.0, 8.0, 9.0};
    prim_t p2{5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<double*> cs{&p0.coefficient(), &p1.coefficient(),
                            &p2.coefficient()};
    std::vector<double*> es{&p0.exponent(), &p1.exponent(), &p2.exponent()};
    ContractedGaussianView<double> g(cs, es, &p0.x(), &p0.y(), &p0.z());
    std::array prims{std::move(p0), std::move(p1), std::move(p2)};
    compare_state(prims, g);
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<PrimitiveView<double>, decltype(g[0])>);
    }
}

TEST_CASE("ContractedGaussianView<const double> : alias ctor") {
    prim_t p0{1.0, 2.0, 7.0, 8.0, 9.0};
    prim_t p1{3.0, 4.0, 7.0, 8.0, 9.0};
    prim_t p2{5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<const double*> cs{&p0.coefficient(), &p1.coefficient(),
                                  &p2.coefficient()};
    std::vector<const double*> es{&p0.exponent(), &p1.exponent(),
                                  &p2.exponent()};
    ContractedGaussianView<const double> g(cs, es, &p0.x(), &p0.y(), &p0.z());
    std::array prims{std::move(p0), std::move(p1), std::move(p2)};
    compare_state(prims, g);
    SECTION("Is read-only") {
        STATIC_REQUIRE(
          std::is_same_v<PrimitiveView<const double>, decltype(g[0])>);
    }
}

TEST_CASE("ContractedGaussianView<double> : implicit conversion") {
    prim_t p0{1.0, 2.0, 7.0, 8.0, 9.0};
    prim_t p1{3.0, 4.0, 7.0, 8.0, 9.0};
    prim_t p2{5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<double*> cs{&p0.coefficient(), &p1.coefficient(),
                            &p2.coefficient()};
    std::vector<double*> es{&p0.exponent(), &p1.exponent(), &p2.exponent()};
    ContractedGaussianView<double> g(cs, es, &p0.x(), &p0.y(), &p0.z());
    std::array prims{std::move(p0), std::move(p1), std::move(p2)};
    const ContractedGaussian<double>& g2(g);
    compare_state(g2, prims);
}
