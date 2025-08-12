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

from chemist import PointD, PointF, ShellType
from chemist.basis_set import (
    AtomicBasisSetD,
    AtomicBasisSetF,
    ContractedGaussianD,
    ContractedGaussianF,
    PrimitiveD,
    PrimitiveF,
    ShellD,
    ShellF,
)


def make_abs_test_case(abs_type):
    """
    The test cases for AtomicBasisSetF and AtomicBasisSetD are basically the
    same aside from the types of the class. This function essentially templates
    the test case on the type of the point.

    :param point_type: Either ``AtomicBasisSetF`` or ``AtomicBasisSetD``
    """

    class TestAtomicBasisSet(unittest.TestCase):
        def test_ctor(self):
            self.assertEqual(self.defaulted.n_aos(), 0)
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertTrue(self.defaulted.empty())
            self.assertEqual(self.defaulted.size(), 0)
            self.assertTrue(self.defaulted.is_null())

            self.assertEqual(self.with_inputs0.basis_set_name, "name")
            self.assertEqual(self.with_inputs0.atomic_number, 0)
            self.assertEqual(
                self.with_inputs0.center, self.center_type(0.0, 0.1, 0.2)
            )
            self.assertEqual(self.with_inputs0.n_aos(), 0)
            self.assertEqual(self.with_inputs0.n_primitives(), 0)
            self.assertTrue(self.with_inputs0.empty())
            self.assertEqual(self.with_inputs0.size(), 0)
            self.assertFalse(self.with_inputs0.is_null())

            self.assertEqual(self.with_inputs1.basis_set_name, "name")
            self.assertEqual(self.with_inputs1.atomic_number, 1)
            self.assertEqual(
                self.with_inputs1.center, self.center_type(1.0, 1.1, 1.2)
            )
            self.assertEqual(self.with_inputs1.n_aos(), 0)
            self.assertEqual(self.with_inputs1.n_primitives(), 0)
            self.assertTrue(self.with_inputs1.empty())
            self.assertEqual(self.with_inputs1.size(), 0)
            self.assertFalse(self.with_inputs1.is_null())

            self.assertIsNone(self.with_inputs2.basis_set_name)
            self.assertIsNone(self.with_inputs2.atomic_number)
            self.assertEqual(
                self.with_inputs2.center, self.center_type(2.0, 2.1, 2.2)
            )
            self.assertEqual(self.with_inputs2.n_aos(), 0)
            self.assertEqual(self.with_inputs2.n_primitives(), 0)
            self.assertTrue(self.with_inputs2.empty())
            self.assertEqual(self.with_inputs2.size(), 0)
            self.assertFalse(self.with_inputs2.is_null())

            self.assertIsNone(self.with_inputs3.basis_set_name)
            self.assertIsNone(self.with_inputs3.atomic_number)
            self.assertEqual(
                self.with_inputs3.center, self.center_type(3.0, 3.1, 3.2)
            )
            self.assertEqual(self.with_inputs3.n_aos(), 0)
            self.assertEqual(self.with_inputs3.n_primitives(), 0)
            self.assertTrue(self.with_inputs3.empty())
            self.assertEqual(self.with_inputs3.size(), 0)
            self.assertFalse(self.with_inputs3.is_null())

            self.assertEqual(self.with_inputs4.basis_set_name, "name")
            self.assertEqual(self.with_inputs4.atomic_number, 4)
            self.assertEqual(
                self.with_inputs4.center, self.center_type(0.0, 0.0, 0.0)
            )
            self.assertEqual(self.with_inputs4.n_aos(), 0)
            self.assertEqual(self.with_inputs4.n_primitives(), 0)
            self.assertTrue(self.with_inputs4.empty())
            self.assertEqual(self.with_inputs4.size(), 0)
            self.assertFalse(self.with_inputs4.is_null())

            self.assertIsNone(self.with_inputs5.basis_set_name)
            self.assertIsNone(self.with_inputs5.atomic_number)
            self.assertEqual(
                self.with_inputs5.center, self.center_type(5.0, 5.1, 5.2)
            )
            self.assertEqual(self.with_inputs5.n_aos(), 1)
            self.assertEqual(self.with_inputs5.n_primitives(), 1)
            self.assertFalse(self.with_inputs5.empty())
            self.assertEqual(self.with_inputs5.size(), 1)
            self.assertFalse(self.with_inputs5.is_null())

            self.assertEqual(self.with_inputs6.basis_set_name, "name")
            self.assertEqual(self.with_inputs6.atomic_number, 6)
            self.assertEqual(
                self.with_inputs6.center, self.center_type(6.0, 6.1, 6.2)
            )
            self.assertEqual(self.with_inputs6.n_aos(), 1)
            self.assertEqual(self.with_inputs6.n_primitives(), 1)
            self.assertFalse(self.with_inputs6.empty())
            self.assertEqual(self.with_inputs6.size(), 1)
            self.assertFalse(self.with_inputs6.is_null())

        def test_basis_set_name(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertIsNone(self.defaulted.basis_set_name)
            self.assertFalse(self.defaulted.is_null())

            self.defaulted.basis_set_name = "new name"

            self.assertEqual(self.defaulted.basis_set_name, "new name")

        def test_atomic_number(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertIsNone(self.defaulted.atomic_number)
            self.assertFalse(self.defaulted.is_null())

            self.defaulted.atomic_number = 100

            self.assertEqual(self.defaulted.atomic_number, 100)

        def test_center(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertEqual(self.defaulted.center, self.center_type())
            self.assertFalse(self.defaulted.is_null())

            self.defaulted.center = self.center_type(100.0, 100.0, 100.0)

            self.assertEqual(
                self.defaulted.center, self.center_type(100.0, 100.0, 100.0)
            )

        def test_add_shell(self):
            self.assertTrue(self.defaulted.is_null())
            self.defaulted.add_shell(
                self.pure, 0, self.cg_type([0.0], [1.0], 2.0, 3.0, 4.0)
            )

            self.assertFalse(self.defaulted.is_null())
            self.assertEqual(
                self.defaulted.center, self.center_type(0.0, 0.0, 0.0)
            )
            self.assertEqual(self.defaulted.n_aos(), 1)
            self.assertEqual(self.defaulted.n_primitives(), 1)
            self.assertFalse(self.defaulted.empty())
            self.assertEqual(self.defaulted.size(), 1)

        def test_n_aos(self):
            self.assertEqual(self.defaulted.n_aos(), 0)
            self.assertEqual(self.with_inputs6.n_aos(), 1)

        def test_n_primitives(self):
            self.assertEqual(self.defaulted.n_primitives(), 0)
            self.assertEqual(self.with_inputs6.n_primitives(), 1)

        def test_primitive_range(self):
            self.assertEqual(self.with_inputs6.primitive_range(0), (0, 1))

        def test_primitive_to_shell(self):
            self.assertEqual(self.with_inputs6.primitive_to_shell(0), 0)

        def test_primitive(self):
            self.assertEqual(
                self.with_inputs6.primitive(0),
                self.prim_type(0.0, 1.0, 6.0, 6.1, 6.2),
            )

        def test_empty(self):
            self.assertTrue(self.defaulted.empty())
            self.assertFalse(self.with_inputs6.empty())

        def test_at(self):
            self.assertEqual(
                self.with_inputs6.at(0),
                self.shell_type(self.pure, 0, [0.0], [1.0], 6.0, 6.1, 6.2),
            )

        def test_size(self):
            self.assertEqual(self.defaulted.size(), 0)
            self.assertEqual(self.with_inputs6.size(), 1)

        def test_is_null(self):
            self.assertTrue(self.defaulted.is_null())
            self.assertFalse(self.with_inputs6.is_null())

        def test_comparisons(self):
            # Default
            other_default = abs_type()
            self.assertEqual(self.defaulted, other_default)
            self.assertFalse(self.defaulted != other_default)

            # Non-default
            other_with_inputs = abs_type(
                "name", 6, self.center_type(6.0, 6.1, 6.2), [self.shell]
            )
            self.assertEqual(self.with_inputs6, other_with_inputs)
            self.assertFalse(self.with_inputs6 != other_with_inputs)

            # Different name
            abs1 = abs_type(
                "other name", 6, self.center_type(6.0, 6.1, 6.2), [self.shell]
            )
            self.assertNotEqual(self.with_inputs6, abs1)
            self.assertTrue(self.with_inputs6 != abs1)

            # Different atomic number
            abs2 = abs_type(
                "name", 7, self.center_type(6.0, 6.1, 6.2), [self.shell]
            )
            self.assertNotEqual(self.with_inputs6, abs2)
            self.assertTrue(self.with_inputs6 != abs2)

            # Different center
            abs3 = abs_type(
                "name", 6, self.center_type(9.0, 6.1, 6.2), [self.shell]
            )
            self.assertNotEqual(self.with_inputs6, abs3)
            self.assertTrue(self.with_inputs6 != abs3)

            # Different shells
            abs4 = abs_type("name", 6, self.center_type(6.0, 6.1, 6.2))
            abs4.add_shell(self.pure, 0, self.cg_type([1.0], [1.0], 0, 0, 0))
            self.assertNotEqual(self.with_inputs6, abs4)
            self.assertTrue(self.with_inputs6 != abs4)

        def setUp(self):
            # "typedefs"
            if abs_type == AtomicBasisSetF:
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
            self.shell = self.shell_type(
                self.pure, 0, [0.0], [1.0], 0.0, 0.0, 0.0
            )

            # The instances being tested
            self.defaulted = abs_type()
            self.with_inputs0 = abs_type("name", 0, 0.0, 0.1, 0.2)
            self.with_inputs1 = abs_type(
                "name", 1, self.center_type(1.0, 1.1, 1.2)
            )
            self.with_inputs2 = abs_type(2.0, 2.1, 2.2)
            self.with_inputs3 = abs_type(self.center_type(3.0, 3.1, 3.2))
            self.with_inputs4 = abs_type("name", 4)
            self.with_inputs5 = abs_type(
                self.center_type(5.0, 5.1, 5.2), [self.shell]
            )
            self.with_inputs6 = abs_type(
                "name", 6, self.center_type(6.0, 6.1, 6.2), [self.shell]
            )

    return TestAtomicBasisSet


class TestAtomicBasisSetF(make_abs_test_case(AtomicBasisSetF)):
    pass


class TestAtomicBasisSetD(make_abs_test_case(AtomicBasisSetD)):
    pass
