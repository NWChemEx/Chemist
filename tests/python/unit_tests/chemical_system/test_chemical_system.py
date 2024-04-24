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


class TestChemicalSystem(unittest.TestCase):

    def test_molecule(self):
        self.assertEqual(self.defaulted.molecule, self.default_mol)
        self.assertEqual(self.has_default_mol.molecule, self.default_mol)
        self.assertEqual(self.has_mol.molecule, self.mol)

        # Molecule is referenced
        self.assertEqual(self.has_mol.molecule.size(), 1)
        mol_ref = self.has_mol.molecule
        mol_ref.push_back(chemist.Atom())
        self.assertEqual(self.has_mol.molecule.size(), 2)
        self.assertEqual(mol_ref, self.has_mol.molecule)

        # Can write to it
        self.defaulted.molecule = self.mol
        self.assertEqual(self.defaulted.molecule, self.mol)

    def test_comparisons(self):
        # Default vs. default
        other_default = chemist.ChemicalSystem()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs. non-default with default state
        self.assertEqual(self.defaulted, self.has_default_mol)
        self.assertFalse(self.defaulted != self.has_default_mol)

        # Non-default with default state vs. non-default with non-default state
        self.assertNotEqual(self.has_default_mol, self.has_mol)
        self.assertFalse(self.has_default_mol == self.has_mol)

    def setUp(self):
        self.defaulted = chemist.ChemicalSystem()
        self.default_mol = chemist.Molecule()
        self.mol = chemist.Molecule()
        self.mol.push_back(chemist.Atom())
        self.has_default_mol = chemist.ChemicalSystem(self.default_mol)
        self.has_mol = chemist.ChemicalSystem(self.mol)
