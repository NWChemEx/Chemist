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

from chemist.fragmenting import Cap
from chemist import Nucleus
import unittest


class TestCap(unittest.TestCase):

    def test_default_ctor(self):
        self.assertEqual(len(self.defaulted), 0)

    def test_value_ctor(self):
        self.assertEqual(len(self.c12), 1)
        self.assertEqual(self.c12.get_anchor_index(), 1)
        self.assertEqual(self.c12.get_replaced_index(), 2)
        self.assertEqual(self.c12.at(0), self.atom0)

        self.assertEqual(len(self.c23), 2)
        self.assertEqual(self.c23.get_anchor_index(), 2)
        self.assertEqual(self.c23.get_replaced_index(), 3)
        self.assertEqual(self.c23.at(0), self.atom0)
        self.assertEqual(self.c23.at(1), self.atom1)

    def test_insert(self):
        other_c23 = Cap(2, 3, self.atom0)
        self.assertNotEqual(other_c23, self.c23)

        other_c23.insert(self.atom1)
        self.assertEqual(other_c23, self.c23)

    def test_at(self):
        self.assertRaises(IndexError, self.defaulted.at, 0)

        self.assertEqual(self.c12.at(0), self.atom0)
        self.assertEqual(self.c12[0], self.atom0)
        self.assertRaises(IndexError, self.c12.at, 1)
        self.assertRaises(IndexError, self.c12.__getitem__, 1)

        self.assertEqual(self.c23.at(0), self.atom0)
        self.assertEqual(self.c23.at(1), self.atom1)
        self.assertRaises(IndexError, self.c23.at, 2)

        # Is writeable
        self.c12[0] = self.atom0
        self.assertEqual(self.c12[0], self.atom0)

    def test_size(self):
        self.assertEqual(len(self.defaulted), 0)
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(len(self.c12), 1)
        self.assertEqual(self.c12.size(), 1)
        self.assertEqual(len(self.c23), 2)
        self.assertEqual(self.c23.size(), 2)

    def test_set_anchor_index(self):
        self.defaulted.set_anchor_index(1)
        self.assertEqual(self.defaulted.get_anchor_index(), 1)

        # Can overwrite existing value
        self.c12.set_anchor_index(3)
        self.assertEqual(self.c12.get_anchor_index(), 3)

    def test_get_anchor_index(self):
        self.assertRaises(RuntimeError, self.defaulted.get_anchor_index)

        self.assertEqual(self.c12.get_anchor_index(), 1)
        self.assertEqual(self.c23.get_anchor_index(), 2)

    def test_set_replaced_index(self):
        self.defaulted.set_replaced_index(1)
        self.assertEqual(self.defaulted.get_replaced_index(), 1)

        # Can overwrite
        self.c12.set_replaced_index(3)
        self.assertEqual(self.c12.get_replaced_index(), 3)

    def test_get_replaced_index(self):
        self.assertRaises(RuntimeError, self.defaulted.get_replaced_index)

        self.assertEqual(self.c12.get_replaced_index(), 2)
        self.assertEqual(self.c23.get_replaced_index(), 3)

    def test_comparisons(self):
        # Default v default
        other_default = Cap()
        self.assertTrue(other_default == self.defaulted)
        self.assertFalse(other_default != self.defaulted)

        # Default v non-default
        self.assertTrue(self.defaulted != self.c12)
        self.assertFalse(self.defaulted == self.c12)

        # Same value
        other_c12 = Cap(1, 2, self.atom0)
        self.assertTrue(self.c12 == other_c12)
        self.assertFalse(self.c12 != other_c12)

        # Different anchor
        c42 = Cap(4, 2, self.atom0)
        self.assertTrue(c42 != self.c12)
        self.assertFalse(c42 == self.c12)

        # Different replaced
        c14 = Cap(1, 4, self.atom0)
        self.assertTrue(c14 != self.c12)
        self.assertFalse(c14 == self.c12)

        # Different atom
        other_c12[0] = self.atom1
        self.assertTrue(self.c12 != other_c12)
        self.assertFalse(self.c12 == other_c12)

        # Different atom order
        other_c23 = Cap(2, 3, self.atom1, self.atom0)
        self.assertTrue(self.c23 != other_c23)
        self.assertFalse(self.c23 == other_c23)

    def setUp(self):
        self.defaulted = Cap()
        self.atom0 = Nucleus()
        self.atom1 = Nucleus("H", 1, 1837.289, 1.0, 2.0, 3.0)

        self.c12 = Cap(1, 2, self.atom0)
        self.c23 = Cap(2, 3, self.atom0, self.atom1)
