#include <catch2/catch.hpp>
#include <iostream>
#include <libchemist/ao_shell.hpp>
#include <sde/detail_/memoization.hpp>
#include <sstream>

using namespace libchemist;
using Cartesian   = AOShell::Cartesian;
using Spherical   = AOShell::Spherical;
using size_type   = typename AOShell::size_type;
using coord_type  = typename AOShell::coord_type;
using prim_vector = std::vector<AOPrimitive>;

static const std::array<std::string, 5> corr_hash{
  "281913aefaef7007f0dfdd8c1080fb67", // Default ctor
  "9cc4009ce4fb86b00ec551cd1c9bb657", // Two prims, pure S
  "7b474c64cbe2c05bcf00672d7d0f87ea", // Two prims, pure D
  "e7a41ab9c2bbddb4cabeb9d8544362ee", // Two prims, cart F
  "677572a854c0950b6e4bc78ede97e565", // Two prims, pure S, and center
};

template<size_t hash>
void check_shell(AOShell& shell, const coord_type& carts,
                 const prim_vector& prims, size_type l, bool pure,
                 size_type nAOs) {
    const AOShell& const_shell = shell;

    REQUIRE(shell.size() == nAOs);
    REQUIRE(const_shell.size() == nAOs);
    REQUIRE(shell.nprims() == prims.size());
    REQUIRE(const_shell.nprims() == prims.size());
    REQUIRE(shell.pure() == pure);
    REQUIRE(const_shell.pure() == pure);
    REQUIRE(shell.l() == l);
    REQUIRE(const_shell.l() == l);
    REQUIRE(shell.center() == carts);
    REQUIRE(const_shell.center() == carts);
    for(size_type i = 0; i < prims.size(); ++i) {
        REQUIRE(shell.coef(i) == prims[i].c);
        REQUIRE(const_shell.coef(i) == prims[i].c);
        REQUIRE(shell.alpha(i) == prims[i].alpha);
        REQUIRE(const_shell.alpha(i) == prims[i].alpha);
    }
    sde::Hasher h(sde::HashType::Hash128);
    h(shell);
    REQUIRE(bphash::hash_to_string(h.finalize()) == corr_hash[hash]);
}

TEST_CASE("AOShell Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<Cartesian, AOShell::Cartesian>);
        REQUIRE(std::is_same_v<Spherical, AOShell::Spherical>);
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<coord_type, std::array<double, 3>>);
    }

    prim_vector prims{{1.23, 2.34}, {3.45, 4.56}};
    coord_type carts{1.0, 2.0, 3.0};
    SECTION("Default CTor") {
        AOShell shell;
        check_shell<0>(shell, coord_type{}, prim_vector{}, 0, true, 1);
        AOShell shell2;
        REQUIRE(shell == shell2);
        REQUIRE(!(shell != shell2));
    }

    SECTION("State CTor") {
        SECTION("Two primitives (pure s-shell)") {
            AOShell shell(prims[0], prims[1]);
            check_shell<1>(shell, coord_type{}, prims, 0, true, 1);
        }

        SECTION("Two primitives d-shell") {
            AOShell shell(prims[0], prims[1], 2);
            check_shell<2>(shell, coord_type{}, prims, 2, true, 5);

            AOShell shell2(prims[0], 2, prims[1]);
            check_shell<2>(shell2, coord_type{}, prims, 2, true, 5);

            AOShell shell3(2, prims[0], prims[1]);
            check_shell<2>(shell3, coord_type{}, prims, 2, true, 5);
        }

        SECTION("Two primitives, Cartesian f-shell") {
            AOShell shell(Cartesian{}, prims[0], prims[1], 3);
            check_shell<3>(shell, coord_type{}, prims, 3, false, 10);

            AOShell shell2(3, Cartesian{}, prims[0], prims[1]);
            check_shell<3>(shell2, coord_type{}, prims, 3, false, 10);

            AOShell shell3(prims[0], Cartesian{}, prims[1], 3);
            check_shell<3>(shell3, coord_type{}, prims, 3, false, 10);
        }

        SECTION("Primitives, Spherical s-shell, and center") {
            AOShell shell(carts, prims[0], prims[1]);
            check_shell<4>(shell, carts, prims, 0, true, 1);
        }
    }

    AOShell shell(prims[0], carts, prims[1]);

    SECTION("Copy CTor") {
        AOShell shell2(shell);
        check_shell<4>(shell2, carts, prims, 0, true, 1);
        REQUIRE(shell2 == shell);
    }

    //    SECTION("Copy Assignment Operator") {
    //        AOShell shell2;
    //        AOShell& pshell = (shell2 = shell);
    //        check_shell(shell2, carts, prims, 0, true, 1);
    //        REQUIRE(&shell2 == &pshell);
    //    }

    SECTION("Move CTor") {
        AOShell shell2(std::move(shell));
        check_shell<4>(shell2, carts, prims, 0, true, 1);
    }

    SECTION("Move Assignment Operator") {
        AOShell shell2;
        AOShell& pshell = (shell2 = std::move(shell));
        check_shell<4>(shell2, carts, prims, 0, true, 1);
        REQUIRE(&shell2 == &pshell);
    }

    SECTION("Printing") {
        std::string corr =
          "l: 0 Rx: 1.000000000000000 Ry: 2.000000000000000 Rz: "
          "3.000000000000000\nc: 2.340000000000000 alpha: 1.230000000000000\n"
          "c: 4.560000000000000 alpha: 3.450000000000000\n";
        std::stringstream ss;
        ss << shell;
        REQUIRE(ss.str() == corr);
    }
}