#include "LibChemist/BasisShell.hpp"

#include "catch/catch.hpp"

using namespace LibChemist;

TEST_CASE("BasisShell class") {
    // Make sure we can construct a default BasisShell
    BasisShell Defaulted;

    // Some fake exponents
    std::vector<double> as({3.1, 4.5, 6.9});
    // Some fake contraction coefficients
    std::vector<double> cs({8.1, 2.6, 7.1});
    // Some fake contraction coefficients assuming a general contraction
    std::vector<double> gen_cs({1.4, 6.8, 7.1, 9.1, 5.4, 6.0});

    BasisShell CartBS(ShellType::CartesianGaussian, 2, 1, as, cs);
    BasisShell PureBS(ShellType::SphericalGaussian, -1, 2, std::move(as),
                      std::move(gen_cs));
    REQUIRE(CartBS.type == ShellType::CartesianGaussian);
    REQUIRE(PureBS.type == ShellType::SphericalGaussian);
    REQUIRE(CartBS.l == 2);
    REQUIRE(PureBS.ngen == 2);
    REQUIRE(CartBS.nprim == 3);
    REQUIRE(CartBS.nfunctions(0) == 6);
    REQUIRE(PureBS.nfunctions(0) == 1);
    REQUIRE(PureBS.nfunctions(1) == 3);

    BasisShell Copy(CartBS);
    REQUIRE(Copy == CartBS);
    REQUIRE(Copy != PureBS);
    BasisShell Moved(std::move(Copy));
    REQUIRE(Moved == CartBS);
    Defaulted = std::move(Moved);
    REQUIRE(Defaulted == CartBS);
    Moved = Defaulted;
    REQUIRE(Moved == Defaulted);
    REQUIRE(Moved == CartBS);
    REQUIRE(CartBS.alpha(1) == 4.5);
    REQUIRE(CartBS.coef(1, 0) == 2.6);
    REQUIRE(PureBS.coef(1, 1) == 5.4);
}
