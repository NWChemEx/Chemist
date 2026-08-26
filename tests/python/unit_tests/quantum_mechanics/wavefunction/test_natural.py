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
from chemist.wavefunction import AOs, NaturalAOs, TransformedAOs
from tensorwrapper import Tensor

from ..test_qm import h2_aos, h2_transform


class TestNaturalAOs(unittest.TestCase):
    def setUp(self):
        self.aos = h2_aos()
        self.transform = h2_transform()
        self.diagonal = Tensor(np.array([-0.5, 0.5]))
        self.defaulted = NaturalAOs()
        self.has_value = NaturalAOs(self.diagonal, self.aos, self.transform)

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.from_space, AOs())
        self.assertEqual(self.defaulted.transform, Tensor())
        self.assertEqual(self.defaulted.diagonalized_matrix, Tensor())

    def test_value_ctor(self):
        self.assertEqual(self.has_value.from_space, self.aos)
        self.assertEqual(self.has_value.transform, self.transform)
        self.assertEqual(self.has_value.diagonalized_matrix, self.diagonal)

    def test_diagonalized_matrix(self):
        self.defaulted.diagonalized_matrix = self.diagonal
        self.assertEqual(self.defaulted.diagonalized_matrix, self.diagonal)

    def test_is_a_transformed_aos(self):
        self.assertIsInstance(self.has_value, TransformedAOs)

    def test_comparisons(self):
        self.assertEqual(self.defaulted, NaturalAOs())
        self.assertEqual(
            self.has_value, NaturalAOs(self.diagonal, self.aos, self.transform)
        )
        self.assertNotEqual(self.defaulted, self.has_value)
