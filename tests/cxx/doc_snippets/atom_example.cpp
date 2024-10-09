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
#include "catch.hpp"
#include <chemist/molecule/atom.hpp>
#include <iostream>
#include <sstream>
#include <string>

using namespace chemist;
namespace chemist_examples {
// Creating Atom objects using different constructors
// Begin constructors
// Default constructor, properties are initialized to:
// name = '', atomic_number = 0, mass = 0.0
// x = 0.0, y = 0.0, z = 0.0, nuclear_charge = 0.0, n_electrons = 0.
Atom a0;
// Constructor with given name, atomic number, mass, and x, y, z coordinates.
// Nuclear charge and number of electrons are both set to the atomic number.
Atom a1("H", 1ul, 1.0079, 0.0, 0.0, 0.0);
// Constructor with given name, atomic number, mass, x, y, z, and  nuclear
// charge. Number of electrons is set to the atomic number.
Atom a2("H", 1ul, 1.0079, 0.0, 0.0, 0.0, 1.0);
// Constructor with given name, atomic number, mass, x, y, z, nuclear
// charge and number of electrons.
Atom a3("H", 1ul, 1.0079, 0.0, 0.0, 0.0, 1.0, 2);
// Copy constructor
Atom a4(a3);
// End constructors

int atom_example() {
    // Accessing the properties of an Atom object
    // Begin properties
    // Default constructor
    Atom a5;
    // Explicit constructor with the given states
    Atom a6("He", 2ul, 4.0026, 0.0, 0.0, 1.0, 2.0, 3);
    // a5 and a6 are not equal
    REQUIRE(a5 != a6);
    // Accessing the name (std::string)
    REQUIRE(a5.name() == "");
    REQUIRE(a6.name() == "He");
    a5.name() = "He";
    REQUIRE(a5.name() == a6.name());
    // Accessing the atomic number (unsigned integer)
    REQUIRE(a5.Z() == 0ul);
    REQUIRE(a6.Z() == 2ul);
    a5.Z() = 2ul;
    REQUIRE(a5.Z() == a6.Z());
    // Accessing the mass (double)
    REQUIRE(a5.mass() == 0.0);
    REQUIRE(a6.mass() == 4.0026);
    a5.mass() = 4.0026;
    REQUIRE(a5.mass() == a6.mass());
    // Accessing the coordinates
    REQUIRE(a5.x() == 0.0);
    REQUIRE(a5.y() == 0.0);
    REQUIRE(a5.z() == 0.0);
    REQUIRE(a6.z() == 1.0);
    a5.z() = 1.0;
    REQUIRE(a5.z() == a6.z());
    // Accessing the nuclear charge (double)
    REQUIRE(a5.nuclear_charge() == 0.0);
    REQUIRE(a6.nuclear_charge() == 2.0);
    a5.nuclear_charge() = 2.0;
    REQUIRE(a5.nuclear_charge() == a6.nuclear_charge());
    // Accessing the number of electrons (unsigned integer)
    REQUIRE(a5.n_electrons() == 0ul);
    REQUIRE(a6.n_electrons() == 3ul);
    a5.n_electrons() = 3ul;
    REQUIRE(a5.n_electrons() == a6.n_electrons());
    // Accessing the charge (double, read only)
    REQUIRE(a5.charge() == -1.0);
    REQUIRE(a6.charge() == -1.0);
    // After the changes above, a5 and a6 are equal
    REQUIRE(a5 == a6);
    // End properties

    // Begin printing
    std::stringstream ss;
    // ss will be: He 1.000000000000000 2.000000000000000 3.000000000000000
    ss << a1;
    // End printing

    // Begin serializing
    std::stringstream ssb, ssj, ssx;
    {
        cereal::BinaryOutputArchive output_binary_archive(ssb);
        cereal::JSONOutputArchive output_json_archive(ssj);
        cereal::XMLOutputArchive output_xml_archive(ssx);
        output_binary_archive(a3);
        output_json_archive(a3);
        output_xml_archive(a3);
    }
    // Deserializing an Atom object
    Atom a7, a8, a9;
    {
        cereal::BinaryInputArchive input_binary_archive(ssb);
        cereal::JSONInputArchive input_json_archive(ssj);
        cereal::XMLInputArchive input_xml_archive(ssx);
        input_binary_archive(a7);
        input_json_archive(a8);
        input_xml_archive(a9);
    }
    REQUIRE(a3 == a7);
    REQUIRE(a3 == a8);
    REQUIRE(a3 == a9);
    // End serializing
    return 0;
} // atom_example()
} // namespace chemist_examples
