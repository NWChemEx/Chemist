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


class TestNucleiView(unittest.TestCase):

    def test_empty(self):
        self.assertTrue(self.defaulted.empty())
        self.assertFalse(self.has_value.empty())

    def test_at(self):
        # Check values
        n0 = self.has_value.at(0)
        n1 = self.has_value.at(1)
        self.assertEqual(n0, self.n0)
        self.assertEqual(n1, self.n1)

        # Are views
        n0.x = 42.0
        self.assertEqual(self.value_set.at(0).x, 42.0)

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.has_value.size(), 2)

    def test_comparisons(self):
        # Default vs default
        other_default = chemist.NucleiView()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Different number of points
        other_set = chemist.Nuclei()
        other_set.push_back(self.n0)
        other_value = chemist.NucleiView(other_set)
        self.assertNotEqual(self.has_value, other_value)
        self.assertFalse(self.has_value == other_value)

        # Same (and has points)
        other_set.push_back(self.n1)
        other_value = chemist.NucleiView(other_set)
        self.assertEqual(self.has_value, other_value)
        self.assertFalse(self.has_value != other_value)

        # Different order
        diff_order_set = chemist.Nuclei()
        diff_order_set.push_back(self.n1)
        diff_order_set.push_back(self.n0)
        diff_order = chemist.NucleiView(diff_order_set)
        self.assertNotEqual(self.has_value, diff_order)
        self.assertFalse(self.has_value == diff_order)

    def test_iter(self):
        # Empty
        for n in self.defaulted:
            self.fail('Empty Nuclei should not iterate')

        # Non-empty
        for n in self.has_value:
            self.assertTrue(n == self.n0 or n == self.n1)

    def test_str(self):
        self.assertEqual(str(self.defaulted), '')
        self.assertEqual(
            str(self.has_value),
            ' 0.000000000000000 0.000000000000000 0.000000000000000\nH 2.000000000000000 3.000000000000000 4.000000000000000\n'
        )

    def setUp(self):
        self.defaulted = chemist.NucleiView()
        self.n0 = chemist.Nucleus()
        self.n1 = chemist.Nucleus('H', 1, 1.0, 2.0, 3.0, 4.0, 5.0)
        self.value_set = chemist.Nuclei()
        self.value_set.push_back(self.n0)
        self.value_set.push_back(self.n1)
        self.has_value = chemist.NucleiView(self.value_set)
