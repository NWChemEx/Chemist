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

from chemist import PrimitiveF, PrimitiveD, PointF, PointD
import unittest


def make_primitive_test_case(prim_type, center_type):
    """
    The test cases for PrimitiveF and PrimitiveD are basically the same aside 
    from the types of the class. This function essentially templates the test 
    case on the type of the point.

    :param point_type: Either ``PrimitiveF`` or ``PrimitiveD``
    """

    class TestPrimitive(unittest.TestCase):
        def test_ctor(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(self.defaulted.coefficient, 0.0)
            self.assertEqual(self.defaulted.exponent, 0.0)
            self.assertEqual(self.defaulted.center,
                             center_type(0.0, 0.0, 0.0))
            self.assertFalse(self.defaulted.is_null())

            self.assertEqual(self.from_coords.coefficient, 0.0)
            self.assertEqual(self.from_coords.exponent, 1.0)
            self.assertEqual(self.from_coords.center,
                             center_type(2.0, 3.0, 4.0))
            self.assertFalse(self.from_coords.is_null())

            self.assertEqual(self.from_center.coefficient, 5.0)
            self.assertEqual(self.from_center.exponent, 6.0)
            self.assertEqual(self.from_center.center,
                             center_type(7.0, 8.0, 9.0))
            self.assertFalse(self.from_center.is_null())

        def test_center(self):
            self.assertEqual(self.from_coords.center,
                             center_type(2.0, 3.0, 4.0))

            self.from_coords.center = center_type(7.0, 8.0, 9.0)

            self.assertEqual(self.from_coords.center,
                             center_type(7.0, 8.0, 9.0))

        def test_coefficient(self):
            self.assertEqual(self.from_coords.coefficient, 0.0)

            self.from_coords.coefficient = 1.0

            self.assertEqual(self.from_coords.coefficient, 1.0)

        def test_exponent(self):
            self.assertEqual(self.from_coords.exponent, 1.0)

            self.from_coords.exponent = 2.0

            self.assertEqual(self.from_coords.exponent, 2.0)

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertFalse(self.from_coords.is_null())

        def test_comparisons(self):
            # Default
            other_default = prim_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default equals
            other_from_coords = prim_type(0.0, 1.0, 2.0, 3.0, 4.0)
            self.assertEqual(self.from_coords, other_from_coords)
            self.assertFalse(self.from_coords != other_from_coords)

            # Different Coefficient
            p1 = prim_type(100.0, 1.0, 2.0, 3.0, 4.0)
            self.assertNotEqual(self.from_coords, p1)
            self.assertFalse(self.from_coords == p1)

            # Different Exponent
            p2 = prim_type(0.0, 101.0, 2.0, 3.0, 4.0)
            self.assertNotEqual(self.from_coords, p2)
            self.assertFalse(self.from_coords == p2)

            # Different Center
            p3 = prim_type(0.0, 1.0, 102.0, 3.0, 4.0)
            self.assertNotEqual(self.from_coords, p3)
            self.assertFalse(self.from_coords == p3)

        def setUp(self):
            self.defaulted = prim_type()
            self.from_coords = prim_type(0.0, 1.0, 2.0, 3.0, 4.0)
            self.from_center = prim_type(5.0, 6.0, center_type(7.0, 8.0, 9.0))

    return TestPrimitive


class TestPrimitiveF(make_primitive_test_case(PrimitiveF, PointF)):
    pass


class TestPrimitiveD(make_primitive_test_case(PrimitiveD, PointD)):
    pass
