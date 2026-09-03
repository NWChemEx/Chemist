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

from chemist import GridPoint, GridPointView


class TestGridPointView(unittest.TestCase):
    def test_ctor(self):
        self.assertEqual(self.from_point.get_weight(), 1.0)
        self.assertEqual(self.from_point.get_x(), 2.0)
        self.assertEqual(self.from_point.get_y(), 3.0)
        self.assertEqual(self.from_point.get_z(), 4.0)

    def test_weight(self):
        self.assertEqual(self.from_point.get_weight(), 1.0)
        self.from_point.set_weight(42.0)
        self.assertEqual(self.from_point.get_weight(), 42.0)
        # View writes through to the aliased GridPoint
        self.assertEqual(self.grid_point.get_weight(), 42.0)

    def test_coords(self):
        self.assertEqual(self.from_point.get_x(), 2.0)

        # View can alter the value of the point being viewed
        self.from_point.set_x(-1.0)
        self.assertEqual(self.from_point.get_x(), -1.0)
        self.assertEqual(self.grid_point.get_x(), -1.0)

        # View reflects changes to the value of the point
        self.grid_point.set_x(42.0)
        self.assertEqual(self.from_point.get_x(), 42.0)

    def test_comparisons(self):
        # From GridPoint
        other_from_point = GridPointView(self.grid_point)
        self.assertEqual(self.from_point, other_from_point)
        self.assertFalse(self.from_point != other_from_point)

        # Different Weight
        other_point = GridPoint(42.0, 2.0, 3.0, 4.0)
        other_view = GridPointView(other_point)
        self.assertNotEqual(self.from_point, other_view)
        self.assertTrue(self.from_point != other_view)

        # Different Coordinates
        other_point = GridPoint(1.0, 42.0, 3.0, 4.0)
        other_view = GridPointView(other_point)
        self.assertNotEqual(self.from_point, other_view)
        self.assertTrue(self.from_point != other_view)

        # Compare views and points
        self.assertEqual(self.grid_point, self.from_point)
        self.assertFalse(self.grid_point != self.from_point)
        self.assertEqual(self.from_point, self.grid_point)
        self.assertFalse(self.from_point != self.grid_point)

    def setUp(self):
        self.grid_point = GridPoint(1.0, 2.0, 3.0, 4.0)
        self.from_point = GridPointView(self.grid_point)
