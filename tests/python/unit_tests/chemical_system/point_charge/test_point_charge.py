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

from chemist import PointChargeD, PointChargeF, PointD, PointF


def make_point_charge_test(charge_type, point_type):
    class TestPointCharge(unittest.TestCase):
        def test_base(self):
            """
            This test makes sure we can access the base class's member
            functions/properties. From unit tests of the PointF/PointD
            classes we know the functions work so we're mainly ensuring they
            are accessible.
            """

            # Direct access to coordinates
            self.assertEqual(self.defaulted.x, 0.0)
            self.assertEqual(self.defaulted.y, 0.0)
            self.assertEqual(self.defaulted.z, 0.0)

            # Access via the coord function
            for i in range(3):
                self.assertEqual(self.defaulted.coord(i), 0.0)

            self.assertEqual(self.defaulted.magnitude(), 0.0)

            # Difference of two points a the origin
            r1 = point_type() - self.defaulted
            self.assertEqual(r1, point_type())

            # Repeat, this time with q0
            self.assertEqual(self.q0.x, 2.0)
            self.assertEqual(self.q0.y, 3.0)
            self.assertEqual(self.q0.z, 4.0)

            self.assertEqual(self.q0.coord(0), 2.0)
            self.assertEqual(self.q0.coord(1), 3.0)
            self.assertEqual(self.q0.coord(2), 4.0)

            # N.B. For charge_type == PointChargeF the underlying data type
            #      only has about six decimal places of precision.
            p = 6 if charge_type == PointChargeF else 10
            self.assertAlmostEqual(self.q0.magnitude(), 29**0.5, places=p)

            r1 = point_type() - self.q0
            self.assertEqual(r1, point_type(-2.0, -3.0, -4.0))

        def test_charge(self):
            # Test original values
            self.assertEqual(self.defaulted.charge, 0.0)
            self.assertEqual(self.q0.charge, 1.0)

            # Can change it
            self.defaulted.charge = -42.0
            self.assertEqual(self.defaulted.charge, -42.0)

        def test_comparisons(self):
            # Default vs. default
            other_default = charge_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Default vs non-default
            self.assertNotEqual(self.defaulted, self.q0)
            self.assertFalse(self.defaulted == self.q0)

            # Different charge
            q1 = charge_type(42.0, 2.0, 3.0, 4.0)
            self.assertNotEqual(self.q0, q1)
            self.assertFalse(self.q0 == q1)

            # Different x-coordinate
            q1 = charge_type(1.0, 42.0, 3.0, 4.0)
            self.assertNotEqual(self.q0, q1)
            self.assertFalse(self.q0 == q1)

            # Different y-coordinate
            q1 = charge_type(1.0, 2.0, 42.0, 4.0)
            self.assertNotEqual(self.q0, q1)
            self.assertFalse(self.q0 == q1)

            # Different z-coordinate
            q1 = charge_type(1.0, 2.0, 3.0, 42.0)
            self.assertNotEqual(self.q0, q1)
            self.assertFalse(self.q0 == q1)

            # Compare to a point_type with same state
            self.assertEqual(self.defaulted, point_type())
            self.assertEqual(point_type(), self.defaulted)

            # Compare to a point_type with different state
            self.assertNotEqual(self.q0, point_type())
            self.assertNotEqual(point_type(), self.q0)

        def setUp(self):
            self.defaulted = charge_type()
            self.q0 = charge_type(1.0, 2.0, 3.0, 4.0)

    return TestPointCharge


class TestPointChargeF(make_point_charge_test(PointChargeF, PointF)):
    pass


class TestPointChargeD(make_point_charge_test(PointChargeD, PointD)):
    pass
