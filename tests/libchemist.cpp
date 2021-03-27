#include "libchemist/libchemist.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <limits>

using namespace libchemist;
using coord_type = std::array<double, 3>;

inline auto corr_bs() {
    AOBasisSet<double> rv;
    Center<double> O(0.0, -0.1432223429807816, 0.0);
    O.add_shell(ShellType::pure, 0ul,
                std::vector<double>{0.15432897, 0.53532814, 0.44463454},
                std::vector<double>{130.7093200, 23.8088610, 6.4436083});
    O.add_shell(ShellType::pure, 0ul,
                std::vector<double>{-0.09996723, 0.39951283, 0.70011547},
                std::vector<double>{5.0331513, 1.1695961, 0.3803890});
    O.add_shell(ShellType::pure, 1ul,
                std::vector<double>{0.15591627, 0.60768372, 0.39195739},
                std::vector<double>{5.0331513, 1.1695961, 0.3803890});
    Center<double> H1(1.6380335020342418, 1.1365568803584036, 0.0);
    H1.add_shell(ShellType::pure, 0ul,
                 std::vector<double>{0.15432897, 0.53532814, 0.44463454},
                 std::vector<double>{3.42525091, 0.62391373, 0.16885540});
    Center<double> H2(-1.6380335020342418, 1.1365568803584036, 0.0);
    H2.add_shell(ShellType::pure, 0ul,
                 std::vector<double>{0.15432897, 0.53532814, 0.44463454},
                 std::vector<double>{3.42525091, 0.62391373, 0.16885540});
    rv.add_center(O);
    rv.add_center(H1);
    rv.add_center(H2);
    return rv;
}

TEST_CASE("Convenience Functions") {
    SECTION("apply_basis") {
        std::cout.precision(std::numeric_limits<double>::max_digits10);
        Molecule water = MoleculeManager().at("water");
        REQUIRE(apply_basis("sto-3g", water).basis_set() == corr_bs());
    }
}
