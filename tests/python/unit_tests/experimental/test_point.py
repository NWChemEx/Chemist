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

from chemist.experimental import Point


class TestPoint(unittest.TestCase):
    def setUp(self):
        self.defaulted = Point()
        self.origin = Point(0.0, 0.0, 0.0)
        self.p = Point(1.0, 2.0, 3.0)
        # 3-4-5 in the xy-plane, so the magnitude is exactly representable
        self.p345 = Point(3.0, 4.0, 0.0)

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.get_x(), 0.0)
        self.assertEqual(self.defaulted.get_y(), 0.0)
        self.assertEqual(self.defaulted.get_z(), 0.0)

    def test_value_ctor(self):
        self.assertEqual(self.p.get_x(), 1.0)
        self.assertEqual(self.p.get_y(), 2.0)
        self.assertEqual(self.p.get_z(), 3.0)

    def test_get_coord(self):
        self.assertEqual(self.p.get_coord(0), 1.0)
        self.assertEqual(self.p.get_coord(1), 2.0)
        self.assertEqual(self.p.get_coord(2), 3.0)

        with self.assertRaises(IndexError):
            self.p.get_coord(3)

    def test_set_coord(self):
        self.p.set_coord(0, 4.0)
        self.p.set_coord(1, 5.0)
        self.p.set_coord(2, 6.0)
        self.assertEqual(self.p, Point(4.0, 5.0, 6.0))

        with self.assertRaises(IndexError):
            self.p.set_coord(3, 0.0)

    def test_set_x_y_z(self):
        self.defaulted.set_x(1.0)
        self.defaulted.set_y(2.0)
        self.defaulted.set_z(3.0)
        self.assertEqual(self.defaulted, self.p)

    def test_magnitude(self):
        self.assertEqual(self.origin.magnitude(), 0.0)
        self.assertEqual(self.p345.magnitude(), 5.0)

    def test_inner_product(self):
        self.assertEqual(self.origin.inner_product(self.p), 0.0)
        self.assertEqual(self.p.inner_product(self.p), 14.0)
        self.assertEqual(self.p.inner_product(self.p345), 11.0)

    def test_sub(self):
        self.assertEqual(self.p - self.origin, self.p)
        self.assertEqual(self.p - self.p, self.origin)
        self.assertEqual(self.p345 - self.p, Point(2.0, 2.0, -3.0))

    def test_comparisons(self):
        self.assertEqual(self.defaulted, self.origin)
        self.assertNotEqual(self.p, self.origin)
        self.assertNotEqual(self.p, Point(1.0, 2.0, 4.0))

    def test_repr(self):
        self.assertEqual(repr(self.p), "x : 1,\ny : 2,\nz : 3")


if __name__ == "__main__":
    unittest.main(verbosity=2)
