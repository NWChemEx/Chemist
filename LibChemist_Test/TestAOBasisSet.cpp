#include <LibChemist/AOBasisSet.hpp>
#include <catch/catch.hpp>

using namespace LibChemist;

using prim_vector = std::vector<AOPrimitive>;
using coord_type = typename AOShell::coord_type;


TEST_CASE("AOBasisSet class") {
    prim_vector prims{{1.23, 2.34}, {3.45, 4.56}};
    coord_type carts {1.0, 2.0, 3.0};
    AOShell shell(carts, prims[0], prims[1]);

    AOBasisSet bs(shell);
}
