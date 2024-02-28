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

from chemist.basis_set import ShellViewF, ShellViewD
from chemist.basis_set import ShellF, ShellD
from chemist.basis_set import ContractedGaussianF, ContractedGaussianD
from chemist.basis_set import PrimitiveF, PrimitiveD
from chemist import PointF, PointD
from chemist import ShellType
import unittest


def make_shell_view_test_case(shell_view_type):
    """
    The test cases for ShellViewF and ShellViewD are basically the same aside 
    from the types of the class. This function essentially templates the test 
    case on the type of the point.

    :param point_type: Either ``ShellViewF`` or ``ShellViewD``
    """

    class TestShellView(unittest.TestCase):

        def test_ctor(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.defaulted.n_primitives(), 0)

            self.assertFalse(self.from_shell.is_null())
            self.assertEqual(self.from_shell.size(), 1)
            self.assertEqual(self.from_shell.n_primitives(), 1)
            self.assertEqual(self.from_shell.pure, ShellType.pure)
            self.assertEqual(self.from_shell.l, 0)
            self.assertEqual(self.from_shell.contracted_gaussian,
                             self.cg_type([0.0], [1.0], 2.0, 3.0, 4.0))
            self.assertEqual(self.from_shell.primitive(0),
                             self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0))

        def test_pure(self):
            self.assertEqual(self.from_shell.pure, self.pure)
            self.from_shell.pure = self.cart
            self.assertEqual(self.from_shell.pure, self.cart)
            self.assertEqual(self.shell.pure, self.cart)

        def test_l(self):
            self.assertEqual(self.from_shell.l, 0)
            self.from_shell.l = 1
            self.assertEqual(self.from_shell.l, 1)
            self.assertEqual(self.shell.l, 1)

        def test_contracted_gaussian(self):
            self.assertEqual(self.from_shell.contracted_gaussian,
                             self.cg_type([0.0], [1.0], 2.0, 3.0, 4.0))
            self.from_shell.contracted_gaussian = self.cg_type([1.0], [1.0],
                                                               2.0, 3.0, 4.0)
            self.assertEqual(self.from_shell.contracted_gaussian,
                             self.cg_type([1.0], [1.0], 2.0, 3.0, 4.0))
            self.assertEqual(self.shell.contracted_gaussian,
                             self.cg_type([1.0], [1.0], 2.0, 3.0, 4.0))

        def test_center(self):
            self.assertEqual(self.from_shell.center,
                             self.center_type(2.0, 3.0, 4.0))
            self.from_shell.center = self.center_type(7.0, 8.0, 9.0)
            self.assertEqual(self.from_shell.center, self.center)
            self.assertEqual(self.shell.center, self.center)

        def test_n_primitives(self):
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertEqual(self.from_shell.n_primitives(), 1)

        def test_primitive(self):
            self.assertEqual(self.from_shell.primitive(0),
                             self.prim_type(0.0, 1.0, 2.0, 3.0, 4.0))

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.from_shell.size(), 1)

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertFalse(self.from_shell.is_null())

        def test_comparisons(self):
            # Default
            other_default = shell_view_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default
            other_with_inputs = shell_view_type(self.shell)
            self.assertEqual(self.from_shell, other_with_inputs)
            self.assertFalse(self.from_shell != other_with_inputs)

            # Compare with viewed type
            default_viewed = self.shell_type()
            other_viewed = self.shell_type(self.pure, 0, [0.0], [1.0], 2.0,
                                           3.0, 4.0)

            self.assertEqual(self.defaulted, default_viewed)
            self.assertFalse(self.defaulted != default_viewed)
            self.assertEqual(self.from_shell, other_viewed)
            self.assertFalse(self.from_shell != other_viewed)

            self.assertNotEqual(self.defaulted, other_viewed)
            self.assertFalse(self.defaulted == other_viewed)
            self.assertNotEqual(self.from_shell, default_viewed)
            self.assertFalse(self.from_shell == default_viewed)

            # Different purity
            shell1 = self.shell_type(self.cart, 0, [0.0], [1.0], 2.0, 3.0, 4.0)
            shellview1 = shell_view_type(shell1)
            self.assertNotEqual(self.from_shell, shellview1)
            self.assertTrue(self.from_shell != shellview1)

            # Different angular momentum
            shell2 = self.shell_type(self.pure, 1, [0.0], [1.0], 2.0, 3.0, 4.0)
            shellview2 = shell_view_type(shell2)
            self.assertNotEqual(self.from_shell, shellview2)
            self.assertTrue(self.from_shell != shellview2)

            # Different contracted Gaussian
            shell3 = self.shell_type(self.pure, 0, self.cg)
            shellview3 = shell_view_type(shell3)
            self.assertNotEqual(self.from_shell, shellview3)
            self.assertTrue(self.from_shell != shellview3)

        def setUp(self):
            # "typedefs"
            if shell_view_type == ShellViewF:
                self.center_type = PointF
                self.prim_type = PrimitiveF
                self.cg_type = ContractedGaussianF
                self.shell_type = ShellF
            else:
                self.center_type = PointD
                self.prim_type = PrimitiveD
                self.cg_type = ContractedGaussianD
                self.shell_type = ShellD

            # Some inputs
            self.pure = ShellType.pure
            self.cart = ShellType.cartesian
            self.center = self.center_type(7.0, 8.0, 9.0)
            self.cg = self.cg_type([10.0], [11.0], self.center)
            self.shell = self.shell_type(self.pure, 0, [0.0], [1.0], 2.0, 3.0,
                                         4.0)

            # The instances being tested
            self.defaulted = shell_view_type()
            self.from_shell = shell_view_type(self.shell)

    return TestShellView


class TestShellViewF(make_shell_view_test_case(ShellViewF)):
    pass


class TestShellViewD(make_shell_view_test_case(ShellViewD)):
    pass
