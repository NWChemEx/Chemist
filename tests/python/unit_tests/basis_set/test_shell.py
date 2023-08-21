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

from chemist.basis_set import ShellF, ShellD
from chemist.basis_set import ContractedGaussianF, ContractedGaussianD
from chemist.basis_set import PrimitiveF, PrimitiveD
from chemist import PointF, PointD
from chemist import ShellType
import unittest


def make_shell_test_case(shell_type):
    """
    The test cases for ShellF and ShellD are basically the same aside from the 
    types of the class. This function essentially templates the test case on the
    type of the point.

    :param point_type: Either ``ShellF`` or ``ShellD``
    """

    class TestShell(unittest.TestCase):
        def test_ctor(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.defaulted.n_primitives(), 0)

            self.assertFalse(self.with_inputs1.is_null())
            self.assertEqual(self.with_inputs1.size(), 1)
            self.assertEqual(self.with_inputs1.n_primitives(), 1)
            self.assertEqual(self.with_inputs1.pure, ShellType.pure)
            self.assertEqual(self.with_inputs1.l, 0)
            self.assertEqual(self.with_inputs1.contracted_gaussian,
                             self.cg_type([0.0], [1.0], 2.0, 3.0, 4.0))
            self.assertEqual(self.with_inputs1.primitive(0),
                             self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0))

            self.assertFalse(self.with_inputs2.is_null())
            self.assertEqual(self.with_inputs2.size(), 3)
            self.assertEqual(self.with_inputs2.n_primitives(), 1)
            self.assertEqual(self.with_inputs2.pure, ShellType.cartesian)
            self.assertEqual(self.with_inputs2.l, 1)
            self.assertEqual(self.with_inputs2.contracted_gaussian,
                             self.cg_type([5.0], [6.0], 7.0, 8.0, 9.0))
            self.assertEqual(self.with_inputs2.primitive(0),
                             self.prim_type(5.0, 6.0, 7.0, 8.0, 9.0))

            self.assertFalse(self.with_inputs3.is_null())
            self.assertEqual(self.with_inputs3.size(), 6)
            self.assertEqual(self.with_inputs3.n_primitives(), 1)
            self.assertEqual(self.with_inputs3.pure, ShellType.cartesian)
            self.assertEqual(self.with_inputs3.l, 2)
            self.assertEqual(self.with_inputs3.contracted_gaussian,
                             self.cg_type([10.0], [11.0], 7.0, 8.0, 9.0))
            self.assertEqual(self.with_inputs3.primitive(0),
                             self.prim_type(10.0, 11.0, 7.0, 8.0, 9.0))

        def test_pure(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(self.defaulted.pure, ShellType.pure)
            self.assertFalse(self.defaulted.is_null())

            self.assertEqual(self.with_inputs1.pure, ShellType.pure)

        def test_l(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(self.defaulted.l, 0)
            self.assertFalse(self.defaulted.is_null())

            self.assertEqual(self.with_inputs1.l, 0)

        def test_contracted_gaussian(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(self.defaulted.contracted_gaussian,
                             self.cg_type())
            self.assertFalse(self.defaulted.is_null())

            self.assertEqual(self.with_inputs1.contracted_gaussian,
                             self.cg_type([0.0], [1.0], 2.0, 3.0, 4.0))

        def test_n_primitives(self):
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertEqual(self.with_inputs1.n_primitives(), 1)

        def test_primitive(self):
            self.assertEqual(self.with_inputs1.primitive(0),
                             self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0))

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.with_inputs1.size(), 1)

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertFalse(self.with_inputs1.is_null())

        def test_comparisons(self):
            # Default
            other_default = shell_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default
            other_with_inputs = shell_type(
                self.pure, 0, [0.0], [1.0], 2.0, 3.0, 4.0)
            self.assertEqual(self.with_inputs1, other_with_inputs)
            self.assertFalse(self.with_inputs1 != other_with_inputs)

            # Different purity
            shell1 = shell_type(self.cart, 0, [0.0], [1.0], 2.0, 3.0, 4.0)
            self.assertNotEqual(self.with_inputs1, shell1)
            self.assertTrue(self.with_inputs1 != shell1)

            # Different angular momentum
            shell2 = shell_type(self.pure, 1, [0.0], [1.0], 2.0, 3.0, 4.0)
            self.assertNotEqual(self.with_inputs1, shell2)
            self.assertTrue(self.with_inputs1 != shell2)

            # Different contracted Gaussian
            shell3 = shell_type(self.pure, 0, self.cg)
            self.assertNotEqual(self.with_inputs1, shell3)
            self.assertTrue(self.with_inputs1 != shell3)

        def setUp(self):
            # "typedefs"
            if shell_type == ShellF:
                self.center_type = PointF
                self.prim_type = PrimitiveF
                self.cg_type = ContractedGaussianF
            else:
                self.center_type = PointD
                self.prim_type = PrimitiveD
                self.cg_type = ContractedGaussianD

            # Some inputs
            self.pure = ShellType.pure
            self.cart = ShellType.cartesian
            self.center = self.center_type(7.0, 8.0, 9.0)
            self.cg = self.cg_type([10.0], [11.0], self.center)

            # The instances being tested
            self.defaulted = shell_type()
            self.with_inputs1 = shell_type(
                self.pure, 0, [0.0], [1.0], 2.0, 3.0, 4.0)
            self.with_inputs2 = shell_type(
                self.cart, 1, [5.0], [6.0], self.center)
            self.with_inputs3 = shell_type(self.cart, 2, self.cg)

    return TestShell


class TestShellF(make_shell_test_case(ShellF)):
    pass


class TestShellD(make_shell_test_case(ShellD)):
    pass
