# Copyright 2023 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import unittest

import chemist


class TestStringMethods(unittest.TestCase):
    def testMolecule(self):
        # Begin constructors
        # Default constructor, no atoms, charge = 0.0, multiplicity = 1
        m0 = chemist.Molecule()

        # PyBind11 doesn't support constructors with an initializer list
        # Therefore, using the push_back method to add atoms to the molecule
        a_H1 = chemist.Atom("H", 1, 1.0079, 0.0, 0.0, 0.0)
        a_H2 = chemist.Atom("H", 1, 1.0079, 1.0, 0.0, 0.0)
        m1 = chemist.Molecule()
        m1.push_back(a_H1)
        m1.push_back(a_H2)

        # Creating Molecule with a specific charge, multiplicity, and nuclei
        n_H1 = chemist.Nucleus("H", 1, 1.0079, 0.0, 0.0, 0.0)
        n_H2 = chemist.Nucleus("H", 1, 1.0079, 1.0, 0.0, 0.0)
        # Constructing a Nuclei object by pushing back Nucleus objects
        nuclei = chemist.Nuclei()
        nuclei.push_back(n_H1)
        nuclei.push_back(n_H2)
        m2 = chemist.Molecule(1, 2, nuclei)
        # End constructors

        # Begin properties
        # Accessing the size (number of atoms)
        m1_size = m1.size()
        assert m1_size == 2
        assert m0.size() == 0

        # Accessing the charge
        m1_charge = m1.charge()
        assert m1_charge == 0.0
        assert m0.charge() == 0.0
        assert m1.charge() == 0.0
        assert m2.charge() == 1.0

        # Accessing the multiplicity
        assert m0.multiplicity() == 1
        assert m1.multiplicity() == 1

        # Accessing the number of electrons
        assert m0.n_electrons() == 0
        assert m1.n_electrons() == 2
        assert m2.n_electrons() == 1

        # Comparing Molecule objects
        assert m0 != m1
        assert m1 != m2
        assert m2 == m2

        # Modifying the properties of a Molecule object
        m1.set_charge(1)
        m1.set_multiplicity(2)
        assert m1 == m2

        # We can add atoms to a Molecule object
        a_O1 = chemist.Atom("O", 8, 15.9994, 2.0, 0.0, 0.0)
        m1.push_back(a_O1)
        assert m1.size() == 3
        assert m1.n_electrons() == 9
        # End properties
