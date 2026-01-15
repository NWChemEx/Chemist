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

from chemist.basis_set import AOBasisSetD

from ..test_qm import h2_basis


class TestAOs(unittest.TestCase):
    def test_default_ctor(self):
        self.assertEqual(self.defaulted.from_space.size(), 0)
        self.assertEqual(self.defaulted.from_space.ao_basis_set, AOBasisSetD())
        self.assertEqual(self.defaulted.transform, Tensor())

    def test_value_ctor(self):
        self.assertEqual(self.has_value.from_space, self.aos)
        self.assertEqual(self.has_value.transform, self.transform)

    def test_from_space(self):
        # Basic access tested in ctor tests
        # Now we need to test if we can write to it
        self.defaulted.from_space = self.aos
        self.assertEqual(self.defaulted.from_space, self.aos)

    def test_transform(self):
        # Basic access tested in ctor tests
        # Now we need to test if we can write to it
        self.defaulted.transform = self.transform
        self.assertEqual(self.defaulted.transform, self.transform)

    def test_comparisons(self):
        self.assertEqual(self.defaulted, TransformedAOs())
        self.assertNotEqual(self.defaulted, self.has_value)
        self.assertEqual(
            self.has_value, TransformedAOs(self.aos, self.transform)
        )

    def setUp(self):
        self.defaulted = TransformedAOs()
        self.empty_aos = AOs()
        self.np_value = np.array([[1.0, 2.0], [3.0, 4.0]])
        self.transform = Tensor(self.np_value)
        self.basis = h2_basis()
        self.aos = AOs(self.basis)
        self.has_value = TransformedAOs(self.aos, self.transform)
