# Copyright 2026 NWChemEx-Project
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

from chemist.experimental import (
    AOOrdering,
    AtomicBasisSet,
    CartesianCCAShell,
    ImmutableCartesianCCAShellView,
    ImmutableSphericalCCAShellView,
    Point,
    PointView,
    PrimitiveView,
    ShellPurity,
    SphericalCCAShell,
)

S_CS, S_ES = [0.5, 0.25], [3.0, 0.5]
P_CS, P_ES = [1.0], [0.8]

# (purity, shell type, the type of view indexing hands out)
KINDS = (
    (ShellPurity.cartesian, CartesianCCAShell, ImmutableCartesianCCAShellView),
    (ShellPurity.pure, SphericalCCAShell, ImmutableSphericalCCAShellView),
)


def make_set(purity):
    basis = AtomicBasisSet("cc-pVDZ", 8, Point(1.0, 2.0, 3.0), purity)
    basis.add_shell(0, S_CS, S_ES)
    basis.add_shell(1, P_CS, P_ES)
    return basis


class TestAtomicBasisSet(unittest.TestCase):
    def setUp(self):
        self.r0 = Point(1.0, 2.0, 3.0)

    def test_default_ctor(self):
        defaulted = AtomicBasisSet()
        self.assertTrue(defaulted.empty())
        self.assertEqual(len(defaulted), 0)
        self.assertEqual(defaulted.get_name(), "")
        self.assertEqual(defaulted.get_atomic_number(), 0)
        self.assertEqual(defaulted.get_center(), Point(0.0, 0.0, 0.0))
        self.assertEqual(defaulted.purity(), ShellPurity.cartesian)
        self.assertEqual(defaulted.ordering(), AOOrdering.cca)

    def test_value_ctor(self):
        basis = AtomicBasisSet("cc-pVDZ", 8, self.r0)
        self.assertEqual(basis.get_name(), "cc-pVDZ")
        self.assertEqual(basis.get_atomic_number(), 8)
        self.assertEqual(basis.get_center(), self.r0)
        self.assertEqual(basis.purity(), ShellPurity.cartesian)

        pure = AtomicBasisSet("cc-pVDZ", 8, self.r0, ShellPurity.pure)
        self.assertTrue(pure.is_pure())
        self.assertFalse(pure.is_cartesian())

    def test_shells(self):
        for purity, shell_type, view_type in KINDS:
            with self.subTest(purity=purity):
                basis = make_set(purity)
                s = shell_type(S_CS, S_ES, 0, self.r0)
                p = shell_type(P_CS, P_ES, 1, self.r0)
                self.assertEqual(len(basis), 2)
                self.assertIs(type(basis[0]), view_type)
                self.assertEqual(basis.at(0), s)
                self.assertEqual(basis[1], p)
                self.assertEqual(basis.get_l(1), 1)
                self.assertEqual(len(list(basis)), 2)
                with self.assertRaises(IndexError):
                    basis.at(2)

    def test_n_aos(self):
        self.assertEqual(make_set(ShellPurity.cartesian).n_aos(), 1 + 3)
        self.assertEqual(make_set(ShellPurity.pure).n_aos(), 1 + 3)

    def test_primitives(self):
        basis = make_set(ShellPurity.cartesian)
        self.assertEqual(basis.n_primitives(), 3)
        self.assertEqual(basis.primitive_range(0), (0, 2))
        self.assertEqual(basis.primitive_range(1), (2, 3))
        self.assertEqual(
            [basis.primitive_to_shell(i) for i in range(3)], [0, 0, 1]
        )
        self.assertIs(type(basis.primitive(2)), PrimitiveView)
        self.assertEqual(basis.primitive(2).get_coefficient(), 1.0)
        self.assertEqual(basis.primitive(2).get_exponent(), 0.8)
        self.assertEqual(basis.get_coefficients(), S_CS + P_CS)
        self.assertEqual(basis.get_exponents(), S_ES + P_ES)

    def test_the_shells_and_primitives_alias_the_set(self):
        basis = make_set(ShellPurity.cartesian)
        shell = basis[1]
        basis.primitive(2).set_exponent(9.0)
        self.assertEqual(
            shell.get_contracted_gaussian()[0].get_exponent(), 9.0
        )

        basis.set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(shell.get_center(), Point(4.0, 5.0, 6.0))
        self.assertEqual(basis.primitive(0).get_center(), Point(4.0, 5.0, 6.0))

    def test_setters(self):
        basis = make_set(ShellPurity.cartesian)
        basis.set_name("STO-3G")
        basis.set_atomic_number(1)
        basis.set_l(1, 2)
        self.assertEqual(basis.get_name(), "STO-3G")
        self.assertEqual(basis.get_atomic_number(), 1)
        self.assertEqual(basis.get_l(1), 2)
        self.assertIs(type(basis.get_center()), PointView)

    def test_push_back(self):
        for purity, shell_type, _ in KINDS:
            with self.subTest(purity=purity):
                basis = AtomicBasisSet("cc-pVDZ", 8, self.r0, purity)
                basis.push_back(shell_type(S_CS, S_ES, 0, self.r0))
                basis.push_back(make_set(purity)[1])
                self.assertEqual(basis, make_set(purity))

                moved = shell_type(S_CS, S_ES, 0, Point(0.0, 0.0, 0.0))
                with self.assertRaises(ValueError):
                    basis.push_back(moved)

    def test_push_back_wrong_purity(self):
        basis = AtomicBasisSet("cc-pVDZ", 8, self.r0, ShellPurity.pure)
        with self.assertRaises(ValueError):
            basis.push_back(CartesianCCAShell(S_CS, S_ES, 0, self.r0))

    def test_add_shell_mismatched_lengths(self):
        basis = make_set(ShellPurity.cartesian)
        with self.assertRaises(ValueError):
            basis.add_shell(2, S_CS, P_ES)
        self.assertEqual(len(basis), 2)

    def test_comparisons(self):
        basis = make_set(ShellPurity.cartesian)
        self.assertEqual(basis, make_set(ShellPurity.cartesian))
        self.assertNotEqual(basis, make_set(ShellPurity.pure))
        other = make_set(ShellPurity.cartesian)
        other.set_name("STO-3G")
        self.assertNotEqual(basis, other)

    def test_swap(self):
        basis = make_set(ShellPurity.cartesian)
        other = AtomicBasisSet()
        basis.swap(other)
        self.assertEqual(other, make_set(ShellPurity.cartesian))
        self.assertEqual(basis, AtomicBasisSet())


if __name__ == "__main__":
    unittest.main()
