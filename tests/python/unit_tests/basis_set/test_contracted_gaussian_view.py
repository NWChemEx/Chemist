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

from chemist import ContractedGaussianViewF, ContractedGaussianViewD
from chemist import ContractedGaussianF, ContractedGaussianD
from chemist import PrimitiveF, PrimitiveD
from chemist import PointF, PointD
import unittest


def make_cg_view_test_case(cg_view_type):
    """
    The test cases for ContractedGaussianViewF and ContractedGaussianViewD are 
    basically the same aside from the types of the class. This function 
    essentially templates the test case on the type of the point.

    :param point_type: Either ``ContractedGaussianViewF`` or 
                       ``ContractedGaussianViewD``
    """

    class TestContractedGaussianView(unittest.TestCase):
        def test_ctor(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertTrue(self.defaulted.empty())

            self.assertFalse(self.from_cg.is_null())
            self.assertFalse(self.from_cg.empty())
            self.assertEqual(self.from_cg.size(), 1)
            self.assertEqual(self.from_cg.center,
                             self.center_type(2.0, 3.0, 4.0))

        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.from_cg.empty())

        def test_at(self):
            self.assertEqual(self.from_cg.at(0),
                             self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0))

            # Returning views?
            self.from_cg.at(0).coefficient = 5.0
            self.from_cg.at(0).exponent = 6.0
            self.assertEqual(self.from_cg.at(0),
                             self.prim_type(5.0, 6.0, 2.0, 3.0, 4.0))
            self.assertEqual(self.cg.at(0),
                             self.prim_type(5.0, 6.0, 2.0, 3.0, 4.0))

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.from_cg.size(), 1)

        def test_center(self):
            self.assertEqual(self.from_cg.center,
                             self.center_type(2.0, 3.0, 4.0))

            # Writable?
            self.from_cg.center = self.center_type(7.0, 8.0, 9.0)
            self.assertEqual(self.from_cg.center,
                             self.center_type(7.0, 8.0, 9.0))
            self.assertEqual(self.cg.center,
                             self.center_type(7.0, 8.0, 9.0))

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertFalse(self.from_cg.is_null())

        def test_comparisons(self):
            # Default
            other_default = cg_view_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default
            other_with_inputs = cg_view_type(self.cg)
            self.assertEqual(self.from_cg, other_with_inputs)
            self.assertFalse(self.from_cg != other_with_inputs)

            # Compare with viewed type
            default_viewed = self.cg_type()
            other_viewed = self.cg_type([0.0], [1.0], 2.0, 3.0, 4.0)

            self.assertEqual(self.defaulted, default_viewed)
            self.assertFalse(self.defaulted != default_viewed)
            self.assertEqual(self.from_cg, other_viewed)
            self.assertFalse(self.from_cg != other_viewed)

            self.assertNotEqual(self.defaulted, other_viewed)
            self.assertFalse(self.defaulted == other_viewed)
            self.assertNotEqual(self.from_cg, default_viewed)
            self.assertFalse(self.from_cg == default_viewed)

            # Different coefficients
            cg1 = self.cg_type([10.0], [1.0], 2.0, 3.0, 4.0)
            cgv1 = cg_view_type(cg1)
            self.assertNotEqual(self.from_cg, cgv1)
            self.assertTrue(self.from_cg != cgv1)

            # Different exponents
            cg2 = self.cg_type([0.0], [10.0], 2.0, 3.0, 4.0)
            cgv2 = cg_view_type(cg2)
            self.assertNotEqual(self.from_cg, cgv2)
            self.assertTrue(self.from_cg != cgv2)

            # Different center
            cg3 = self.cg_type([0.0], [1.0], 10.0, 3.0, 4.0)
            cgv3 = cg_view_type(cg3)
            self.assertNotEqual(self.from_cg, cgv3)
            self.assertTrue(self.from_cg != cgv3)

        def setUp(self):
            # "typedefs"
            if cg_view_type == ContractedGaussianViewF:
                self.center_type = PointF
                self.prim_type = PrimitiveF
                self.cg_type = ContractedGaussianF
            else:
                self.center_type = PointD
                self.prim_type = PrimitiveD
                self.cg_type = ContractedGaussianD

            # Input CG
            self.cg = self.cg_type([0.0], [1.0], 2.0, 3.0, 4.0)

            # Instances being tested
            self.defaulted = cg_view_type()
            self.from_cg = cg_view_type(self.cg)

    return TestContractedGaussianView


class TestContractedGaussianViewF(make_cg_view_test_case(ContractedGaussianViewF)):
    pass


class TestContractedGaussianViewD(make_cg_view_test_case(ContractedGaussianViewD)):
    pass
