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
from chemist.basis_set import (
    ContractedGaussianD,
    ContractedGaussianF,
    PrimitiveD,
    PrimitiveF,
)


def make_cg_test_case(cg_type):
    """
    The test cases for ContractedGaussianF and ContractedGaussianD are basically
    the same aside from the types of the class. This function essentially
    templates the test case on the type of the point.

    :param point_type: Either ``ContractedGaussianF`` or ``ContractedGaussianD``
    """

    class TestContractedGaussian(unittest.TestCase):
        def test_ctor(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertTrue(self.defaulted.empty())

            self.assertFalse(self.with_inputs1.is_null())
            self.assertFalse(self.with_inputs1.empty())
            self.assertEqual(self.with_inputs1.size(), 1)
            self.assertEqual(
                self.with_inputs1.center, self.center_type(2.0, 3.0, 4.0)
            )

            self.assertFalse(self.with_inputs2.is_null())
            self.assertFalse(self.with_inputs2.empty())
            self.assertEqual(self.with_inputs2.size(), 2)
            self.assertEqual(
                self.with_inputs2.center, self.center_type(7.0, 8.0, 9.0)
            )

        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.with_inputs1.empty())

        def test_at(self):
            self.assertEqual(
                self.with_inputs1.at(0),
                self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0),
            )

            # Returning views?
            self.with_inputs1.at(0).coefficient = 5.0
            self.with_inputs1.at(0).exponent = 6.0
            self.assertEqual(
                self.with_inputs1.at(0),
                self.prim_type(5.0, 6.0, 2.0, 3.0, 4.0),
            )

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.with_inputs1.size(), 1)

        def test_center(self):
            self.assertEqual(
                self.with_inputs1.center, self.center_type(2.0, 3.0, 4.0)
            )

            # Null CG returns origin and is no longer null
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(
                self.defaulted.center, self.center_type(0.0, 0.0, 0.0)
            )
            self.assertFalse(self.defaulted.is_null())

            # Writable?
            self.with_inputs1.center = self.center_type(7.0, 8.0, 9.0)
            self.assertEqual(
                self.with_inputs1.center, self.center_type(7.0, 8.0, 9.0)
            )

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertFalse(self.with_inputs1.is_null())

        def test_comparisons(self):
            # Default
            other_default = cg_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default
            other_with_inputs = cg_type([0.0], [1.0], 2.0, 3.0, 4.0)
            self.assertEqual(self.with_inputs1, other_with_inputs)
            self.assertFalse(self.with_inputs1 != other_with_inputs)

            # Different coefficients
            cg1 = cg_type([10.0], [1.0], 2.0, 3.0, 4.0)
            self.assertNotEqual(self.with_inputs1, cg1)
            self.assertTrue(self.with_inputs1 != cg1)

            # Different exponents
            cg2 = cg_type([0.0], [10.0], 2.0, 3.0, 4.0)
            self.assertNotEqual(self.with_inputs1, cg2)
            self.assertTrue(self.with_inputs1 != cg2)

            # Different center
            cg3 = cg_type([0.0], [1.0], 10.0, 3.0, 4.0)
            self.assertNotEqual(self.with_inputs1, cg3)
            self.assertTrue(self.with_inputs1 != cg3)

        def setUp(self):
            # "typedefs"
            if cg_type == ContractedGaussianF:
                self.center_type = PointF
                self.prim_type = PrimitiveF
            else:
                self.center_type = PointD
                self.prim_type = PrimitiveD

            self.defaulted = cg_type()
            self.with_inputs1 = cg_type([0.0], [1.0], 2.0, 3.0, 4.0)
            self.with_inputs2 = cg_type(
                [5.0, 5.1], [6.0, 6.1], self.center_type(7.0, 8.0, 9.0)
            )

    return TestContractedGaussian


class TestContractedGaussianF(make_cg_test_case(ContractedGaussianF)):
    pass


class TestContractedGaussianD(make_cg_test_case(ContractedGaussianD)):
    pass
