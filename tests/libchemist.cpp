#include <catch2/catch.hpp>
#include <iostream>
#include <libchemist/libchemist.hpp>
#include <limits>

using namespace libchemist;
using coord_type = std::array<double, 3>;

static AOBasisSet corr_bs(
  AOShell(0ul, AOPrimitive{130.7093200, 0.15432897},
          AOPrimitive{23.8088610, 0.53532814},
          AOPrimitive{6.4436083, 0.44463454},
          coord_type{0.0, -0.1432223429807816, 0.0}),
  AOShell(0ul, AOPrimitive{5.0331513, -0.09996723},
          AOPrimitive{1.1695961, 0.39951283},
          AOPrimitive{0.3803890, 0.70011547},
          coord_type{0.0, -0.1432223429807816, 0.0}),
  AOShell(1ul, AOPrimitive{5.0331513, 0.15591627},
          AOPrimitive{1.1695961, 0.60768372},
          AOPrimitive{0.3803890, 0.39195739},
          coord_type{0.0, -0.1432223429807816, 0.0}),
  AOShell(0ul, AOPrimitive{3.42525091, 0.15432897},
          AOPrimitive{0.62391373, 0.53532814},
          AOPrimitive{0.16885540, 0.44463454},
          coord_type{1.6380335020342418, 1.1365568803584036, 0.0}),
  AOShell(0ul, AOPrimitive{3.42525091, 0.15432897},
          AOPrimitive{0.62391373, 0.53532814},
          AOPrimitive{0.16885540, 0.44463454},
          coord_type{-1.6380335020342418, 1.1365568803584036, 0.0}));

TEST_CASE("Convenience Functions") {
    SECTION("apply_basis") {
        std::cout.precision(std::numeric_limits<double>::max_digits10);
        Molecule water = MoleculeManager().at("water");
        REQUIRE(apply_basis("sto-3g", water) == corr_bs);
    }
}