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

from chemist import PointF, PointD, PointSetF, PointSetD, PointSetViewF, \
                    PointSetViewD
import unittest


def make_test_point_set_view(point_type, point_set_type, point_set_view_type):

    class TestPointSetView(unittest.TestCase):

        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.has_value.empty())

        def test_at(self):
            # Check values
            r0 = self.has_value.at(0)
            r1 = self.has_value.at(1)
            self.assertEqual(r0, self.r0)
            self.assertEqual(r1, self.r1)

            # Is view?
            r0.x = 42.0
            self.assertEqual(self.has_value_set.at(0).x, 42.0)

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.has_value.size(), 2)

        def test_as_points(self):
            self.assertEqual(self.defaulted.as_point_set(), point_set_type())
            self.assertEqual(self.has_value.as_point_set(), self.has_value_set)

        def test_comparisons(self):
            # Default vs default
            other_default = point_set_view_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Different number of points
            other_value_set = point_set_type()
            other_value_set.push_back(self.r0)
            other_value = point_set_view_type(other_value_set)
            self.assertNotEqual(self.has_value, other_value)
            self.assertFalse(self.has_value == other_value)

            # Same (and has points)
            other_value_set.push_back(self.r1)
            other_value = point_set_view_type(other_value_set)
            self.assertEqual(self.has_value, other_value)
            self.assertFalse(self.has_value != other_value)

            # Different order
            diff_order_set = point_set_type()
            diff_order_set.push_back(self.r1)
            diff_order_set.push_back(self.r0)
            diff_order = point_set_view_type(diff_order_set)
            self.assertNotEqual(self.has_value, diff_order)
            self.assertFalse(self.has_value == diff_order)

        def setUp(self):
            self.defaulted = point_set_view_type()
            self.r0 = point_type()
            self.r1 = point_type(1.0, 2.0, 3.0)
            self.has_value_set = point_set_type()
            self.has_value_set.push_back(self.r0)
            self.has_value_set.push_back(self.r1)
            self.has_value = point_set_view_type(self.has_value_set)

    return TestPointSetView


class TestPointSetViewF(
        make_test_point_set_view(PointF, PointSetF, PointSetViewF)):
    pass


class TestPointSetViewD(
        make_test_point_set_view(PointD, PointSetD, PointSetViewD)):
    pass
