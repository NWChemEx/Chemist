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

import chemist


def make_charge_view_test(charge_type):
    class TestChargeView(unittest.TestCase):
        def test_base(self):
            """
            This test makes sure we can access the base class's member
            functions/properties. From unit tests of the PointF/PointD
            classes we know the functions work so we're mainly ensuring they
            are accessible.
            """
            # Direct access to coordinates
            self.assertEqual(self.q0_view.x, 0.0)
            self.assertEqual(self.q0_view.y, 0.0)
            self.assertEqual(self.q0_view.z, 0.0)

            # Access via the coord function
            for i in range(3):
                self.assertEqual(self.q0_view.coord(i), 0.0)

            self.assertEqual(self.q0_view.magnitude(), 0.0)

            # Repeat, this time with q1
            self.assertEqual(self.q1_view.x, 2.0)
            self.assertEqual(self.q1_view.y, 3.0)
            self.assertEqual(self.q1_view.z, 4.0)

            self.assertEqual(self.q1_view.coord(0), 2.0)
            self.assertEqual(self.q1_view.coord(1), 3.0)
            self.assertEqual(self.q1_view.coord(2), 4.0)

            # N.B. For charge_type == PointChargeF the underlying data type
            #      only has about six decimal places of precision.
            p = 6 if charge_type == chemist.PointChargeF else 10
            self.assertAlmostEqual(
                self.q1_view.magnitude(), 29**0.5, places=p
            )

        def test_charge(self):
            # Test original values
            self.assertEqual(self.q0_view.charge, 0.0)
            self.assertEqual(self.q1_view.charge, 1.0)

            # Can change it
            self.q0_view.charge = -42.0
            self.assertEqual(self.q0_view.charge, -42.0)

            # Changes the actual charge
            self.assertEqual(self.q0.charge, -42.0)

        def test_comparisons(self):
            # Default view vs. default view
            other_default = charge_type()
            other_view = self.charge_view_type(other_default)
            self.assertEqual(self.q0_view, other_view)
            self.assertFalse(self.q0_view != other_view)

            # Default view vs. default value
            self.assertEqual(self.q0_view, other_default)
            self.assertFalse(self.q0_view != other_default)

            # Default view vs non-default view
            self.assertNotEqual(self.q0_view, self.q1_view)
            self.assertFalse(self.q0_view == self.q1_view)

            # Default view vs. non-default value
            self.assertNotEqual(self.q0_view, self.q1)
            self.assertFalse(self.q0_view == self.q1)

            # Different charge
            q1 = charge_type(42.0, 2.0, 3.0, 4.0)
            q1_view = self.charge_view_type(q1)
            self.assertNotEqual(self.q1_view, q1_view)
            self.assertFalse(self.q1_view == q1_view)

            # Different x-coordinate
            q1 = charge_type(1.0, 42.0, 3.0, 4.0)
            q1_view = self.charge_view_type(q1)
            self.assertNotEqual(self.q1_view, q1_view)
            self.assertFalse(self.q1_view == q1_view)

            # Different y-coordinate
            q1 = charge_type(1.0, 2.0, 42.0, 4.0)
            q1_view = self.charge_view_type(q1)
            self.assertNotEqual(self.q1_view, q1_view)
            self.assertFalse(self.q1_view == q1_view)

            # Different z-coordinate
            q1 = charge_type(1.0, 2.0, 3.0, 42.0)
            q1_view = self.charge_view_type(q1)
            self.assertNotEqual(self.q1_view, q1_view)
            self.assertFalse(self.q1_view == q1_view)

            # Compare to a point_type with same state
            r0 = self.point_type()
            r0_view = self.point_view(r0)
            self.assertEqual(self.q0_view, r0_view)
            self.assertEqual(r0_view, self.q0_view)

            # Compare to a point_type with different state
            self.assertNotEqual(self.q1, r0_view)
            self.assertNotEqual(r0_view, self.q1)

        def setUp(self):
            if charge_type == chemist.PointChargeF:
                self.charge_view_type = chemist.PointChargeViewF
                self.point_type = chemist.PointF
                self.point_view = chemist.PointViewF
            else:
                self.charge_view_type = chemist.PointChargeViewD
                self.point_type = chemist.PointD
                self.point_view = chemist.PointViewD

            self.q0 = charge_type()
            self.q0_view = self.charge_view_type(self.q0)
            self.q1 = charge_type(1.0, 2.0, 3.0, 4.0)
            self.q1_view = self.charge_view_type(self.q1)

    return TestChargeView


class TestChargeViewF(make_charge_view_test(chemist.PointChargeF)):
    pass


class TestChargeViewD(make_charge_view_test(chemist.PointChargeD)):
    pass
