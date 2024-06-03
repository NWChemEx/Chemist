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


class TestMolecule(unittest.TestCase):

    def test_empty(self):
        self.assertTrue(self.defaulted.empty())
        self.assertFalse(self.has_value.empty())

    def test_push_back(self):
        # Sanity check
        self.assertEqual(self.defaulted.size(), 0)

        # Add a0
        self.defaulted.push_back(self.a0)
        self.assertEqual(self.defaulted.size(), 1)
        self.assertEqual(self.defaulted.at(0), self.a0.nucleus)

        # Add a1
        self.defaulted.push_back(self.a1)
        self.assertEqual(self.defaulted.size(), 2)
        self.assertEqual(self.defaulted.at(0), self.a0.nucleus)
        self.assertEqual(self.defaulted.at(1), self.a1.nucleus)
        self.assertEqual(self.defaulted, self.has_value)

    def test_at(self):
        # Check values
        n0 = self.has_value.at(0)
        n1 = self.has_value.at(1)
        self.assertEqual(n0, self.a0.nucleus)
        self.assertEqual(n1, self.a1.nucleus)

        # Fails if lifetime not maintained
        chem_sys = chemist.ChemicalSystem(self.has_value)
        self.assertEqual(chem_sys.molecule.at(0), self.a0.nucleus)
        self.assertEqual(chem_sys.molecule.at(1), self.a1.nucleus)

        # Are views
        n0.x = 42.0
        self.assertEqual(self.has_value.at(0).x, 42.0)

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.has_value.size(), 2)

    def test_comparisons(self):
        # Default vs default
        other_default = chemist.Molecule()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Different number of points
        other_default.push_back(self.a0)
        self.assertNotEqual(self.has_value, other_default)
        self.assertFalse(self.has_value == other_default)

        # Same (and has points)
        other_default.push_back(self.a1)
        self.assertEqual(self.has_value, other_default)
        self.assertFalse(self.has_value != other_default)

        # Different order
        diff_order = chemist.Molecule()
        diff_order.push_back(self.a1)
        diff_order.push_back(self.a0)
        self.assertNotEqual(self.has_value, diff_order)
        self.assertFalse(self.has_value == diff_order)

    def test_iter(self):
        # Default
        for atom in self.defaulted:
            self.fail('Should not iterate over empty molecule')

        # Has value
        for i, atom in enumerate(self.has_value):
            self.assertEqual(atom, self.has_value.at(i))

    def test_str(self):
        # Default
        self.assertEqual(str(self.defaulted), '')

        # Has value
        self.assertEqual(
            str(self.has_value),
            'nelectrons : 0,\nname : ,\natomic number : 0,\nmass : 0,\n' +
            'charge : 0,\nx : 0,\ny : 0,\nz : 0\n' +
            'nelectrons : 1,\nname : H,\natomic number : 1,\nmass : 1,\n' +
            'charge : 5,\nx : 2,\ny : 3,\nz : 4\n')

    def setUp(self):
        self.defaulted = chemist.Molecule()
        self.a0 = chemist.Atom()
        self.a1 = chemist.Atom('H', 1, 1.0, 2.0, 3.0, 4.0, 5.0)
        self.has_value = chemist.Molecule()
        self.has_value.push_back(self.a0)
        self.has_value.push_back(self.a1)
