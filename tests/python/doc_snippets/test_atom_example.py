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

import chemist
import unittest

class TestStringMethods(unittest.TestCase):
    def testAtom(self):
        # Creating Atom objects using different constructors
        # Begin constructors
        # Default constructor, properties are initialized to:
        # name = '', atomic_number = 0, mass = 0.0
        # x = 0.0, y = 0.0, z = 0.0, nuclear_charge = 0.0, n_electrons = 0.
        a0 = chemist.Atom()
        # Constructor with given name, atomic number, mass, and x, y, z coordinates.
        # Nuclear charge and number of electrons are both set to the atomic number.
        a1 = chemist.Atom("H", 1, 1.0079, 0.0, 0.0, 0.0)
        # Constructor with given name, atomic number, mass, x, y, z, and  nuclear
        # charge. Number of electrons is set to the atomic number.
        a2 = chemist.Atom("H", 1, 1.0079, 0.0, 0.0, 0.0, 1.0)
        # Constructor with given name, atomic number, mass, x, y, z, nuclear
        # charge and number of electrons.
        a3 = chemist.Atom("H", 1, 1.0079, 0.0, 0.0, 0.0, 1.0, 2)
        # End constructors

        # Accessing the properties of an Atom object
        # Begin properties
        # Default constructor
        a5 = chemist.Atom()
        # Explicit constructor with the given states
        a6 = chemist.Atom("He", 2, 4.0026, 0.0, 0.0, 1.0, 2.0, 3)

        # a5 and a6 are different atoms
        assert a5 != a6

        # Accessing the name
        assert a5.name == ""
        assert a6.name == "He"
        a5.name = "He"
        assert a5.name == a6.name

        # Accessing the atomic number
        assert a5.Z == 0
        assert a6.Z == 2
        a5.Z = 2
        assert a5.Z == a6.Z

        # Accessing the mass
        assert a5.mass == 0.0
        assert a6.mass == 4.0026
        a5.mass = 4.0026
        assert a5.mass == a6.mass

        # Accessing the coordinates
        assert a5.x == 0.0
        assert a5.y == 0.0
        assert a5.z == 0.0
        assert a6.z == 1.0
        a5.z = 1.0
        assert a5.z == a6.z

        # Accessing the nuclear charge
        assert a5.nuclear_charge == 0.0
        assert a6.nuclear_charge == 2.0
        a5.nuclear_charge = 2.0
        assert a5.nuclear_charge == a6.nuclear_charge

        # Accessing the number of electrons
        assert a5.n_electrons == 0
        assert a6.n_electrons == 3
        a5.n_electrons = 3
        assert a5.n_electrons == a6.n_electrons

        # Accessing the charge, note the parenthesis (read only)
        assert a5.charge() == -1.0
        assert a6.charge() == -1.0

        # After all the changes above, a5 and a6 are now equal
        assert a5 == a6
        # End properties
