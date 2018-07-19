#include <LibChemist/AOShell.hpp>
#include <catch/catch.hpp>

using namespace LibChemist;
using Cartesian = AOShell::Cartesian;
using Spherical = AOShell::Spherical;
using size_type = typename AOShell::size_type;
using coord_type = typename AOShell::coord_type;
using prim_vector = std::vector<AOPrimitive>;

void check_shell(AOShell& shell, const prim_vector& prims, size_type l,
                 bool pure, size_type nAOs) {
    const AOShell& const_shell = shell;

    REQUIRE(shell.size() == nAOs);
    REQUIRE(const_shell.size() == nAOs);
    REQUIRE(shell.nprims() == prims.size());
    REQUIRE(const_shell.nprims() == prims.size());
    REQUIRE(shell.pure() == pure);
    REQUIRE(const_shell.pure() == pure);
    REQUIRE(shell.l() == l);
    REQUIRE(const_shell.l() == l);
    for(size_type i=0; i < prims.size(); ++i) {
        REQUIRE(shell.coef(i) == prims[i].c);
        REQUIRE(const_shell.coef(i) == prims[i].c);
        REQUIRE(shell.alpha(i) == prims[i].alpha);
        REQUIRE(const_shell.alpha(i) == prims[i].alpha);
    }
}

TEST_CASE("AOShell Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<Cartesian, AOShell::Cartesian>);
        REQUIRE(std::is_same_v<Spherical, AOShell::Spherical>);
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<coord_type, std::array<double, 3>>);
    }

    prim_vector prims{{1.23, 2.34}, {3.45, 4.56}};

    SECTION("Default CTor") {
        AOShell shell;
        check_shell(shell, prim_vector{}, 0, true, 1);
    }

    SECTION("State CTor") {
        SECTION("Two primitives (pure s-shell)") {
            AOShell shell(prims[0], prims[1]);
            check_shell(shell, prims, 0, true, 1);
        }

        SECTION("Two primitives d-shell") {
            AOShell shell(prims[0], prims[1], 2);
            check_shell(shell, prims, 2, true, 5);

            AOShell shell2(prims[0], 2, prims[1]);
            check_shell(shell2, prims, 2, true, 5);

            AOShell shell3(2, prims[0], prims[1]);
            check_shell(shell3, prims, 2, true, 5);
        }

        SECTION("Two primitives, Cartesian f-shell") {
            AOShell shell(Cartesian{}, prims[0], prims[1], 3);
            check_shell(shell, prims, 3, false, 10);
        }
    }

}
