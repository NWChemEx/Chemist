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

from chemist import GridPoint, PointD


class TestGridPoint(unittest.TestCase):
    def test_ctor(self):
        self.assertEqual(self.defaulted.weight, 0.0)
        self.assertEqual(self.defaulted.point, PointD())

        self.assertEqual(self.from_coords.weight, 1.0)
        self.assertEqual(self.from_coords.point, PointD(2.0, 3.0, 4.0))

        self.assertEqual(self.from_point.weight, 5.0)
        self.assertEqual(self.from_point.point, PointD(6.0, 7.0, 8.0))

    def test_weight(self):
        self.assertEqual(self.from_coords.weight, 1.0)
        self.from_coords.weight = 42.0
        self.assertEqual(self.from_coords.weight, 42.0)

    def test_point(self):
        self.assertEqual(self.from_coords.point, PointD(2.0, 3.0, 4.0))
        self.from_coords.point = PointD(42.0, 42.0, 42.0)
        self.assertEqual(self.from_coords.point, PointD(42.0, 42.0, 42.0))
        self.from_coords.point.x = -1.0
        self.assertEqual(self.from_coords.point, PointD(-1.0, 42.0, 42.0))

    def test_comparisons(self):
        # Default
        other_default = GridPoint()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Non-default equals
        other_from_coords = GridPoint(1.0, 2.0, 3.0, 4.0)
        self.assertEqual(self.from_coords, other_from_coords)
        self.assertFalse(self.from_coords != other_from_coords)

        # Different Weight
        p1 = GridPoint(100.0, 2.0, 3.0, 4.0)
        self.assertNotEqual(self.from_coords, p1)
        self.assertFalse(self.from_coords == p1)

        # Different Coordinates
        p2 = GridPoint(1.0, 42.0, 42.0, 42.0)
        self.assertNotEqual(self.from_coords, p2)
        self.assertFalse(self.from_coords == p2)

    def setUp(self):
        self.defaulted = GridPoint()
        self.from_coords = GridPoint(1.0, 2.0, 3.0, 4.0)
        self.from_point = GridPoint(5.0, PointD(6.0, 7.0, 8.0))
