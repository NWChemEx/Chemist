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
    def testAtom(self):
        # Creating Nucleus objects using different constructors
        n0 = chemist.Nucleus()
        n1 = chemist.Nucleus("H", 1, 1.0079)
        n2 = chemist.Nucleus("H", 1, 1.0079, 0.0, 0.0, 0.0)

        # Accessing the properties of a Nucleus object
        # Accessing the name
        assert n0.name == ""
        assert n1.name == "H"
        # Accessing the atomic number
        assert n0.Z == 0
        assert n1.Z == 1
        # Accessing the mass
        assert n0.mass == 0.0
        assert n1.mass == 1.0079
        # Accessing the position
        assert n0.x == 0.0
        assert n0.y == 0.0
        assert n0.z == 0.0
        assert n1.x == 0.0
        # Accessing the charge
        assert n0.charge == 0.0
        assert n1.charge == 1.0

        # Comparing Nucleus objects
        assert n0 != n1
        assert n1 == n2

        # Modifying the properties of a Nucleus object
        n1.name = "He"
        n1.Z = 2
        n1.mass = 4.0026
        n1.x = 1.0
        n1.y = 2.0
        n1.z = 3.0
        assert n1 != n2
