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
#include <chemist/molecule/molecule.hpp>
#include <iostream>
#include <sstream>
#include <string>

using namespace chemist;
namespace chemist_examples {
int molecule_example() {
    Atom a_H1("H", 1ul, 1.0079, 0.0, 0.0, 0.0);
    Atom a_H2("H", 1ul, 1.0079, 1.0, 0.0, 0.0);
    Atom a_O1("O", 8ul, 15.9994, 0.0, 1.0, 0.0);

    Nucleus n_H1("H", 1ul, 1.0079, 0.0, 0.0, 0.0);
    Nucleus n_H2("H", 1ul, 1.0079, 1.0, 0.0, 0.0);
    Nuclei n_set{n_H1, n_H2};

    // Creating Molecule objects using different constructors
    // Default constructor, no atoms, charge = 0.0, multiplicity = 1
    Molecule m0;
    // Creating Molecule with an initializer list of atoms
    // charge = 0.0, multiplicity = 1
    Molecule m1{a_H1, a_H2};
    // Creating Molecule with a specific charge and multiplicity and an
    // initializer list of atoms
    Molecule m2(1., 2, {a_H1, a_H2});
    // Creating Molecule with a specific charge and multiplicity and an
    // initializer list of nuclei
    Molecule m3(1., 2, n_set);
    // Copy constructor
    Molecule m4(m1);

    // Accessing the properties of an Molecule object
    // Accessing the size (number of atoms)
    std::size_t m1_size = m1.size();
    assert(m1_size == 2ul);
    assert(m0.size() == 0ul);
    // Accessing the charge
    double m1_charge = m1.charge();
    assert(m0.charge() == 0.0);
    assert(m1.charge == 0.0);
    assert(m2.charge == 1.0);
    // Accessing the multiplicity
    std::size_t m1_multiplicity = m1.multiplicity();
    assert(m1_multiplicity == 1ul);
    assert(m0.multiplicity() == 1ul);
    // Accessing the number of electrons
    assert(m1.n_electrons() == 2ul);
    assert(m0.n_electrons() == 0ul);
    assert(m2.n_electrons() == 1ul);

    // Comparing Molecule objects
    // Since m0 has no state
    assert(m0 != m1);
    // Since m1 and m2 has different states
    assert(m1 != m2);
    // Since all properties are equal, m2 == m3 is true
    assert(m2 == m3);

    // Modifying the properties of an Molecule object
    m1.set_charge(1.0);
    m1.set_multiplicity(2ul);
    assert(m1 == m2);
    // We can add atoms to a Molecule object
    m1.push_back(a_O1);
    assert(m1.size() == 3ul);

    // Printing an Molecule object
    std::stringstream ss;
    // ss will be:
    // H 0.000000000000000 0.000000000000000 0.000000000000000
    // H 1.000000000000000 0.000000000000000 0.000000000000000
    // O 0.000000000000000 1.000000000000000 0.000000000000000
    ss << m1;

    // Serializing an Molecule object
    std::stringstream ss2;
    {
        cereal::BinaryOutputArchive output_archive(ss2);
        output_archive(m1);
    }
    // Deserializing an Molecule object
    Molecule m5;
    {
        cereal::BinaryInputArchive input_archive(ss2);
        input_archive(m5);
    }
    assert(m1 == m5);
    return 0;
} // molecule_example()
} // namespace chemist_examples