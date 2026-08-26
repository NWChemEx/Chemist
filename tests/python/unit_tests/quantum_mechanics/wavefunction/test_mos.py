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

from chemist.wavefunction import AOs, MOs, TransformedAOs
from tensorwrapper import Tensor

from ..test_qm import h2_aos, h2_transform


class TestMOs(unittest.TestCase):
    def setUp(self):
        self.aos = h2_aos()
        self.transform = h2_transform()
        self.defaulted = MOs()
        self.has_value = MOs(self.aos, self.transform)

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.from_space, AOs())
        self.assertEqual(self.defaulted.transform, Tensor())

    def test_value_ctor(self):
        self.assertEqual(self.has_value.from_space, self.aos)
        self.assertEqual(self.has_value.transform, self.transform)

    def test_is_a_transformed_aos(self):
        self.assertIsInstance(self.has_value, TransformedAOs)

    def test_comparisons(self):
        self.assertEqual(self.defaulted, MOs())
        self.assertEqual(self.has_value, MOs(self.aos, self.transform))
        self.assertNotEqual(self.defaulted, self.has_value)

    def test_is_a_strong_type(self):
        # MOs adds no state to TransformedAOs, but it is a distinct space and
        # must not compare equal to a plain TransformedAOs with the same state.
        self.assertNotEqual(
            self.has_value, TransformedAOs(self.aos, self.transform)
        )
