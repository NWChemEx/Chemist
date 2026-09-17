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

from chemist.experimental import ImmutablePointView, Point, PointView


class TestPointView(unittest.TestCase):
    def setUp(self):
        self.p = Point(1.0, 2.0, 3.0)
        self.p345 = Point(3.0, 4.0, 0.0)
        self.pv = PointView(self.p)
        self.cpv = ImmutablePointView(self.p345)

    def test_ctor_aliases(self):
        self.assertEqual(self.pv.get_x(), 1.0)

        # The view aliases self.p; it did not copy it
        self.p.set_x(42.0)
        self.assertEqual(self.pv.get_x(), 42.0)

    def test_get_coord(self):
        self.assertEqual(self.pv.get_coord(0), 1.0)
        self.assertEqual(self.pv.get_coord(1), 2.0)
        self.assertEqual(self.pv.get_coord(2), 3.0)

        with self.assertRaises(IndexError):
            self.pv.get_coord(3)

    def test_set_writes_through(self):
        self.pv.set_x(4.0)
        self.pv.set_y(5.0)
        self.pv.set_z(6.0)
        self.assertEqual(self.p, Point(4.0, 5.0, 6.0))

    def test_set_coord_writes_through(self):
        self.pv.set_coord(1, 42.0)
        self.assertEqual(self.p.get_y(), 42.0)

    def test_read_only_view_has_no_setters(self):
        self.assertFalse(hasattr(self.cpv, "set_x"))
        self.assertFalse(hasattr(self.cpv, "set_coord"))
        self.assertTrue(hasattr(self.pv, "set_x"))

    def test_as_point_is_a_deep_copy(self):
        copy = self.pv.as_point()
        self.assertEqual(copy, self.p)

        copy.set_x(42.0)
        self.assertEqual(self.p.get_x(), 1.0)

    def test_magnitude(self):
        self.assertEqual(self.cpv.magnitude(), 5.0)

    def test_inner_product(self):
        self.assertEqual(self.pv.inner_product(self.p), 14.0)
        self.assertEqual(self.pv.inner_product(PointView(self.p345)), 11.0)

    def test_sub(self):
        self.assertEqual(self.cpv - self.pv, Point(2.0, 2.0, -3.0))
        self.assertEqual(self.cpv - self.p, Point(2.0, 2.0, -3.0))

    def test_comparisons(self):
        self.assertEqual(self.pv, self.p)
        self.assertNotEqual(self.pv, self.p345)
        self.assertNotEqual(self.pv, PointView(self.p345))

    def test_repr(self):
        self.assertEqual(repr(self.pv), "x : 1,\ny : 2,\nz : 3")


if __name__ == "__main__":
    unittest.main(verbosity=2)
