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

from chemist.basis_set import PrimitiveViewF, PrimitiveViewD
from chemist.basis_set import PrimitiveF, PrimitiveD
from chemist import PointF, PointD
import unittest


def make_primitive_view_test_case(view_type):
    """
    The test cases for PrimitiveViewF and PrimitiveViewD are basically the same 
    aside from the types of the class. This function essentially templates the 
    test case on the type of the point.

    :param point_type: Either ``PrimitiveViewF`` or ``PrimitiveViewD``
    """

    class TestPrimitive(unittest.TestCase):
        def test_ctor(self):
            self.assertTrue(self.defaulted.is_null())

            self.assertEqual(self.from_prim.coefficient, 0.0)
            self.assertEqual(self.from_prim.exponent, 1.0)
            self.assertEqual(self.from_prim.center,
                             self.center_type(2.0, 3.0, 4.0))
            self.assertFalse(self.from_prim.is_null())

        def test_center(self):
            self.assertEqual(self.from_prim.center,
                             self.center_type(2.0, 3.0, 4.0))

            self.from_prim.center = self.center_type(7.0, 8.0, 9.0)

            self.assertEqual(self.from_prim.center,
                             self.center_type(7.0, 8.0, 9.0))

        def test_coefficient(self):
            self.assertEqual(self.from_prim.coefficient, 0.0)

            self.from_prim.coefficient = 1.0

            self.assertEqual(self.from_prim.coefficient, 1.0)

        def test_exponent(self):
            self.assertEqual(self.from_prim.exponent, 1.0)

            self.from_prim.exponent = 2.0

            self.assertEqual(self.from_prim.exponent, 2.0)

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertFalse(self.from_prim.is_null())

        def test_comparisons(self):
            # Default
            other_default = view_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default equals
            other_from_prim = view_type(self.primitive)
            self.assertEqual(self.from_prim, other_from_prim)
            self.assertFalse(self.from_prim != other_from_prim)

            # Compare with viewed type
            default_viewed = self.prim_type()
            other_viewed = self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0)

            self.assertEqual(self.defaulted, default_viewed)
            self.assertFalse(self.defaulted != default_viewed)
            self.assertEqual(self.from_prim, other_viewed)
            self.assertFalse(self.from_prim != other_viewed)

            self.assertNotEqual(self.defaulted, other_viewed)
            self.assertFalse(self.defaulted == other_viewed)
            self.assertNotEqual(self.from_prim, default_viewed)
            self.assertFalse(self.from_prim == default_viewed)

            # Different Coefficient
            p1 = self.prim_type(100.0, 1.0, 2.0, 3.0, 4.0)
            pv1 = view_type(p1)
            self.assertNotEqual(self.from_prim, pv1)
            self.assertFalse(self.from_prim == pv1)

            # Different Exponent
            p2 = self.prim_type(0.0, 101.0, 2.0, 3.0, 4.0)
            pv2 = view_type(p2)
            self.assertNotEqual(self.from_prim, pv2)
            self.assertFalse(self.from_prim == pv2)

            # Different Center
            p3 = self.prim_type(0.0, 1.0, 102.0, 3.0, 4.0)
            pv3 = view_type(p3)
            self.assertNotEqual(self.from_prim, pv3)
            self.assertFalse(self.from_prim == pv3)

        def setUp(self):
            # "typedefs"
            if view_type == PrimitiveViewF:
                self.center_type = PointF
                self.prim_type = PrimitiveF
            else:
                self.center_type = PointD
                self.prim_type = PrimitiveD

            # Input primitive
            self.primitive = self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0)

            # Instances being tested
            self.defaulted = view_type()
            self.from_prim = view_type(self.primitive)

    return TestPrimitive


class TestPrimitiveF(make_primitive_view_test_case(PrimitiveViewF)):
    pass


class TestPrimitiveD(make_primitive_view_test_case(PrimitiveViewD)):
    pass
