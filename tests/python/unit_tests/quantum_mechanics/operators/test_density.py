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

from chemist.qm_operator import DensityCMOsElectron, DensityMOsElectron

from ..test_qm import h2_cmos, h2_determinant, h2_mos


class TestDensityCMOsElectron(unittest.TestCase):
    def setUp(self):
        self.cmos = h2_cmos()
        self.weights = [2.0, 0.0]
        self.defaulted = DensityCMOsElectron()
        self.has_value = DensityCMOsElectron(self.cmos, self.weights)

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.defaulted.weights, [])

    def test_value_ctor(self):
        self.assertEqual(self.has_value.orbitals, self.cmos)
        self.assertEqual(self.has_value.weights, self.weights)
        self.assertEqual(self.has_value.size(), 2)

    def test_value_ctor_throws_if_sizes_differ(self):
        with self.assertRaises(RuntimeError):
            DensityCMOsElectron(self.cmos, [1.0])

    def test_wavefunction_ctor(self):
        # Building the density from a determinant contracts over the orbitals
        # that determinant does not occupy, i.e. it uses its occupations.
        rho = DensityCMOsElectron(h2_determinant())
        self.assertEqual(rho.orbitals, self.cmos)
        self.assertEqual(rho.weights, self.weights)

    def test_weights(self):
        self.has_value.weights = [1.0, 1.0]
        self.assertEqual(self.has_value.weights, [1.0, 1.0])

    def test_orbitals(self):
        self.defaulted.orbitals = self.cmos
        self.assertEqual(self.defaulted.orbitals, self.cmos)

    def test_comparisons(self):
        self.assertEqual(self.defaulted, DensityCMOsElectron())
        self.assertEqual(
            self.has_value, DensityCMOsElectron(self.cmos, self.weights)
        )
        self.assertNotEqual(self.defaulted, self.has_value)


class TestDensityMOsElectron(unittest.TestCase):
    def setUp(self):
        self.mos = h2_mos()
        self.has_value = DensityMOsElectron(self.mos, [2.0, 0.0])

    def test_value_ctor(self):
        self.assertEqual(self.has_value.orbitals, self.mos)
        self.assertEqual(self.has_value.weights, [2.0, 0.0])

    def test_comparisons(self):
        self.assertEqual(
            self.has_value, DensityMOsElectron(self.mos, [2.0, 0.0])
        )
        self.assertNotEqual(self.has_value, DensityMOsElectron())
