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

// Forwad typedefs from the molecule
using value_type      = typename Molecule::value_type;
using reference       = typename Molecule::reference;
using const_reference = typename Molecule::const_reference;
using size_type       = typename Molecule::size_type;
using iterator        = typename Molecule::iterator;
using const_iterator  = typename Molecule::const_iterator;

// Some typedefs related to Atoms
using coord_type = typename value_type::coord_type;
using size_type  = typename value_type::size_type;

// Typedefs used for the tests
using cart_t      = std::array<double, 3>;
using vector_type = std::vector<value_type>;

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
        REQUIRE(mol.size() == 0);
    }

    vector_type atoms{Atom{"H", 1ul, 1.0079, cart_t{0.0, 0.0, 0.89}},
                      Atom{"D", 1ul, 2.0079, cart_t{0.0, 0.0, 0.0}}};

    SECTION("State CTor") {
        SECTION("An atom") {
            Molecule mol{atoms[0]};
            REQUIRE(mol.size() == 1);
            REQUIRE(mol[0] == atoms[0]);
        }

        SECTION("H-D molecule") {
            Molecule mol{atoms[0], atoms[1]};
            REQUIRE(mol.size() == 2);
            REQUIRE(mol[0] == atoms[0]);
            REQUIRE(mol[1] == atoms[1]);
        }
    }

    Molecule mol{atoms[0], atoms[1]};
    SECTION("Copy CTor") {
        Molecule mol2(mol);
        REQUIRE(mol2 == mol);
        REQUIRE(&mol.at(0).x() != &mol2.at(0).x());
    }

    SECTION("Copy Assignment") {
        Molecule mol2;
        auto& pmol = (mol2 = mol);
        REQUIRE(&pmol == &mol2);
        REQUIRE(mol2 == mol);
        REQUIRE(&mol.at(0).x() != &mol2.at(0).x());
    }

    SECTION("Move CTor") {
        Molecule copy(mol);
        Molecule mol2(std::move(mol));
        REQUIRE(mol2 == copy);
    }

    SECTION("Move Assignment") {
        Molecule mol2;
        Molecule copy(mol);
        auto& pmol = (mol2 = std::move(mol));
        REQUIRE(&pmol == &mol2);
        REQUIRE(mol2 == copy);
    }

    SECTION("Printing") {
        std::stringstream ss, corr_ss;
        corr_ss << atoms[0] << std::endl << atoms[1] << std::endl;
        ss << mol;
        REQUIRE(corr_ss.str() == ss.str());
    }

    SECTION("Equality") {
        using c_t = typename Atom::coord_type;
        using m_t = typename Atom::mass_type;
        Atom H{"H", 1ul,
               c_t{0.000000000000000, 1.579252144093028, 2.174611055780858},
               m_t{1837.4260218693814}};
        Atom O{"O", 8ul,
               c_t{0.000000000000000, 0.000000000000000, 0.000000000000000},
               m_t{29165.122045980286}};
        Atom H2{"H", 1ul,
                c_t{0.000000000000000, 1.579252144093028, -2.174611055780858},
                m_t{1837.4260218693814}};
        Molecule h2o_a{H, O, H2};
        Molecule h2o_b{H, O, H2};
        SECTION("Molecules are the same") { REQUIRE(h2o_a == h2o_b); }
        SECTION("Molecules are different") { REQUIRE(h2o_a != Molecule{}); }
    }
}

TEST_CASE("Molecule serialization") {
    Atom C("C", coord_type{0.0, 0.0, 1.0}, 12.0107, size_type{1});
    Atom O("O", coord_type{0.0, 0.0, 0.0}, 15.999, size_type{1});
    Atom C2("C", coord_type{0.0, 0.0, -1.0}, 12.0107, size_type{1});
    Molecule mol{C, O, C2};
    Molecule mol2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(mol);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(mol2);
    }
    REQUIRE(mol == mol2);
}
