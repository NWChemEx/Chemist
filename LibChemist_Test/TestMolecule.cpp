#include <LibChemist/Molecule.hpp>
#include <SDE/Memoization.hpp>
#include <catch/catch.hpp>
#include <sstream>

using namespace LibChemist;

// Forwad typedefs from the molecule
using value_type      = typename Molecule::value_type;
using reference       = typename Molecule::reference;
using const_reference = typename Molecule::const_reference;
using size_type       = typename Molecule::size_type;
using iterator        = typename Molecule::iterator;
using const_iterator  = typename Molecule::const_iterator;

// Typedefs used for the tests
using cart_t      = std::array<double, 3>;
using vector_type = std::vector<value_type>;

static const std::array<std::string, 7> corr_hash{
  "86951e08965a42313aab98b7068dc987", // Default CTor
  "db61ac3b2cae688d81ae946ea6016a50", // Charge
  "81b850fa0174301d1960b5cecf70e6c4", // Mult
  "5beacbdfcc4e09dfbc5c24f2b20e1752", // 1 atom
  "42dbe60b8068e41c05e90c0bdd508c87", // 2 atom anion
  "acf68ef67589dad2d2ab0a0ad6cad8ff", // D-D molecule
  "c2013e4113832eacd5e7928bf2eb6cc4"  // H-D molecule
};

template<size_t hash>
void check_state(Molecule& mol, const vector_type& atoms, double charge,
                 size_type mult, size_type nelecs) {
    const Molecule& const_mol = mol;
    REQUIRE(mol.charge() == charge);
    REQUIRE(mol.multiplicity() == mult);
    REQUIRE(const_mol.charge() == charge);
    REQUIRE(const_mol.multiplicity() == mult);
    REQUIRE(mol.size() == atoms.size());
    REQUIRE(mol.nelectrons() == nelecs);
    REQUIRE(const_mol.nelectrons() == nelecs);

    bphash::Hasher h(bphash::HashType::Hash128);
    h(mol);
    REQUIRE(bphash::hash_to_string(h.finalize()) == corr_hash[hash]);

    if(!mol.size()) return;
    double* pcarts        = &mol.at(0).coords()[0];
    double* pmass         = &mol.at(0).mass();
    size_type* pZ         = &mol.at(0).Z();
    const double* pccarts = &const_mol.at(0).coords()[0];
    const double* pcmass  = &const_mol.at(0).mass();
    const size_type* pcZ  = &const_mol.at(0).Z();
    for(size_type i = 0, carti = 0; i < atoms.size(); ++i) {
        REQUIRE(mol.at(i) == atoms[i]);
        REQUIRE(const_mol.at(i) == atoms[i]);
        REQUIRE(const_mol[i] == atoms[i]);
        REQUIRE(mol[i] == atoms[i]);
        REQUIRE(pmass[i] == atoms[i].mass());
        REQUIRE(pcmass[i] == atoms[i].mass());
        REQUIRE(pZ[i] == atoms[i].Z());
        REQUIRE(pcZ[i] == atoms[i].Z());
        for(size_type j = 0; j < 3; ++j, ++carti) {
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
        REQUIRE(
          std::is_same_v<const_iterator, typename vector_type::const_iterator>);
    }

    SECTION("Default CTor") {
        Molecule mol;
        check_state<0>(mol, vector_type{}, 0.0, 1ul, 0ul);
    }

    vector_type atoms{Atom{"H", 1ul, 1.0079, cart_t{0.0, 0.0, 0.89}},
                      Atom{"D", 1ul, 2.0079, cart_t{0.0, 0.0, 0.0}}};

    using Charge       = Molecule::Charge;
    using Multiplicity = Molecule::Multiplicity;

    SECTION("State CTor") {
        SECTION("Charge") {
            Molecule mol(Charge{2.0});
            check_state<1>(mol, vector_type{}, 2.0, 1ul, 0ul - 2ul);
        }

        SECTION("Multiplicity") {
            Molecule mol(Multiplicity{3ul});
            check_state<2>(mol, vector_type{}, 0.0, 3ul, 0ul);
        }

        SECTION("An atom") {
            Molecule mol(atoms[0]);
            check_state<3>(mol, vector_type{atoms[0]}, 0.0, 1ul, 1ul);
        }

        SECTION("H-D molecule anion") {
            Molecule mol(atoms[0], Charge{-1.0}, atoms[1], Multiplicity{2ul});
            check_state<4>(mol, atoms, -1.0, 2ul, 3ul);
        }
    }

    Molecule mol(atoms[0], atoms[1]);

    SECTION("Assignment doesn't mess buffers up") {
        mol[0] = atoms[1];
        check_state<5>(mol, vector_type{atoms[1], atoms[1]}, 0.0, 1ul, 2ul);
    }

    SECTION("Copy CTor") {
        Molecule mol2(mol);
        check_state<6>(mol2, atoms, 0.0, 1ul, 2ul);
        REQUIRE(&mol.at(0).coords()[0] != &mol2.at(0).coords()[0]);
    }

    SECTION("Copy Assignment") {
        Molecule mol2;
        auto& pmol = (mol2 = mol);
        check_state<6>(mol2, atoms, 0.0, 1ul, 2ul);
        REQUIRE(&pmol == &mol2);
        REQUIRE(&mol.at(0).coords()[0] != &mol2.at(0).coords()[0]);
    }

    SECTION("Move CTor") {
        Molecule mol2(std::move(mol));
        check_state<6>(mol2, atoms, 0.0, 1ul, 2ul);
    }

    SECTION("Move Assignment") {
        Molecule mol2;
        auto& pmol = (mol2 = std::move(mol));
        check_state<6>(mol2, atoms, 0.0, 1ul, 2ul);
        REQUIRE(&pmol == &mol2);
    }

    SECTION("Printing") {
        std::stringstream ss, corr_ss;
        corr_ss << atoms[0] << std::endl << atoms[1] << std::endl;
        ss << mol;
        REQUIRE(corr_ss.str() == ss.str());
    }
}
