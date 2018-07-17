#include <LibChemist/BasisSet.hpp>
#include <catch/catch.hpp>

using namespace LibChemist;

TEST_CASE("BasisSet class") {
    // Make sure we can construct a default BasisSet
    BasisSet bs;

    // Some fake exponents
    std::vector<double> as({3.1, 4.5, 6.9});
    // Some fake contraction coefficients
    std::vector<double> cs({8.1, 2.6, 7.1});
    // Some fake contraction coefficients assuming a general contraction
    std::vector<double> gen_cs({1.4, 6.8, 7.1, 9.1, 5.4, 6.0});
    std::vector<double> origin(3, 0.0);
    BasisShell Cart = BasisShell(ShellType::CartesianGaussian, 2, 1, as, cs);
    BasisShell Pure =
      BasisShell(ShellType::SphericalGaussian, -1, 2, as, gen_cs);

    bs.add_shell(origin.data(), Cart);
    bs.add_shell(origin.data(), Pure);

    BasisSet corr;
    REQUIRE(corr != bs);

    corr.centers = std::vector<double>(6, 0.0);
    corr.coefs =
      std::vector<double>({8.1, 2.6, 7.1, 1.4, 6.8, 7.1, 9.1, 5.4, 6.0});
    corr.alphas = std::vector<double>({3.1, 4.5, 6.9, 3.1, 4.5, 6.9});
    corr.nprims = std::vector<size_t>({3, 3});
    corr.ngens  = std::vector<size_t>({1, 2});
    corr.types  = std::vector<ShellType>(
      {ShellType::CartesianGaussian, ShellType::SphericalGaussian});
    corr.ls = std::vector<int>({2, -1});
    REQUIRE(corr == bs);
    REQUIRE(bs.max_am() == 2);
    REQUIRE(bs.size() == 10);

    REQUIRE(bs.shellsize(0) == 6);
    REQUIRE(bs.shellsize(1) == 4);

    BasisSet Copy(bs);
    REQUIRE(Copy == bs);
    REQUIRE(Copy == corr);

    BasisSet Move(std::move(Copy));
    REQUIRE(Move == bs);
    REQUIRE(Move == corr);

    Copy = std::move(Move);
    REQUIRE(Copy == bs);
    REQUIRE(Copy == corr);

    Move = Copy;
    REQUIRE(Copy == Move);
    REQUIRE(Move == bs);
    REQUIRE(Move == corr);

    // Ungeneralize test
    BasisSet corr_ungen;
    corr_ungen.centers = std::vector<double>(9, 0.0);
    corr_ungen.coefs =
      std::vector<double>({8.1, 2.6, 7.1, 1.4, 6.8, 7.1, 9.1, 5.4, 6.0});
    corr_ungen.alphas =
      std::vector<double>({3.1, 4.5, 6.9, 3.1, 4.5, 6.9, 3.1, 4.5, 6.9});
    corr_ungen.nprims = std::vector<size_t>({3, 3, 3});
    corr_ungen.ngens  = std::vector<size_t>({1, 1, 1});
    corr_ungen.types  = std::vector<ShellType>({ShellType::CartesianGaussian,
                                               ShellType::SphericalGaussian,
                                               ShellType::SphericalGaussian});
    corr_ungen.ls     = std::vector<int>({2, 0, 1});
    REQUIRE(corr_ungen == ungeneralize_basis_set(bs));

    // Concatenation test
    BasisSet corr_concat;
    corr_concat.centers = std::vector<double>(12, 0.0);
    corr_concat.coefs =
      std::vector<double>({8.1, 2.6, 7.1, 1.4, 6.8, 7.1, 9.1, 5.4, 6.0, 8.1,
                           2.6, 7.1, 1.4, 6.8, 7.1, 9.1, 5.4, 6.0});
    corr_concat.alphas = std::vector<double>(
      {3.1, 4.5, 6.9, 3.1, 4.5, 6.9, 3.1, 4.5, 6.9, 3.1, 4.5, 6.9});
    corr_concat.nprims = std::vector<size_t>(4, 3);
    corr_concat.ngens  = std::vector<size_t>({1, 2, 1, 2});
    corr_concat.types  = std::vector<ShellType>(
      {ShellType::CartesianGaussian, ShellType::SphericalGaussian,
       ShellType::CartesianGaussian, ShellType::SphericalGaussian});
    corr_concat.ls = std::vector<int>({2, -1, 2, -1});
    REQUIRE(corr_concat == basis_set_concatenate(bs, Copy));
}
