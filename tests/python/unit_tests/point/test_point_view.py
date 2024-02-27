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

from chemist import PointF, PointD, PointViewF, PointViewD
import unittest


def make_point_view_test_case(point_type, point_view_type):
    """
    The test cases for PointViewF vs PointViewD are basically the same aside
    from the types of the class. This function essentially templates the test
    case on the type of the the point view (and the type of the point).

    :param point_type: Either ``PointF`` or ``PointD``
    :param point_view_type: ``PointViewF`` if ``point_type`` is ``PointF`` and
                            ``PointViewD`` if ``point_type`` is ``PointD``.

    """

    class TestPointView(unittest.TestCase):

        def test_ctor(self):
            self.assertEqual(self.defaulted_view.x, 0.0)
            self.assertEqual(self.defaulted_view.y, 0.0)
            self.assertEqual(self.defaulted_view.z, 0.0)

            self.assertEqual(self.r0_view.x, 1.0)
            self.assertEqual(self.r0_view.y, 2.0)
            self.assertEqual(self.r0_view.z, 3.0)

        def test_coord(self):
            # Test the initial value
            for i in range(3):
                self.assertEqual(self.defaulted_view.coord(i), 0.0)

        def test_x(self):
            # Test the initial value
            self.assertEqual(self.r0_view.x, 1.0)

            # Test we can write to it
            self.r0_view.x = 2.0

            # Test that our change took
            self.assertEqual(self.r0.x, 2.0)
            self.assertEqual(self.r0_view.x, 2.0)

            # Test that changes to r0 are reflected
            self.r0.x = 3.0
            self.assertEqual(self.r0.x, 3.0)
            self.assertEqual(self.r0_view.x, 3.0)

        def test_y(self):
            # Test the initial value
            self.assertEqual(self.r0_view.y, 2.0)

            # Test we can write to it
            self.r0_view.y = 3.0

            # Test that our change took
            self.assertEqual(self.r0.y, 3.0)
            self.assertEqual(self.r0_view.y, 3.0)

            # Test that changes to r0 are reflected
            self.r0.y = 4.0
            self.assertEqual(self.r0.y, 4.0)
            self.assertEqual(self.r0_view.y, 4.0)

        def test_z(self):
            # Test the initial value
            self.assertEqual(self.r0_view.z, 3.0)

            # Test we can write to it
            self.r0_view.z = 4.0

            # Test that our change took
            self.assertEqual(self.r0.z, 4.0)
            self.assertEqual(self.r0_view.z, 4.0)

            # Test that changes to r0 are reflected
            self.r0.z = 5.0
            self.assertEqual(self.r0.z, 5.0)
            self.assertEqual(self.r0_view.z, 5.0)

        def test_magnitude(self):
            self.assertEqual(self.defaulted_view.magnitude(), 0.0)
            # N.B. Python used doubles not floats, so in Python the C++ value
            #      has garbage after about the 6-th decimal place if point_type
            #      is PointF
            p = 6 if point_type == PointF else 10
            corr = 14.0**0.5
            self.assertAlmostEqual(self.r0_view.magnitude(), corr, places=p)

        def test_comparisons(self):
            # Default view vs default view
            other_defaulted = point_type()
            other_defaulted_view = point_view_type(other_defaulted)
            self.assertEqual(self.defaulted_view, other_defaulted_view)
            self.assertFalse(self.defaulted_view != other_defaulted_view)

            # Default view vs default (n.b. we ensure it's symmetric)
            self.assertEqual(self.defaulted_view, self.defaulted)
            self.assertEqual(self.defaulted, self.defaulted_view)

            # Different x coordinate
            r1 = point_type(0.0, 2.0, 3.0)
            r1_view = point_view_type(r1)
            self.assertNotEqual(self.r0_view, r1_view)
            self.assertFalse(self.r0_view == r1_view)

            self.assertNotEqual(self.r0, r1_view)
            self.assertNotEqual(r1_view, self.r0)

            # Different y coordinate
            r1 = point_type(1.0, 0.0, 3.0)
            r1_view = point_view_type(r1)
            self.assertNotEqual(self.r0_view, r1_view)
            self.assertFalse(self.r0_view == r1_view)

            # Different z coordinate
            r1 = point_type(1.0, 2.0, 0.0)
            r1_view = point_view_type(r1)
            self.assertNotEqual(self.r0_view, r1_view)
            self.assertFalse(self.r0_view == r1_view)

        def setUp(self):
            self.defaulted = point_type()
            self.defaulted_view = point_view_type(self.defaulted)
            self.r0 = point_type(1.0, 2.0, 3.0)
            self.r0_view = point_view_type(self.r0)

    return TestPointView


class TestPointViewF(make_point_view_test_case(PointF, PointViewF)):
    pass


class TestPointViewD(make_point_view_test_case(PointD, PointViewD)):
    pass
