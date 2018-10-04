#include <LibChemist/MoleculeFileParser.hpp>
#include <catch/catch.hpp>
#include <sstream>

using namespace LibChemist;


void check_equality(Molecule& lhs, Molecule& rhs)
{
    REQUIRE(lhs.charge() == rhs.charge());
    REQUIRE(lhs.multiplicity() == rhs.multiplicity());
    REQUIRE(lhs.size() == rhs.size());
    for(auto i = 0; i < lhs.size(); ++i)
    {
        REQUIRE(lhs.at(i).Z() == rhs.at(i).Z());
        REQUIRE(lhs.at(i).mass() == rhs.at(i).mass());
        REQUIRE(lhs.at(i).coords() == rhs.at(i).coords());
        REQUIRE(lhs.at(i).name() == rhs.at(i).name());

    }



}

using c_t = typename Atom::coord_type;
using m_t = typename Atom::mass_type;

TEST_CASE("Normal XYZ") {
    std::string h2o_example =
      "3\n"
      "\n"
      "H    0.000000000000000   1.579252144093028   2.174611055780858\n"
      "O    0.000000000000000   0.000000000000000   0.000000000000000\n"
      "H    0.000000000000000   1.579252144093028  -2.174611055780858\n"
      " \n";
//    using c_t = typename Atom::coord_type;
//    using m_t = typename Atom::mass_type;
    Atom H{"H", 1ul, c_t{0.000000000000000, 1.579252144093028, 2.174611055780858}, m_t{1837.4260218693814}};
    Atom O{"O", 8ul, c_t{0.000000000000000, 0.000000000000000, 0.000000000000000}, m_t{29165.122045980286}};
    Atom H2{"H", 1ul, c_t{0.000000000000000, 1.579252144093028, -2.174611055780858}, m_t{1837.4260218693814}};
    Molecule ref_h2o(H, O, H2);
    ref_h2o.nalpha()       = 5.0;
    ref_h2o.nbeta()        = 5.0;
    ref_h2o.multiplicity() = 1.0;
    std::stringstream ss3(h2o_example);
    Molecule h2o = parse_molecule_file(ss3, XYZParser(), PeriodicTable());
    check_equality(ref_h2o, h2o);
}

TEST_CASE("Harder XYZ parsing") {
    std::string xyz_example = " 2 3\nHe 0.1 .1 0.0\nHE 1.1 1e-1 0.0\n";

    std::array<double, 3> carts1(), carts2();
    PeriodicTable pt;
    Atom He = pt.get_atom(2);
    Atom He2{He};
    He.coords()  = {0.1, 0.1, 0.0};
    He2.coords() = {1.1, 0.1, 0.0};
    Molecule ref_mol(He, He2);
    ref_mol.charge() = 2.0;
    ref_mol.multiplicity() = 3.0;

    std::stringstream ss(xyz_example);
    Molecule mol = parse_molecule_file(ss, XYZParser(), pt);
    check_equality(ref_mol, mol);
}

TEST_CASE("Hardest XYZ parsing") {
    std::string xyz_example = " 2 3\nHe 0.1 .1 0.0\nSome Comment\n\nHE 1.1 1e-1 0.0\nH 1 1 blank\n";

    std::array<double, 3> carts1(), carts2();
    PeriodicTable pt;
    Atom He = pt.get_atom(2);
    Atom He2{He};
    He.coords()  = {0.1, 0.1, 0.0};
    He2.coords() = {1.1, 0.1, 0.0};
    Molecule ref_mol(He, He2);
    ref_mol.charge() = 2.0;
    ref_mol.multiplicity() = 3.0;

    std::stringstream ss(xyz_example);
    Molecule mol = parse_molecule_file(ss, XYZParser(), pt);
    check_equality(ref_mol, mol);
}

