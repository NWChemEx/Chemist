#include <catch2/catch.hpp>
#include <libchemist/basis_set/ao_basis_function.hpp>

using namespace libchemist::basis_set;

TEST_CASE("AOBasisFunction<double>") {
    using g_type = Gaussian<double>;
    std::vector<g_type> gs{g_type{1.23, 2.34}, g_type{3.45, 4.56}};
    AOBasisFunction<double> bf{gs.begin(), gs.end()};
    SECTION("Primary Ctor") {
        REQUIRE(bf.size() == 2);
        REQUIRE(bf[0] == gs[0]);
        REQUIRE(bf[1] == gs[1]);
    }
}
