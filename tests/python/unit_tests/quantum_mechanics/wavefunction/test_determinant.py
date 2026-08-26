# Copyright 2026 NWChemEx-Project
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

from chemist.wavefunction import (
    CMOs,
    DeterminantCMOs,
    DeterminantMOs,
    Wavefunction,
)

from ..test_qm import h2_cmos, h2_mos


class TestDeterminantCMOs(unittest.TestCase):
    def setUp(self):
        self.cmos = h2_cmos()
        self.defaulted = DeterminantCMOs()
        self.has_value = DeterminantCMOs([0], self.cmos)

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.orbital_indices, [])
        self.assertEqual(self.defaulted.orbitals, CMOs())

    def test_value_ctor(self):
        self.assertEqual(self.has_value.orbital_indices, [0])
        self.assertEqual(self.has_value.orbitals, self.cmos)

    def test_orbital_indices(self):
        self.has_value.orbital_indices = [0, 1]
        self.assertEqual(self.has_value.orbital_indices, [0, 1])

        # The C++ side is a set, so it de-duplicates and sorts
        self.has_value.orbital_indices = [1, 0, 1]
        self.assertEqual(self.has_value.orbital_indices, [0, 1])

    def test_orbitals(self):
        self.defaulted.orbitals = self.cmos
        self.assertEqual(self.defaulted.orbitals, self.cmos)

    def test_occupations(self):
        # h2_cmos spans two orbitals and only the first is occupied
        self.assertEqual(self.has_value.occupations(), [2, 0])
        both = DeterminantCMOs([0, 1], self.cmos)
        self.assertEqual(both.occupations(), [2, 2])

    def test_is_a_wavefunction(self):
        self.assertIsInstance(self.has_value, Wavefunction)

    def test_comparisons(self):
        self.assertEqual(self.defaulted, DeterminantCMOs())
        self.assertEqual(self.has_value, DeterminantCMOs([0], self.cmos))
        self.assertNotEqual(self.has_value, DeterminantCMOs([1], self.cmos))
        self.assertNotEqual(self.defaulted, self.has_value)


class TestDeterminantMOs(unittest.TestCase):
    def setUp(self):
        self.mos = h2_mos()
        self.has_value = DeterminantMOs([0], self.mos)

    def test_value_ctor(self):
        self.assertEqual(self.has_value.orbital_indices, [0])
        self.assertEqual(self.has_value.orbitals, self.mos)

    def test_comparisons(self):
        self.assertEqual(self.has_value, DeterminantMOs([0], self.mos))
        self.assertNotEqual(self.has_value, DeterminantMOs([1], self.mos))
