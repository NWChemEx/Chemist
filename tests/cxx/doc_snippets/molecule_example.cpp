/*
 * Copyright 2023 NWChemEx-Project
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

// Begin cereal includes
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
// End cereal includes
#include <catch2/catch.hpp>
#include <chemist/molecule/molecule.hpp>
#include <iostream>
#include <sstream>
#include <string>

using namespace chemist;
namespace chemist_examples {
int molecule_example() {
    // Begin constructors
    // Creating Molecule objects using different constructors
    // Default constructor, no atoms, charge = 0.0, multiplicity = 1
    Molecule m0;
    // Creating Molecule with an initializer list of atoms
    // charge = 0.0, multiplicity = 1
    Atom a_H1("H", 1ul, 1.0079, 0.0, 0.0, 0.0);
    Atom a_H2("H", 1ul, 1.0079, 1.0, 0.0, 0.0);
    Molecule m1{a_H1, a_H2};
    // Creating Molecule with a specific charge and multiplicity and an
    // initializer list of atoms
    Molecule m2(1., 2, {a_H1, a_H2});
    // Creating Molecule with a specific charge and multiplicity and an
    // initializer list of nuclei
    Nucleus n_H1("H", 1ul, 1.0079, 0.0, 0.0, 0.0);
    Nucleus n_H2("H", 1ul, 1.0079, 1.0, 0.0, 0.0);
    Nuclei n_set{n_H1, n_H2};
    Molecule m3(1, 2, n_set);
    // Copy constructor
    Molecule m4(m1);
    // End constructors

    // Begin properties
    // Accessing the properties of a Molecule object
    // Accessing the size (number of atoms)
    std::size_t m1_size = m1.size();
    REQUIRE(m1_size == 2ul);
    REQUIRE(m0.size() == 0ul);
    // Accessing the charge
    double m1_charge = m1.charge();
    REQUIRE(m0.charge() == 0);
    REQUIRE(m1.charge() == 0);
    REQUIRE(m2.charge() == 1);
    // Accessing the multiplicity
    std::size_t m1_multiplicity = m1.multiplicity();
    REQUIRE(m1_multiplicity == 1ul);
    REQUIRE(m0.multiplicity() == 1ul);
    // Accessing the number of electrons
    REQUIRE(m0.n_electrons() == 0ul);
    REQUIRE(m1.n_electrons() == 2ul);
    REQUIRE(m2.n_electrons() == 1ul);

    // Comparing Molecule objects
    // Since m0 has no state
    REQUIRE(m0 != m1);
    // Since m1 and m2 has different states
    REQUIRE(m1 != m2);
    // Since all states are equal, m2 == m3 is true
    REQUIRE(m2 == m3);

    // Modifying the properties of an Molecule object
    m1.set_charge(1.0);
    m1.set_multiplicity(2ul);
    REQUIRE(m1 == m2);
    // We can add atoms to a Molecule object
    Atom a_O1("O", 8ul, 15.9994, 0.0, 1.0, 0.0);
    m1.push_back(a_O1);
    REQUIRE(m1.size() == 3ul);
    REQUIRE(m1.n_electrons() == 9ul);
    // End properties

    // Printing an Molecule object
    std::stringstream ss;
    // ss will be:
    // H 0.000000000000000 0.000000000000000 0.000000000000000
    // H 1.000000000000000 0.000000000000000 0.000000000000000
    // O 0.000000000000000 1.000000000000000 0.000000000000000
    ss << m1;

    // Begin serializing
    std::stringstream ssb, ssj, ssx;
    {
        cereal::BinaryOutputArchive output_binary_archive(ssb);
        cereal::JSONOutputArchive output_json_archive(ssj);
        cereal::XMLOutputArchive output_xml_archive(ssx);
        output_binary_archive(m3);
        output_json_archive(m3);
        output_xml_archive(m3);
    }
    // Deserializing an Atom object
    Molecule m7, m8, m9;
    {
        cereal::BinaryInputArchive input_binary_archive(ssb);
        cereal::JSONInputArchive input_json_archive(ssj);
        cereal::XMLInputArchive input_xml_archive(ssx);
        input_binary_archive(m7);
        input_json_archive(m8);
        input_xml_archive(m9);
    }
    REQUIRE(m3 == m7);
    REQUIRE(m3 == m8);
    REQUIRE(m3 == m9);
    // End serializing
    return 0;
} // molecule_example()
} // namespace chemist_examples