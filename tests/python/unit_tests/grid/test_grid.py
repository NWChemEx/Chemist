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

from chemist import Grid, GridPoint


class TestContractedGaussian(unittest.TestCase):
    def test_ctor(self):
        self.assertTrue(self.defaulted.empty())

        self.assertFalse(self.with_inputs.empty())
        self.assertEqual(self.with_inputs.size(), 2)

    def test_empty(self):
        self.assertTrue(self.defaulted.empty())
        self.assertFalse(self.with_inputs.empty())

    def test_at(self):
        self.assertEqual(self.with_inputs.at(0), GridPoint(1.0, 2.0, 3.0, 4.0))

        # Returning views?
        view = self.with_inputs.at(1)
        view.weight = 42.0
        view.point.z = 42.0
        self.assertEqual(
            self.with_inputs.at(1), GridPoint(42.0, 6.0, 7.0, 42.0)
        )

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.with_inputs.size(), 2)

    def test_comparisons(self):
        # Default
        other_default = Grid()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Non-default
        other_with_inputs = Grid(
            [GridPoint(1.0, 2.0, 3.0, 4.0), GridPoint(5.0, 6.0, 7.0, 8.0)]
        )
        self.assertEqual(self.with_inputs, other_with_inputs)
        self.assertFalse(self.with_inputs != other_with_inputs)

        # Different Points
        other_with_inputs = Grid(
            [GridPoint(1.0, 2.0, 3.0, 4.0), GridPoint(5.0, 6.0, 7.0, 42.0)]
        )
        self.assertNotEqual(self.with_inputs, other_with_inputs)
        self.assertTrue(self.with_inputs != other_with_inputs)

    def setUp(self):
        self.defaulted = Grid()
        self.with_inputs = Grid(
            [GridPoint(1.0, 2.0, 3.0, 4.0), GridPoint(5.0, 6.0, 7.0, 8.0)]
        )
