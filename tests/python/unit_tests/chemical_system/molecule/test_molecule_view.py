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


class TestMoleculeView(unittest.TestCase):
    def test_empty(self):
        self.assertTrue(self.defaulted.empty())
        self.assertFalse(self.has_value.empty())

    def test_nuclei(self):
        corr_value = chemist.Nuclei()
        self.assertEqual(self.defaulted.nuclei, corr_value)

        corr_value.push_back(self.a0.nucleus)
        corr_value.push_back(self.a1.nucleus)
        self.assertEqual(self.has_value.nuclei, corr_value)

    def test_at(self):
        # Check values
        n0 = self.has_value.at(0)
        n1 = self.has_value.at(1)
        self.assertEqual(n0, self.a0.nucleus)
        self.assertEqual(n1, self.a1.nucleus)

        # Are views
        n0.x = 42.0
        self.assertEqual(self.has_value_mol.at(0).x, 42.0)

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.has_value.size(), 2)

    def test_comparisons(self):
        # Default vs default
        other_default_mol = chemist.Molecule()
        other_default = chemist.MoleculeView(other_default_mol)
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Different number of points
        other_default_mol.push_back(self.a0)
        other_value = chemist.MoleculeView(other_default_mol)
        self.assertNotEqual(self.has_value, other_value)
        self.assertFalse(self.has_value == other_value)

        # Same (and has points)
        other_default_mol.push_back(self.a1)
        other_value = chemist.MoleculeView(other_default_mol)
        self.assertEqual(self.has_value, other_value)
        self.assertFalse(self.has_value != other_value)

        # Different order
        diff_order_mol = chemist.Molecule()
        diff_order_mol.push_back(self.a1)
        diff_order_mol.push_back(self.a0)
        diff_order = chemist.MoleculeView(diff_order_mol)
        self.assertNotEqual(self.has_value, diff_order)
        self.assertFalse(self.has_value == diff_order)

    def test_iter(self):
        # Default
        for atom in self.defaulted:
            self.fail("Should not iterate over empty molecule")

        # Has value
        for i, atom in enumerate(self.has_value):
            self.assertEqual(atom, self.has_value.at(i))

    def setUp(self):
        self.defaulted_mol = chemist.Molecule()
        self.defaulted = chemist.MoleculeView(self.defaulted_mol)
        self.a0 = chemist.Atom()
        self.a1 = chemist.Atom("H", 1, 1.0, 2.0, 3.0, 4.0, 5.0)
        self.has_value_mol = chemist.Molecule()
        self.has_value_mol.push_back(self.a0)
        self.has_value_mol.push_back(self.a1)
        self.has_value = chemist.MoleculeView(self.has_value_mol)
