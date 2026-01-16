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

import numpy as np
from chemist.wavefunction import AOs, TransformedAOs
from tensorwrapper import Tensor

from chemist import DecomposableDensity

from .test_density import h2_basis


class TestDensityClass(unittest.TestCase):
    def test_default_ctor(self):
        self.assertEqual(self.defaulted.basis_set.size(), 0)
        self.assertEqual(self.defaulted.value, Tensor())
        self.assertEqual(self.defaulted.left_factor, Tensor())

    def test_value_ctor(self):
        self.assertEqual(self.has_value1.value, self.value)
        self.assertEqual(self.has_value1.basis_set, self.aos)
        self.assertEqual(self.has_value1.left_factor, self.value)

        self.assertEqual(self.has_value2.value, self.value)
        self.assertEqual(self.has_value2.basis_set, self.aos)
        self.assertEqual(self.has_value2.left_factor, self.value)

    def test_left_factor(self):
        # Basic access tested in ctor tests
        # This is read-only, so let's just check that attempting to write raises
        with self.assertRaises(AttributeError):
            self.defaulted.left_factor = self.value

    def test_comparisons(self):
        self.assertEqual(self.defaulted, DecomposableDensity())
        self.assertNotEqual(self.defaulted, self.has_value1)
        self.assertEqual(self.has_value1, self.has_value2)

    def setUp(self):
        self.defaulted = DecomposableDensity()
        self.np_value = np.array([[1.0, 2.0], [3.0, 4.0]])
        self.value = Tensor(self.np_value)
        self.basis = h2_basis()
        self.aos = AOs(self.basis)
        self.transformed_aos = TransformedAOs(self.aos, self.value)
        self.has_value1 = DecomposableDensity(self.value, self.transformed_aos)
        self.has_value2 = DecomposableDensity(self.value, self.aos, self.value)
