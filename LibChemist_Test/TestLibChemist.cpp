#include <LibChemist/LibChemist.hpp>
#include <catch/catch.hpp>

using namespace LibChemist;
using coord_type = std::array<double, 3>;

static AOBasisSet corr_bs(
  AOShell(0ul, AOPrimitive{3.42525091, 0.15432897},
          AOPrimitive{0.62391373, 0.53532814},
          AOPrimitive{0.16885540, 0.44463454},
          coord_type{0.0, 2.984338304722454, 4.109397663896704}),
  AOShell(0ul, AOPrimitive{130.7093200, 0.15432897},
          AOPrimitive{23.8088610, 0.53532814},
          AOPrimitive{6.4436083, 0.44463454}, coord_type{0.0, 0.0, 0.0}),
  AOShell(0ul, AOPrimitive{5.0331513, -0.09996723},
          AOPrimitive{1.1695961, 0.39951283},
          AOPrimitive{0.3803890, 0.70011547}, coord_type{0.0, 0.0, 0.0}),
  AOShell(1ul, AOPrimitive{5.0331513, 0.15591627},
          AOPrimitive{1.1695961, 0.60768372},
          AOPrimitive{0.3803890, 0.39195739}, coord_type{0.0, 0.0, 0.0}),
  AOShell(0ul, AOPrimitive{3.42525091, 0.15432897},
          AOPrimitive{0.62391373, 0.53532814},
          AOPrimitive{0.16885540, 0.44463454},
          coord_type{0.0, 2.984338304722454, -4.109397663896704}));

TEST_CASE("Convenience Functions") {
    SECTION("apply_basis") {
        auto water = MoleculeManager().at("water");
        REQUIRE(apply_basis("sto-3g", water) == corr_bs);
    }
}
