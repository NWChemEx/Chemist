/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chemist/molecule/molecule.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

using namespace chemist;

// Forward typedefs from the molecule
using atom_type       = typename Molecule::atom_type;
using value_type      = typename Molecule::value_type;
using reference       = typename Molecule::reference;
using const_reference = typename Molecule::const_reference;
using size_type       = typename Molecule::size_type;
using iterator        = typename Molecule::iterator;
using const_iterator  = typename Molecule::const_iterator;

// Typedefs used for the tests
using cart_t      = std::array<double, 3>;
using vector_type = std::vector<atom_type>;

TEST_CASE("Molecule Class") {
    Atom h("H", 1ul, 1.0079, 0.0, 0.0, 0.89, 0.0);
    Atom d("D", 1ul, 2.0079, 0.0, 0.0, 0.0, 0.0);

    vector_type atoms{h, d};

    Molecule defaulted;
    Molecule mol{h};
    Molecule hd{h, d};
    Molecule qm(1, 2, {h, d});

    Molecule::nuclei_type nuclei{h.nucleus(), d.nucleus()};
    Molecule qm2(1, 2, nuclei);

    SECTION("CTors") {
        SECTION("Default CTor") {
            REQUIRE(defaulted.size() == 0ul);
            REQUIRE(defaulted.charge() == 0);
            REQUIRE(defaulted.multiplicity() == 1ul);
        }

        SECTION("initializer list") {
            REQUIRE(mol.size() == 1);
            REQUIRE(mol.charge() == 0);
            REQUIRE(mol.multiplicity() == 2);
            REQUIRE(mol.n_electrons() == 1);
            REQUIRE(mol[0] == atoms[0].nucleus());

            REQUIRE(hd.size() == 2);
            REQUIRE(hd.charge() == 0);
            REQUIRE(hd.multiplicity() == 1);
            REQUIRE(hd.n_electrons() == 2);
            REQUIRE(hd[0] == atoms[0].nucleus());
            REQUIRE(hd[1] == atoms[1].nucleus());
        }

        SECTION("charge, multiplicity, and atoms") {
            REQUIRE(qm.size() == 2);
            REQUIRE(qm.charge() == 1);
            REQUIRE(qm.n_electrons() == 1);
            REQUIRE(qm.multiplicity() == 2);
            REQUIRE(qm[0] == atoms[0].nucleus());
            REQUIRE(qm[1] == atoms[1].nucleus());
        }

        SECTION("charge, multiplicity, and nuclei") {
            REQUIRE(qm2.size() == 2);
            REQUIRE(qm2.charge() == 1);
            REQUIRE(qm2.n_electrons() == 1);
            REQUIRE(qm2.multiplicity() == 2);
            REQUIRE(qm2[0] == atoms[0].nucleus());
            REQUIRE(qm2[1] == atoms[1].nucleus());
        }

        SECTION("Copy CTor") {
            Molecule hd2(hd);
            REQUIRE(hd2 == hd);

            // Spot check it's a deep copy
            REQUIRE(&hd[0].x() != &hd2[0].x());
        }

        SECTION("Copy Assignment") {
            Molecule hd2;
            auto& phd = (hd2 = hd);
            REQUIRE(&phd == &hd2);
            REQUIRE(hd2 == hd);

            // Spot check it's a deep copy
            REQUIRE(&hd[0].x() != &hd2[0].x());
        }

        SECTION("Move CTor") {
            Molecule copy(hd);
            Molecule hd2(std::move(hd));
            REQUIRE(hd2 == copy);
        }

        SECTION("Move Assignment") {
            Molecule hd2;
            Molecule copy(hd);
            auto& phd = (hd2 = std::move(hd));
            REQUIRE(&phd == &hd2);
            REQUIRE(hd2 == copy);
        }
    }
    SECTION("push_back") {
        Atom h2(atoms[0]);
        h2.x() = -999.0;
        hd.push_back(h2);
        REQUIRE(hd.size() == 3);
        REQUIRE(hd[0] == atoms[0].nucleus());
        REQUIRE(hd[1] == atoms[1].nucleus());
        REQUIRE(hd[2] == h2.nucleus());
        REQUIRE(hd.charge() == 0);
        REQUIRE(hd.multiplicity() == 2);
        REQUIRE(hd.n_electrons() == 3);

        mol.push_back(h2);
        REQUIRE(mol.size() == 2);
        REQUIRE(mol[0] == atoms[0].nucleus());
        REQUIRE(mol[1] == h2.nucleus());
        REQUIRE(mol.charge() == 0);
        REQUIRE(mol.multiplicity() == 1);
        REQUIRE(mol.n_electrons() == 2);
    }

    Nuclei corr_nuclei{atoms[0].nucleus(), atoms[1].nucleus()};

    SECTION("nuclei") { REQUIRE(hd.nuclei() == corr_nuclei); }

    SECTION("nuclei() const") {
        REQUIRE(std::as_const(hd).nuclei() == corr_nuclei);
    }

    SECTION("n_electrons") {
        REQUIRE(mol.n_electrons() == 1);
        REQUIRE(hd.n_electrons() == 2);
        REQUIRE(qm.n_electrons() == 1);
    }

    SECTION("charge") {
        REQUIRE(mol.charge() == 0);
        REQUIRE(hd.charge() == 0);
        REQUIRE(qm.charge() == 1);
    }

    SECTION("set_charge") {
        mol.set_charge(1);
        REQUIRE(mol.charge() == 1);
        REQUIRE(mol.n_electrons() == 0);
        REQUIRE(mol.multiplicity() == 1);

        hd.set_charge(1);
        REQUIRE(hd.charge() == 1);
        REQUIRE(hd.n_electrons() == 1);
        REQUIRE(hd.multiplicity() == 2);

        hd.set_charge(-1);
        REQUIRE(hd.charge() == -1);
        REQUIRE(hd.n_electrons() == 3);
        REQUIRE(hd.multiplicity() == 2);
    }

    SECTION("multiplicity") {
        REQUIRE(mol.multiplicity() == 2);
        REQUIRE(hd.multiplicity() == 1);
        REQUIRE(qm.multiplicity() == 2);
    }

    SECTION("set_multiplicity") {
        hd.set_multiplicity(3);
        REQUIRE(hd.multiplicity() == 3);
    }

    SECTION("Printing") {
        std::stringstream ss, corr_ss;
        corr_ss << atoms[0] << std::endl << atoms[1] << std::endl;
        ss << hd;
        REQUIRE(corr_ss.str() == ss.str());
    }

    SECTION("Equality") {
        Atom H{"H",
               1ul,
               1837.4260218693814,
               0.000000000000000,
               1.579252144093028,
               2.174611055780858};
        Atom O{"O",
               8ul,
               29165.122045980286,
               0.000000000000000,
               0.000000000000000,
               0.000000000000000};
        Atom H2{"H",
                1ul,
                1837.4260218693814,
                0.000000000000000,
                1.579252144093028,
                -2.174611055780858};
        Molecule h2o_a{H, O, H2};
        Molecule h2o_b{H, O, H2};
        SECTION("Molecules are the same") { REQUIRE(h2o_a == h2o_b); }
        SECTION("Molecules are different") { REQUIRE(h2o_a != Molecule{}); }

        // Different charges
        h2o_a.set_charge(2);
        REQUIRE(h2o_a != h2o_b);

        h2o_b.set_charge(2);
        REQUIRE(h2o_a == h2o_b); // Sanity check they're back to equal

        // Different multiplicities
        h2o_a.set_multiplicity(3);
        REQUIRE(h2o_a != h2o_b);
    }
}

TEST_CASE("Molecule serialization") {
    Atom C("C", size_type{1}, 12.0107, 0.0, 0.0, 1.0);
    Atom O("O", size_type{1}, 15.999, 0.0, 0.0, 0.0);
    Atom C2("C", size_type{1}, 12.0107, 0.0, 0.0, -1.0);
    Molecule hd{C, O, C2};
    Molecule hd2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(hd);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(hd2);
    }
    REQUIRE(hd == hd2);
}
