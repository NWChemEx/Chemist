#include <LibChemist/Molecule.hpp>
#include <catch/catch.hpp>

using namespace LibChemist;

using Property       = typename Molecule::property_key;
using property_map   = typename Molecule::property_map;
using atom_container = typename Molecule::atom_container;

void check_state(const Molecule& mol, const property_map& props,
                 const atom_container& atoms) {
    REQUIRE(mol.properties == props);
    REQUIRE(mol.atoms == atoms);
}

static std::vector<BasisShell> a_shell{BasisShell(
  ShellType::CartesianGaussian, -1, 2, std::vector<double>({3.0, 4.0}),
  std::vector<double>({5.0, 6.0, 7.0, 8.0}))};

BasisSet corr_basis(bool general = false) {
    std::vector<double> h1(3), h2(3);
    h2[2] = 0.89;
    BasisSet H1, H2;
    H1.add_shell(h1.data(), a_shell[0]);
    H2.add_shell(h2.data(), a_shell[0]);
    if(!general) {
        H1 = ungeneralize_basis_set(H1);
        H2 = ungeneralize_basis_set(H2);
    }
    return basis_set_concatenate(H1, H2);
}

TEST_CASE("Molecule Class") {
    SECTION("Molecule typedefs") {
        // We only test non-forwarded typedefs
        REQUIRE(std::is_same<typename Molecule::property_value, double>::value);
    }

    SECTION("Default Molecule") {
        Molecule mol;
        check_state(mol, {}, {});
    }

    SECTION("Molecule aggregate initialized") {
        property_map props{{Property::nalpha, 1.0},
                           {Property::nbeta, 1.0},
                           {Property::multiplicity, 1.0}};
        Atom H1{{0.0, 0.0, 0.0},
                {{Atom::Property::charge, 1.0}, {Atom::Property::mass, 1.0079}},
                {{"PRIMARY", {a_shell}}}};
        Atom H2{H1};
        H2.coords[2] = 0.89;
        atom_container atoms{H1, H2};
        Molecule molecular_h{props, atoms};
        check_state(molecular_h, props, atoms);
        REQUIRE(molecular_h.nelectrons() == 2.0);
        REQUIRE(molecular_h.charge() == 0.0);
        REQUIRE(molecular_h.get_basis("PRIMARY") == corr_basis());
        REQUIRE(molecular_h.get_basis("PRIMARY", false) == corr_basis(true));

        SECTION("Charged System") {
            // Two ways to get charged system 1. Play w/ Z
            H2.properties[Atom::Property::charge] = 0.0;
            Molecule temp{props, atom_container{H1, H2}};
            REQUIRE(temp.charge() == -1.0);
            // 2. Play w/ n electrons
            molecular_h.properties[Property::nalpha] = 0.0;
            REQUIRE(molecular_h.charge() == 1.0);
        }
    }
}
