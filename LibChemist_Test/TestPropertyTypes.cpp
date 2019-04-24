#include "TestPropertyTypes.hpp"
#include <tamm/tamm_utils.hpp>
#include <catch2/catch.hpp>

using namespace LibChemist;

TEST_CASE("Property Types") {
// bunch of blank input
Molecule mol;
OrbitalSpace orbs;
AOBasisSet bra, ket;
std::size_t deriv = 0;

    SECTION("AOIntegral") {
        DummyInts dumb;
        auto in = dumb.inputs();
        auto sub = dumb.submods();
        in.at("Molecule").change(mol);
        in.at("Basis Sets").change(std::array<AOBasisSet,2>{bra,ket});
        in.at("Derivative").change(deriv);
        auto out = dumb.run(in,sub);
        auto [tensor] = AOIntegral<2,double>::unwrap_results(out);
        auto scal = tamm::get_scalar(tensor);
        REQUIRE(scal == 7.0);

    }
    SECTION("CoreHamiltonian") {
        DummyCore dumb;
        auto in = dumb.inputs();
        auto sub = dumb.submods();
        in.at("Molecule").change(mol);
        in.at("Bra").change(bra);
        in.at("Ket").change(ket);
        in.at("Derivative").change(deriv);
        auto out = dumb.run(in,sub);
        auto [tensor] = CoreHamiltonian<double>::unwrap_results(out);
        auto scal = tamm::get_scalar(tensor);
        REQUIRE(scal == 7.0);
    }
    SECTION("JKMatrices") {
        DummyJK dumb;
        auto in = dumb.inputs();
        auto sub = dumb.submods();
        in.at("Molecule").change(mol);
        in.at("Molecular Orbitals").change(orbs);
        in.at("Bra").change(bra);
        in.at("Ket").change(ket);
        in.at("Derivative").change(deriv);
        auto out = dumb.run(in,sub);
        auto [J, K] = JKMatrices<double>::unwrap_results(out);
        auto scalJ = tamm::get_scalar(J);
        auto scalK = tamm::get_scalar(K);
        REQUIRE(scalJ == 7.0);
        REQUIRE(scalK == 8.0);
    }
    SECTION("OperatorAO") {
        DummyFock dumb;
        auto in = dumb.inputs();
        auto sub = dumb.submods();
        in.at("Molecule").change(mol);
        in.at("Molecular Orbitals").change(orbs);
        in.at("Bra").change(bra);
        in.at("Ket").change(ket);
        in.at("Derivative").change(deriv);
        auto out = dumb.run(in,sub);
        auto [tensor] = OperatorAO<double>::unwrap_results(out);
        auto scal = tamm::get_scalar(tensor);
        REQUIRE(scal == 7.0);
    }
    SECTION("Energy") {
        DummE dumb;
        auto in = dumb.inputs();
        auto sub = dumb.submods();
        in.at("Molecule").change(mol);
        in.at("Derivative").change(deriv);
        auto out = dumb.run(in,sub);
        auto [tensor] = Energy<double>::unwrap_results(out);
        auto scal = tamm::get_scalar(tensor);
        REQUIRE(scal == 7.0);
    }
}
