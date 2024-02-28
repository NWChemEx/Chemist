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

from chemist.basis_set import AtomicBasisSetViewF, AtomicBasisSetViewD
from chemist.basis_set import AtomicBasisSetF, AtomicBasisSetD
from chemist.basis_set import ShellF, ShellD
from chemist.basis_set import ContractedGaussianF, ContractedGaussianD
from chemist.basis_set import PrimitiveF, PrimitiveD
from chemist import PointF, PointD
from chemist import ShellType
import unittest


def make_abs_view_test_case(abs_view_type):
    """
    The test cases for AtomicBasisSetViewF and AtomicBasisSetViewD are basically
    the same aside from the types of the class. This function essentially 
    templates the test case on the type of the point.

    :param point_type: Either ``AtomicBasisSetViewF`` or ``AtomicBasisSetViewD``
    """

    class TestAtomicBasisSetView(unittest.TestCase):

        def test_ctor(self):
            self.assertEqual(self.defaulted.n_aos(), 0)
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertTrue(self.defaulted.empty())
            self.assertEqual(self.defaulted.size(), 0)
            self.assertTrue(self.defaulted.is_null())

            self.assertEqual(self.from_abs.basis_set_name, "name")
            self.assertEqual(self.from_abs.atomic_number, 0)
            self.assertEqual(self.from_abs.center,
                             self.center_type(0.0, 0.1, 0.2))
            self.assertEqual(self.from_abs.n_aos(), 1)
            self.assertEqual(self.from_abs.n_primitives(), 1)
            self.assertFalse(self.from_abs.empty())
            self.assertEqual(self.from_abs.size(), 1)
            self.assertFalse(self.from_abs.is_null())

        def test_basis_set_name(self):
            self.assertEqual(self.from_abs.basis_set_name, "name")
            self.from_abs.basis_set_name = "new name"
            self.assertEqual(self.from_abs.basis_set_name, "new name")
            self.assertEqual(self.abs.basis_set_name, "new name")

        def test_atomic_number(self):
            self.assertEqual(self.from_abs.atomic_number, 0)
            self.from_abs.atomic_number = 1
            self.assertEqual(self.from_abs.atomic_number, 1)
            self.assertEqual(self.abs.atomic_number, 1)

        def test_center(self):
            self.assertEqual(self.from_abs.center,
                             self.center_type(0.0, 0.1, 0.2))
            self.from_abs.center = self.center_type(1.0, 0.1, 0.2)
            self.assertEqual(self.from_abs.center,
                             self.center_type(1.0, 0.1, 0.2))
            self.assertEqual(self.abs.center, self.center_type(1.0, 0.1, 0.2))

        def test_n_aos(self):
            self.assertEqual(self.defaulted.n_aos(), 0)
            self.assertEqual(self.from_abs.n_aos(), 1)

        def test_n_primitives(self):
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertEqual(self.from_abs.n_primitives(), 1)

        def test_primitive_range(self):
            self.assertEqual(self.from_abs.primitive_range(0), (0, 1))

        def test_primitive_to_shell(self):
            self.assertEqual(self.from_abs.primitive_to_shell(0), 0)

        def test_primitive(self):
            self.assertEqual(self.from_abs.primitive(0),
                             self.prim_type(0.0, 1.0, 0.0, 0.1, 0.2))

        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.from_abs.empty())

        def test_at(self):
            self.assertEqual(
                self.from_abs.at(0),
                self.shell_type(self.pure, 0, [0.0], [1.0], 0.0, 0.1, 0.2))

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())

        def test_comparisons(self):
            # Default
            other_default = abs_view_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default
            other_with_inputs = abs_view_type(self.abs)
            self.assertEqual(self.from_abs, other_with_inputs)
            self.assertFalse(self.from_abs != other_with_inputs)

            # Compare with viewed type
            default_viewed = self.abs_type()
            other_viewed = self.abs_type("name", 0,
                                         self.center_type(0.0, 0.1, 0.2),
                                         [self.shell])

            self.assertEqual(self.defaulted, default_viewed)
            self.assertFalse(self.defaulted != default_viewed)
            self.assertEqual(self.from_abs, other_viewed)
            self.assertFalse(self.from_abs != other_viewed)

            self.assertNotEqual(self.defaulted, other_viewed)
            self.assertFalse(self.defaulted == other_viewed)
            self.assertNotEqual(self.from_abs, default_viewed)
            self.assertFalse(self.from_abs == default_viewed)

            # Different name
            abs1 = self.abs_type("different name", 0,
                                 self.center_type(0.0, 0.1, 0.2), [self.shell])
            absv1 = abs_view_type(abs1)
            self.assertNotEqual(self.from_abs, absv1)
            self.assertTrue(self.from_abs != absv1)

            # Different atomic number
            abs2 = self.abs_type("name", 1, self.center_type(0.0, 0.1, 0.2),
                                 [self.shell])
            absv2 = abs_view_type(abs2)
            self.assertNotEqual(self.from_abs, absv2)
            self.assertTrue(self.from_abs != absv2)

            # Different center
            abs3 = self.abs_type("name", 0, self.center_type(1.0, 0.1, 0.2),
                                 [self.shell])
            absv3 = abs_view_type(abs3)
            self.assertNotEqual(self.from_abs, absv3)
            self.assertTrue(self.from_abs != absv3)

            # Different shells
            abs4 = self.abs_type("name", 0, self.center_type(0.0, 0.1, 0.2))
            abs4.add_shell(self.pure, 0, self.cg_type([1.0], [1.0], 0, 0, 0))
            absv4 = abs_view_type(abs4)
            self.assertNotEqual(self.from_abs, absv4)
            self.assertTrue(self.from_abs != absv4)

        def setUp(self):
            # "typedefs"
            if abs_view_type == AtomicBasisSetViewF:
                self.center_type = PointF
                self.prim_type = PrimitiveF
                self.cg_type = ContractedGaussianF
                self.shell_type = ShellF
                self.abs_type = AtomicBasisSetF
            else:
                self.center_type = PointD
                self.prim_type = PrimitiveD
                self.cg_type = ContractedGaussianD
                self.shell_type = ShellD
                self.abs_type = AtomicBasisSetD

            # Some inputs
            self.pure = ShellType.pure
            self.shell = self.shell_type(self.pure, 0, [0.0], [1.0], 0.0, 0.0,
                                         0.0)
            self.abs = self.abs_type("name", 0,
                                     self.center_type(0.0, 0.1,
                                                      0.2), [self.shell])

            # The instances being tested
            self.defaulted = abs_view_type()
            self.from_abs = abs_view_type(self.abs)

    return TestAtomicBasisSetView


class TestAtomicBasisSetViewF(make_abs_view_test_case(AtomicBasisSetViewF)):
    pass


class TestAtomicBasisSetViewD(make_abs_view_test_case(AtomicBasisSetViewD)):
    pass
