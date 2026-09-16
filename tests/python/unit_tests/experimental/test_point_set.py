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

from chemist.experimental import Point, PointSet


class TestPointSet(unittest.TestCase):
    def setUp(self):
        self.p0 = Point(0.0, 1.0, 2.0)
        self.p1 = Point(3.0, 4.0, 5.0)
        self.p2 = Point(6.0, 7.0, 8.0)
        self.defaulted = PointSet()
        self.ps = PointSet([self.p0, self.p1, self.p2])

    def test_default_ctor(self):
        self.assertEqual(len(self.defaulted), 0)
        self.assertTrue(self.defaulted.empty())

    def test_iterable_ctor(self):
        self.assertEqual(len(self.ps), 3)
        self.assertEqual(self.ps[0], self.p0)
        self.assertEqual(self.ps[2], self.p2)

    def test_push_back(self):
        self.defaulted.push_back(self.p0)
        self.defaulted.push_back(self.p1)
        self.defaulted.push_back(self.p2)
        self.assertEqual(self.defaulted, self.ps)

    def test_getitem_returns_a_view(self):
        p = self.ps[1]
        p.set_y(42.0)
        self.assertEqual(self.ps[1].get_y(), 42.0)

    def test_getitem_out_of_range(self):
        with self.assertRaises(IndexError):
            self.ps[3]

    def test_setitem(self):
        self.ps[0] = self.p2
        self.assertEqual(self.ps[0], self.p2)

    def test_iteration(self):
        seen = [p.as_point() for p in self.ps]
        self.assertEqual(seen, [self.p0, self.p1, self.p2])

    def test_coordinates_are_structure_of_arrays(self):
        # All three x-coordinates come back together, in point order
        self.assertEqual(self.ps.get_x_coordinates(), [0.0, 3.0, 6.0])
        self.assertEqual(self.ps.get_y_coordinates(), [1.0, 4.0, 7.0])
        self.assertEqual(self.ps.get_z_coordinates(), [2.0, 5.0, 8.0])

    def test_writing_through_a_view_moves_one_coordinate(self):
        self.ps[1].set_x(42.0)
        self.assertEqual(self.ps.get_x_coordinates(), [0.0, 42.0, 6.0])
        self.assertEqual(self.ps.get_y_coordinates(), [1.0, 4.0, 7.0])

    def test_comparisons(self):
        self.assertEqual(self.ps, PointSet([self.p0, self.p1, self.p2]))
        self.assertNotEqual(self.ps, self.defaulted)
        self.assertNotEqual(self.ps, PointSet([self.p0, self.p1]))


if __name__ == "__main__":
    unittest.main(verbosity=2)
