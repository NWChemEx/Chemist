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
#include <chemist/nucleus/nucleus.hpp>
#include <iostream>
#include <sstream>
#include <string>

using namespace chemist;
namespace chemist_examples {

int nucleus_example() {
    // Creating Nucleus objects using different constructors
    // Default constructor, properties are initialized to:
    // name = '', atomic_number = 0, mass = 0.0
    // x = 0.0, y = 0.0, z = 0.0, charge = 0.0
    Nucleus n0;
    // Constructor with name, atomic number, and mass
    // Coordinates are initialized to: x = 0.0, y = 0.0, z = 0.0
    // Charge is initialized to atomic_number cast to a double, i.e., 1.0
    Nucleus n1("H", 1ul, 1.0079);
    // Constructor with name, atomic number, mass, and x, y, z coordinates
    // Charge is initialized to atomic_number cast to a double, i.e., 1.0
    Nucleus n2("H", 1ul, 1.0079, 0.0, 0.0, 0.0);

    // Accessing the properties of a Nucleus object
    // Accessing the name
    std::string n1_name = n1.name();
    assert(n1_name == "H");
    assert(n0.name() == "");
    // Accessing the atomic number
    std::size_t n1_Z = n1.Z();
    assert(n1_Z == 1ul);
    assert(n0.Z() == 0ul);
    // Accessing the mass
    double n1_mass = n1.mass();
    assert(n1_mass == 1.0079);
    assert(n0.mass() == 0.0);
    // Accessing the coordinates
    double n1_x = n1.x();
    double n1_y = n1.y();
    double n1_z = n1.z();
    assert(n1_x == 0.0);
    assert(n0_x == 0.0);
    // Accessing the charge
    double n1_charge = n1.charge();
    assert(n1_charge == 1.0);
    assert(n0_charge == 0.0);
    // Comparing Nucleus objects
    // Since n0 has no state while n1 has
    assert(n0 != n1);
    // Since all properties are equal, n1 == n2 is true
    assert(n1 == n2);

    // Modifying the properties of a Nucleus object
    // We can convert H into He (coolest fusion ever!)
    // Modifying the name
    n1.name() = "He";
    // Modifying the atomic number
    n1.Z() = 2ul;
    // Modifying the mass
    n1.mass() = 4.0026;
    // Modifying the coordinates
    n1.x() = 1.0;
    n1.y() = 2.0;
    n1.z() = 3.0;
    // Since we modified the state of n1, n1 is no longer equal to n2
    assert(n1 != n2);

    // Printing a Nucleus object
    std::stringstream ss;
    // ss will be: He 1.000000000000000 2.000000000000000 3.000000000000000
    ss << n1;

    // Serializing a Nucleus object
    std::stringstream ss2;
    {
        cereal::BinaryOutputArchive output_archive(ss2);
        output_archive(n1);
    }
    // Deserializing a Nucleus object
    Nucleus n3;
    {
        cereal::BinaryInputArchive input_archive(ss2);
        input_archive(n3);
    }
    assert(n1 == n3);
    return 0;
} // nucleus_example
} // namespace chemist_examples