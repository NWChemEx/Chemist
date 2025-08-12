# Copyright 202r NWChemEx-Project
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


def make_test_charges_view(charges_view_type):
    class TestCharges(unittest.TestCase):
        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.has_value.empty())

        def test_at(self):
            # Check values
            q0 = self.has_value.at(0)
            q1 = self.has_value.at(1)
            self.assertEqual(q0, self.q0)
            self.assertEqual(q1, self.q1)

            # Are views
            q0.x = 42.0
            self.assertEqual(self.value_set.at(0).x, 42.0)

        def test_point_set(self):
            corr_ps = self.point_set()
            corr = self.point_set_view(corr_ps)
            self.assertEqual(self.defaulted.point_set, corr)

            corr_ps = self.point_set()
            corr_ps.push_back(self.point(0.0, 0.0, 0.0))
            corr_ps.push_back(self.point(2.0, 3.0, 4.0))
            corr = self.point_set_view(corr_ps)
            self.assertEqual(self.has_value.point_set, corr)

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.has_value.size(), 2)

        def test_comparisons(self):
            # Default vs default
            other_default = charges_view_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Different number of points
            values_set = self.charge_set()
            values_set.push_back(self.q0)
            other_values = charges_view_type(values_set)
            self.assertNotEqual(self.has_value, other_values)
            self.assertFalse(self.has_value == other_values)

            # Same (and has points)
            values_set.push_back(self.q1)
            other_values = charges_view_type(values_set)
            self.assertEqual(self.has_value, other_values)
            self.assertFalse(self.has_value != other_values)

            # Different order
            diff_order_set = self.charge_set()
            diff_order_set.push_back(self.q1)
            diff_order_set.push_back(self.q0)
            diff_order = charges_view_type(diff_order_set)
            self.assertNotEqual(self.has_value, diff_order)
            self.assertFalse(self.has_value == diff_order)

        def setUp(self):
            if charges_view_type == chemist.ChargesViewF:
                self.point_set = chemist.PointSetF
                self.point_set_view = chemist.PointSetViewF
                self.point = chemist.PointF
                self.charge_type = chemist.PointChargeF
                self.charge_set = chemist.ChargesF
            else:
                self.point_set = chemist.PointSetD
                self.point_set_view = chemist.PointSetViewD
                self.point = chemist.PointD
                self.charge_type = chemist.PointChargeD
                self.charge_set = chemist.ChargesD

            self.defaulted = charges_view_type()
            self.q0 = self.charge_type()
            self.q1 = self.charge_type(1.0, 2.0, 3.0, 4.0)
            self.value_set = self.charge_set()
            self.value_set.push_back(self.q0)
            self.value_set.push_back(self.q1)
            self.has_value = charges_view_type(self.value_set)

    return TestCharges


class TestChargesF(make_test_charges_view(chemist.ChargesViewF)):
    pass


class TestChargesD(make_test_charges_view(chemist.ChargesViewD)):
    pass
