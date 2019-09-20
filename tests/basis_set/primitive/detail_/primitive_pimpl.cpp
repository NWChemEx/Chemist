#include "libchemist/basis_set/primitive/detail_/primitive_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::detail_;

template<typename T, typename U>
void check_state(T&& prim, U&& corr) {
    SECTION("Coefficient") { REQUIRE(prim.coef() == corr[0]); }
    SECTION("Exponent") { REQUIRE(prim.exp() == corr[1]); }
}

TEST_CASE("PrimitivePIMPL<double> : default ctor") {
    PrimitivePIMPL<double> pimpl;
    check_state(pimpl, std::vector<double>(5, 0.0));
}
