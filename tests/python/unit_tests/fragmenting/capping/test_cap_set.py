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

from chemist.fragmenting import Cap, CapSet
from chemist import Nucleus
import unittest


class TestCapSet(unittest.TestCase):

    def test_default_ctor(self):
        self.assertEqual(len(self.defaulted), 0)

    def test_value_ctor(self):
        self.assertEqual(self.has_values.size(), 2)
        self.assertEqual(self.has_values.at(0), self.cap0)
        self.assertEqual(self.has_values.at(1), self.cap1)

    def test_push_back(self):
        self.defaulted.push_back(self.cap0)
        self.defaulted.push_back(self.cap1)
        self.assertEqual(self.defaulted, self.has_values)

    def test_emplace_back(self):
        self.defaulted.push_back(self.cap0)

        # Cap with no atoms
        self.defaulted.emplace_back(1, 2)
        self.assertEqual(self.defaulted, self.has_values)

        atom0 = self.atom0

        # Cap with one atom
        self.defaulted.emplace_back(2, 3, atom0)
        self.assertEqual(self.defaulted.at(2), Cap(2, 3, atom0))

        # Cap with two atoms
        self.defaulted.emplace_back(3, 4, atom0, atom0)
        self.assertEqual(self.defaulted.at(3), Cap(3, 4, atom0, atom0))

    def test_at(self):
        self.assertRaises(IndexError, self.defaulted.at, 0)

        self.assertEqual(self.has_values.at(0), self.cap0)
        self.assertEqual(self.has_values.at(1), self.cap1)
        self.assertEqual(self.has_values[0], self.cap0)
        self.assertEqual(self.has_values[1], self.cap1)
        self.assertRaises(IndexError, self.has_values.at, 2)
        self.assertRaises(IndexError, self.has_values.__getitem__, 2)

        # Is writeable
        self.has_values[1] = self.cap0
        self.assertEqual(self.has_values[1], self.cap0)

    def test_size(self):
        self.assertEqual(len(self.defaulted), 0)
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(len(self.has_values), 2)
        self.assertEqual(self.has_values.size(), 2)

    def test_comparisons(self):
        # Default v default
        other_default = CapSet()
        self.assertTrue(other_default == self.defaulted)
        self.assertFalse(other_default != self.defaulted)

        # Default v non-default
        self.assertTrue(self.defaulted != self.has_values)
        self.assertFalse(self.defaulted == self.has_values)

        # Same value
        other_values = CapSet(self.cap0, self.cap1)
        self.assertTrue(self.has_values == other_values)
        self.assertFalse(self.has_values != other_values)

        # Different order
        diff_order = CapSet(self.cap1, self.cap0)
        self.assertTrue(diff_order != self.has_values)
        self.assertFalse(diff_order == self.has_values)

        # Different caps
        cs0 = CapSet(self.cap0)
        cs1 = CapSet(self.cap1)
        self.assertTrue(cs0 != cs1)
        self.assertFalse(cs0 == cs1)

    def setUp(self):
        self.defaulted = CapSet()
        self.atom0 = Nucleus()
        self.cap0 = Cap()
        self.cap1 = Cap(1, 2)
        self.has_values = CapSet(self.cap0, self.cap1)
