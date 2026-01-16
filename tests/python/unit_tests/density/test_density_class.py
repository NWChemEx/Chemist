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
from chemist.wavefunction import AOs
from tensorwrapper import Tensor

from chemist import Density

from .test_density import h2_basis


class TestDensityClass(unittest.TestCase):
    def test_default_ctor(self):
        self.assertEqual(self.defaulted.basis_set.size(), 0)
        self.assertEqual(self.defaulted.value, Tensor())

    def test_value_ctor(self):
        self.assertEqual(self.has_value.value, self.value)
        self.assertEqual(self.has_value.basis_set, self.aos)

    def test_value(self):
        # Basic access tested in ctor tests
        # Now we need to test if we can write to it
        self.defaulted.value = self.value
        self.assertEqual(self.defaulted.value, self.value)

    def test_basis_set(self):
        # Basic access tested in ctor tests
        # Now we need to test if we can write to it
        self.defaulted.basis_set = self.aos
        self.assertEqual(self.defaulted.basis_set, self.aos)

    def test_comparisons(self):
        self.assertEqual(self.defaulted, Density())
        self.assertNotEqual(self.defaulted, self.has_value)
        self.assertEqual(self.has_value, Density(self.value, self.aos))

    def setUp(self):
        self.defaulted = Density()
        self.np_value = np.array([[1.0, 2.0], [3.0, 4.0]])
        self.value = Tensor(self.np_value)
        self.basis = h2_basis()
        self.aos = AOs(self.basis)
        self.has_value = Density(self.value, self.aos)
