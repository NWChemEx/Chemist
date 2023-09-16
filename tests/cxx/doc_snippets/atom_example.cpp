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

#include <cassert>
#include <cereal/archives/binary.hpp>
#include <chemist/molecule/atom.hpp>
#include <iostream>
#include <sstream>
#include <string>

using namespace chemist;
namespace chemist_examples {
// Creating Atom objects using different constructors
// Default constructor, properties are initialized to:
// name = '', atomic_number = 0, mass = 0.0
// x = 0.0, y = 0.0, z = 0.0, charge = 0.0
Atom a0;
// Constructor with name, atomic number, mass, and x, y, z coordinates
// charge = 0.0
Atom a1("H", 1ul, 1.0079, 0.0, 0.0, 0.0);
// Constructor with name, atomic number, mass, and x, y, z coordinates
// charge = 0.0
Atom a2("H", 1ul, 1.0079, 0.0, 0.0, 0.0, 0.0);
// Copy constructor
Atom a3(a1);
int atom_example() {
    // Accessing the properties of an Atom object
    // Accessing the name
    std::string a1_name = a1.name();
    // Accessing the atomic number
    std::size_t a1_Z = a1.Z();
    // Accessing the mass
    double a1_mass = a1.mass();
    // Accessing the coordinates
    double a1_x = a1.x();
    double a1_y = a1.y();
    double a1_z = a1.z();
    // Accessing the charge
    double a1_charge = a1.charge();

    // Comparing Atom objects
    // Since a0 has not state while a1 has
    assert(a0 != a1);
    // Since all properties are equal, a1 == a2 is true
    assert(a1 == a2);
    // Modifying the properties of an Atom object
    // We can convert H into He (coolest fusion ever!)
    // Modifying the name
    a1.name() = "He";
    // Modifying the atomic number
    a1.Z() = 2ul;
    // Modifying the mass
    a1.mass() = 4.0026;
    // Modifying the coordinates
    a1.x() = 1.0;
    a1.y() = 2.0;
    a1.z() = 3.0;

    // Printing an Atom object
    std::stringstream ss;
    // ss will be: He 1.000000000000000 2.000000000000000 3.000000000000000
    ss << a1;

    // Serializing an Atom object
    std::stringstream ss2;
    {
        cereal::BinaryOutputArchive output_archive(ss2);
        output_archive(a1);
    }
    // Deserializing an Atom object
    Atom a4;
    {
        cereal::BinaryInputArchive input_archive(ss2);
        input_archive(a4);
    }
    assert(a1 == a4);
    return 0;
} // atom_example()
} // namespace chemist_examples