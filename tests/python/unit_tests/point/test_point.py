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

from chemist import PointD, PointF


def make_point_test_case(point_type):
    """
    The test cases for PointF and PointD are basically the same aside from the
    types of the class. This function essentially templates the test case on
    the type of the point.

    :param point_type: Either ``PointF`` or ``PointD``
    """

    class TestPoint(unittest.TestCase):
        def test_ctor(self):
            self.assertEqual(self.defaulted.x, 0.0)
            self.assertEqual(self.defaulted.y, 0.0)
            self.assertEqual(self.defaulted.z, 0.0)

            self.assertEqual(self.origin.x, 0.0)
            self.assertEqual(self.origin.y, 0.0)
            self.assertEqual(self.origin.z, 0.0)

            self.assertEqual(self.r0.x, 1.0)
            self.assertEqual(self.r0.y, 2.0)
            self.assertEqual(self.r0.z, 3.0)

        def test_coord(self):
            # Test the initial value
            for i in range(3):
                self.assertEqual(self.defaulted.coord(i), 0.0)

        def test_x(self):
            # Test the initial value
            self.assertEqual(self.r0.x, 1.0)

            # Test we can write to it
            self.r0.x = 2.0

            # Test that our change took
            self.assertEqual(self.r0.x, 2.0)

        def test_y(self):
            # Test the initial value
            self.assertEqual(self.r0.y, 2.0)

            # Test we can write to it
            self.r0.y = 3.0

            # Test that our change took
            self.assertEqual(self.r0.y, 3.0)

        def test_z(self):
            # Test the initial value
            self.assertEqual(self.r0.z, 3.0)

            # Test we can write to it
            self.r0.z = 4.0

            # Test that our change took
            self.assertEqual(self.r0.z, 4.0)

        def test_magnitude(self):
            self.assertEqual(self.defaulted.magnitude(), 0.0)
            self.assertEqual(self.origin.magnitude(), 0.0)
            # N.B. Python used doubles not floats, so in Python the C++ value
            #      has garbage after about the 6-th decimal place if point_type
            #      is PointF
            p = 6 if point_type == PointF else 10
            corr = 14.0**0.5
            self.assertAlmostEqual(self.r0.magnitude(), corr, places=p)

        def test_subtraction(self):
            r1 = self.origin - self.r0
            self.assertEqual(r1.x, -1.0)
            self.assertEqual(r1.y, -2.0)
            self.assertEqual(r1.z, -3.0)

        def test_comparisons(self):
            # Default v default
            other_default = point_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Default v manually set
            self.assertEqual(self.defaulted, self.origin)
            self.assertFalse(self.defaulted != self.origin)

            # Different x coordinate
            r1 = point_type(0.0, 2.0, 3.0)
            self.assertNotEqual(self.r0, r1)
            self.assertFalse(self.r0 == r1)

            # Different y coordinate
            r1 = point_type(1.0, 0.0, 3.0)
            self.assertNotEqual(self.r0, r1)
            self.assertFalse(self.r0 == r1)

            # Different z coordinate
            r1 = point_type(1.0, 2.0, 0.0)
            self.assertNotEqual(self.r0, r1)
            self.assertFalse(self.r0 == r1)

        def setUp(self):
            self.defaulted = point_type()
            self.origin = point_type(0.0, 0.0, 0.0)
            self.r0 = point_type(1.0, 2.0, 3.0)

    return TestPoint


class TestPointF(make_point_test_case(PointF)):
    pass


class TestPointD(make_point_test_case(PointD)):
    pass
