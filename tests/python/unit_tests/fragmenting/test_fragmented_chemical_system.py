# Copyright 2024 NWChemEx Community
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

from chemist import Atom, Molecule, ChemicalSystem
from chemist.fragmenting import FragmentedNuclei, FragmentedMolecule, \
    FragmentedChemicalSystem
import unittest


class TestFragmentedChemicalSystem(unittest.TestCase):

    def test_ctors(self):
        self.assertEqual(self.defaulted.supersystem(), self.empty_ss)
        self.assertEqual(self.defaulted.size(), 0)

        self.assertEqual(self.no_frags.supersystem(), self.ss)
        self.assertEqual(self.no_frags.size(), 0)

        self.assertEqual(self.frags.supersystem(), self.ss)
        self.assertEqual(self.frags.size(), 2)
        self.assertEqual(self.frags.at(0), self.frag0)
        self.assertEqual(self.frags.at(1), self.frag1)

    def test_supersystem(self):
        self.assertEqual(self.defaulted.supersystem(), self.empty_ss)
        self.assertEqual(self.no_frags.supersystem(), self.ss)
        self.assertEqual(self.frags.supersystem(), self.ss)

    def test_at(self):
        self.assertEqual(self.frags.at(0), self.frag0)
        self.assertEqual(self.frags.at(1), self.frag1)

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.no_frags.size(), 0)
        self.assertEqual(self.frags.size(), 2)

    def test_comparisons(self):
        # Default vs default
        rhs = FragmentedChemicalSystem()
        self.assertEqual(self.defaulted, rhs)

        # Default vs empty superset
        rhs = FragmentedChemicalSystem(FragmentedMolecule())
        self.assertEqual(self.defaulted, rhs)

        # Default vs empty
        self.assertNotEqual(self.defaulted, self.no_frags)

        # Default vs value
        self.assertNotEqual(self.defaulted, self.frags)

        # Empty vs empty
        rhs = FragmentedChemicalSystem(self.empty_frag_molecule)
        self.assertEqual(self.no_frags, rhs)

        # Empty vs value
        self.assertNotEqual(self.no_frags, self.frags)

        # Value vs value
        rhs = FragmentedChemicalSystem(self.frag_molecule)
        self.assertEqual(self.frags, rhs)

        # Different fragments
        temp_nuclei = self.ss.molecule.nuclei
        rhs_nuclei = FragmentedNuclei(temp_nuclei.as_nuclei(), [[0, 1]])
        rhs_mol = FragmentedMolecule(rhs_nuclei, 0, 2)
        rhs = FragmentedChemicalSystem(rhs_mol)
        self.assertNotEqual(self.frags, rhs)

    def setUp(self):
        # Supersystems
        self.h = Atom('H', 1, 1.0, 2.0, 3.0, 4.0)
        self.he = Atom('He', 2, 4.0, 5.0, 6.0, 7.0)
        self.ss = ChemicalSystem(Molecule(0, 2, [self.h, self.he]))
        self.empty_ss = ChemicalSystem()

        # Fragmented nuclei and fragmented molecule
        l01 = [[0], [1]]
        temp_nuclei = self.ss.molecule.nuclei
        self.frag_nuclei = FragmentedNuclei(temp_nuclei.as_nuclei(), l01)
        self.frag_molecule = FragmentedMolecule(self.frag_nuclei, 0, 2)
        self.empty_frag_molecule = FragmentedMolecule(self.ss.molecule)

        # Corr fragments
        self.frag0 = ChemicalSystem(Molecule(0, 2, [self.h]))
        self.frag1 = ChemicalSystem(Molecule(0, 1, [self.he]))

        # Fragmented ChemicalSystem
        self.defaulted = FragmentedChemicalSystem()
        self.no_frags = FragmentedChemicalSystem(self.empty_frag_molecule)
        self.frags = FragmentedChemicalSystem(self.frag_molecule)
