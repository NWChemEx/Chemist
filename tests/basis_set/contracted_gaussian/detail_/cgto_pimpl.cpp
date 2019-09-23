include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"
#include <catch2/catch.hpp>

  using namespace libchemist::detail_;

template<typename T, typename U>
void check_state(T&& prim, U&& corr) {
    SECTION("Coefficient") { REQUIRE(prim.coef() == corr[0]); }
    SECTION("Exponent") { REQUIRE(prim.exp() == corr[1]); }
}
