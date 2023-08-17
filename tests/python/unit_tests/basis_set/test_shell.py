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

from chemist import ShellF, ShellD
from chemist import ContractedGaussianF, ContractedGaussianD
from chemist import PrimitiveF, PrimitiveD
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
            pass

        def test_empty(self):
            pass

        def test_size(self):
            pass

        def test_is_null(self):
            pass

        def test_comparisons(self):
            # Default

            # Non-default
            pass

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
