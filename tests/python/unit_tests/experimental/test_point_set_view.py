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

from chemist.experimental import (
    ImmutablePointSetView,
    Point,
    PointSet,
    PointSetView,
)


class TestPointSetView(unittest.TestCase):
    def setUp(self):
        self.p0 = Point(0.0, 1.0, 2.0)
        self.p1 = Point(3.0, 4.0, 5.0)
        self.p2 = Point(6.0, 7.0, 8.0)
        self.ps = PointSet([self.p0, self.p1, self.p2])
        self.psv = PointSetView(self.ps)
        self.cpsv = ImmutablePointSetView(self.ps)

    def test_default_ctor(self):
        defaulted = PointSetView()
        self.assertEqual(len(defaulted), 0)
        self.assertTrue(defaulted.empty())

    def test_ctor_aliases(self):
        self.assertEqual(len(self.psv), 3)

        # The view aliases self.ps; it did not copy it
        self.ps[0].set_x(42.0)
        self.assertEqual(self.psv[0].get_x(), 42.0)

    def test_getitem_writes_through(self):
        self.psv[1].set_y(42.0)
        self.assertEqual(self.ps[1].get_y(), 42.0)

    def test_setitem_writes_through(self):
        self.psv[0] = self.p2
        self.assertEqual(self.ps[0], self.p2)

    def test_getitem_out_of_range(self):
        with self.assertRaises(IndexError):
            self.psv[3]

    def test_read_only_view_hands_out_read_only_points(self):
        self.assertFalse(hasattr(self.cpsv[0], "set_x"))
        self.assertTrue(hasattr(self.psv[0], "set_x"))

        # ... and can not be written through itself
        self.assertFalse(hasattr(self.cpsv, "__setitem__"))

    def test_as_point_set_is_a_deep_copy(self):
        copy = self.cpsv.as_point_set()
        self.assertEqual(copy, self.ps)

        copy[0].set_x(42.0)
        self.assertEqual(self.ps[0].get_x(), 0.0)

    def test_iteration(self):
        seen = [p.as_point() for p in self.cpsv]
        self.assertEqual(seen, [self.p0, self.p1, self.p2])

    def test_coordinates(self):
        self.assertEqual(self.psv.get_x_coordinates(), [0.0, 3.0, 6.0])
        self.assertEqual(self.cpsv.get_z_coordinates(), [2.0, 5.0, 8.0])

    def test_comparisons(self):
        self.assertEqual(self.psv, PointSetView(self.ps))
        self.assertNotEqual(self.psv, PointSetView(PointSet([self.p0])))


if __name__ == "__main__":
    unittest.main(verbosity=2)
