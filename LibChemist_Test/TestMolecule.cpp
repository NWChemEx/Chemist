#include <LibChemist/Molecule.hpp>
#include <catch/catch.hpp>
#include <sstream>

using namespace LibChemist;

//Forwad typedefs from the molecule
using value_type = typename Molecule::value_type;
using reference = typename Molecule::reference;
using const_reference = typename Molecule::const_reference;
using size_type = typename Molecule::size_type;
using iterator = typename Molecule::iterator;
using const_iterator = typename Molecule::const_iterator;

//Typedefs used for the tests
using cart_t = std::array<double, 3>;
using vector_type = std::vector<value_type>;


void check_state(Molecule& mol, const vector_type& atoms, double charge=0.0,
                 size_type mult = 1ul) {
    const Molecule& const_mol = mol;
    REQUIRE(mol.charge() == charge);
    REQUIRE(mol.multiplicity() == mult);
    REQUIRE(const_mol.charge() == charge);
    REQUIRE(const_mol.multiplicity() == mult);
    REQUIRE(mol.size() == atoms.size());
    if(!mol.size()) return;
    double* pcarts = &mol.at(0).coords()[0];
    double* pmass = &mol.at(0).mass();
    size_type* pZ = &mol.at(0).Z();
    const double* pccarts = &const_mol.at(0).coords()[0];
    const double* pcmass = &const_mol.at(0).mass();
    const size_type* pcZ = &const_mol.at(0).Z();
    for(size_type i=0, carti=0; i < atoms.size(); ++i) {
        REQUIRE(mol.at(i) == atoms[i]);
        REQUIRE(const_mol.at(i) == atoms[i]);
        REQUIRE(const_mol[i] == atoms[i]);
        REQUIRE(mol[i] == atoms[i]);
        REQUIRE(pmass[i] == atoms[i].mass());
        REQUIRE(pcmass[i] == atoms[i].mass());
        REQUIRE(pZ[i] == atoms[i].Z());
        REQUIRE(pcZ[i] == atoms[i].Z());
        for(size_type j=0; j < 3; ++j, ++carti) {
            REQUIRE(pcarts[carti] == atoms[i].coords()[j]);
            REQUIRE(pcarts[carti] == atoms[i].coords()[j]);
        }
    }
}

TEST_CASE("Molecule Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<value_type, Atom>);
        REQUIRE(std::is_same_v<reference, Atom&>);
        REQUIRE(std::is_same_v<const_reference, const Atom&>);
        REQUIRE(std::is_same_v<iterator, typename vector_type::iterator>);
        REQUIRE(std::is_same_v<const_iterator,
                               typename vector_type::const_iterator>);
    }

    SECTION("Default CTor") {
        Molecule mol;
        check_state(mol, vector_type{});
    }

    vector_type atoms {Atom{"H", 1.0079, cart_t{0.0, 0.0, 0.89}},
                       Atom{"D", 2.0079, cart_t{0.0, 0.0, 0.0}}};

    using Charge = Molecule::Charge;
    using Multiplicity = Molecule::Multiplicity;

    SECTION("State CTor") {
        SECTION("Charge") {
            Molecule mol(Charge{2.0});
            check_state(mol, vector_type{}, 2.0);
        }

        SECTION("Multiplicity") {
            Molecule mol(Multiplicity{3ul});
            check_state(mol, vector_type{}, 0.0, 3ul);
        }

        SECTION("An atom") {
            Molecule mol(atoms[0]);
            check_state(mol, vector_type{atoms[0]});
        }

        SECTION("H-D molecule anion") {
            Molecule mol(atoms[0], Charge{-1.0}, atoms[1], Multiplicity{2ul});
            check_state(mol, atoms, -1.0, 2ul);
        }
    }

    Molecule mol(atoms[0], atoms[1]);

    SECTION("Assignment doesn't mess buffers up") {
        mol[0] = atoms[1];
        check_state(mol, vector_type{atoms[1], atoms[1]});
    }

    SECTION("Copy CTor") {
        Molecule mol2(mol);
        check_state(mol2, atoms);
        REQUIRE(&mol.at(0).coords()[0] != &mol2.at(0).coords()[0]);
    }

    SECTION("Copy Assignment") {
        Molecule mol2;
        auto& pmol = (mol2 = mol);
        check_state(mol2, atoms);
        REQUIRE(&pmol == &mol2);
        REQUIRE(&mol.at(0).coords()[0] != &mol2.at(0).coords()[0]);
    }

    SECTION("Move CTor") {
        Molecule mol2(std::move(mol));
        check_state(mol2, atoms);
    }

    SECTION("Move Assignment") {
        Molecule mol2;
        auto& pmol = (mol2 = std::move(mol));
        check_state(mol2, atoms);
        REQUIRE(&pmol == &mol2);
    }

    SECTION("Printing") {
        std::stringstream ss, corr_ss;
        corr_ss << atoms[0] << std::endl << atoms[1] <<std::endl;
        ss << mol;
        REQUIRE(corr_ss.str() == ss.str());
    }
}
