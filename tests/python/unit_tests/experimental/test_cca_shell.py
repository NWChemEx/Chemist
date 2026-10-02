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
    AOShell,
    CartesianAO,
    CartesianCCAShell,
    CartesianCCAShellView,
    ContractedGaussian,
    ImmutableCartesianAOView,
    ImmutableCartesianCCAShellView,
    ImmutableSphericalAOView,
    ImmutableSphericalCCAShellView,
    Point,
    SphericalAO,
    SphericalCCAShell,
    SphericalCCAShellView,
)

CS = [2.0, 3.0]
ES = [1.0, 2.0]


def make_cart(l):
    return CartesianCCAShell(CS, ES, l, 1.0, 2.0, 3.0)


def make_pure(l):
    return SphericalCCAShell(CS, ES, l, 1.0, 2.0, 3.0)


class TestCCAShell(unittest.TestCase):
    """Tests what CartesianCCAShell and SphericalCCAShell have in common."""

    def setUp(self):
        self.d = make_cart(2)
        self.pd = make_pure(2)
        self.shell_types = (
            (CartesianCCAShell, make_cart),
            (SphericalCCAShell, make_pure),
        )

    def test_default_ctor(self):
        for shell_type, _ in self.shell_types:
            with self.subTest(shell_type=shell_type):
                defaulted = shell_type()
                self.assertEqual(defaulted.get_l(), 0)
                self.assertEqual(len(defaulted), 1)
                self.assertEqual(len(defaulted.get_contracted_gaussian()), 0)

    def test_ctors(self):
        cg = ContractedGaussian(CS, ES, 2, 1.0, 2.0, 3.0)
        for shell_type, make in self.shell_types:
            with self.subTest(shell_type=shell_type):
                shell = make(2)
                self.assertEqual(
                    shell_type(CS, ES, 2, Point(1.0, 2.0, 3.0)), shell
                )
                self.assertEqual(shell_type(cg), shell)
                self.assertEqual(shell.get_contracted_gaussian(), cg)

    def test_ctor_throws_if_ranges_differ(self):
        for shell_type, _ in self.shell_types:
            with self.subTest(shell_type=shell_type):
                with self.assertRaises(ValueError):
                    shell_type(CS, [1.0], 1, 0.0, 0.0, 0.0)

    def test_is_an_ao_shell(self):
        self.assertIsInstance(self.d, AOShell)
        self.assertIsInstance(self.pd, AOShell)

    def test_get_l(self):
        self.assertEqual(self.d.get_l(), 2)
        self.assertEqual(self.pd.get_l(), 2)
        self.assertEqual(self.pd.get_contracted_gaussian().get_l(), 2)

    def test_is_pure_is_cartesian(self):
        self.assertFalse(self.d.is_pure())
        self.assertTrue(self.d.is_cartesian())
        self.assertTrue(self.pd.is_pure())
        self.assertFalse(self.pd.is_cartesian())

    def test_size(self):
        self.assertEqual([len(make_cart(l)) for l in range(4)], [1, 3, 6, 10])
        self.assertEqual([len(make_pure(l)) for l in range(4)], [1, 3, 5, 7])

    def test_get_center(self):
        self.assertEqual(self.d.get_center(), Point(1.0, 2.0, 3.0))
        self.assertEqual(self.pd.get_center(), Point(1.0, 2.0, 3.0))

    def test_cartesian_powers_follows_the_cca_order(self):
        def order_of(l):
            shell = make_cart(l)
            return [shell.cartesian_powers(a) for a in range(len(shell))]

        self.assertEqual(order_of(0), [(0, 0, 0)])
        self.assertEqual(order_of(1), [(1, 0, 0), (0, 1, 0), (0, 0, 1)])
        self.assertEqual(
            order_of(2),
            [(2, 0, 0), (1, 1, 0), (1, 0, 1), (0, 2, 0), (0, 1, 1), (0, 0, 2)],
        )

    def test_magnetic_index_runs_from_minus_l_to_l(self):
        f = make_pure(3)
        self.assertEqual(
            [f.magnetic_index(a) for a in range(len(f))], list(range(-3, 4))
        )

    def test_angular_indices_throw_for_an_out_of_range_offset(self):
        with self.assertRaises(IndexError):
            self.d.cartesian_powers(6)
        with self.assertRaises(IndexError):
            self.pd.magnetic_index(5)

    def test_each_purity_only_has_its_own_angular_index(self):
        self.assertTrue(hasattr(self.d, "cartesian_powers"))
        self.assertFalse(hasattr(self.d, "magnetic_index"))
        self.assertTrue(hasattr(self.pd, "magnetic_index"))
        self.assertFalse(hasattr(self.pd, "cartesian_powers"))

    def test_indexing_a_cartesian_shell(self):
        dxz = CartesianAO(CS, ES, 1, 0, 1, 1.0, 2.0, 3.0)
        self.assertIs(type(self.d[2]), ImmutableCartesianAOView)
        self.assertEqual(self.d.at(2), dxz)
        self.assertEqual(self.d[2], dxz)
        with self.assertRaises(IndexError):
            self.d.at(6)
        with self.assertRaises(IndexError):
            self.d[6]

    def test_indexing_a_pure_shell(self):
        d_m1 = SphericalAO(CS, ES, 2, -1, 1.0, 2.0, 3.0)
        self.assertIs(type(self.pd[1]), ImmutableSphericalAOView)
        self.assertEqual(self.pd.at(1), d_m1)
        self.assertEqual(self.pd[1], d_m1)
        self.assertEqual(self.pd[1].get_m(), -1)
        with self.assertRaises(IndexError):
            self.pd[5]

    def test_iteration(self):
        self.assertEqual(len(list(self.d)), 6)
        self.assertEqual([ao.get_m() for ao in self.pd], [-2, -1, 0, 1, 2])

    def test_the_aos_alias_the_shells_contracted_gaussian(self):
        cart_ao = self.d[1]
        pure_ao = self.pd[1]
        self.d.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.pd.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(cart_ao.get_center(), Point(4.0, 5.0, 6.0))
        self.assertEqual(pure_ao.get_center(), Point(4.0, 5.0, 6.0))

    def test_aos_still_alias_the_shell_after_set_l(self):
        # Each AO is built when the shell is indexed, and aliases the shell's
        # contracted Gaussian, so it keeps doing so after l changes.
        cart_ao = self.d[0]
        pure_ao = self.pd[0]
        self.d.set_l(3)
        self.pd.set_l(3)
        self.d.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.pd.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(cart_ao.get_center(), Point(4.0, 5.0, 6.0))
        self.assertEqual(pure_ao.get_center(), Point(4.0, 5.0, 6.0))

    def test_the_aos_follow_set_l(self):
        self.d.set_l(3)
        self.assertEqual(
            self.d[9], CartesianAO(CS, ES, 0, 0, 3, 1.0, 2.0, 3.0)
        )

        self.pd.set_l(3)
        self.assertEqual(self.pd[6].get_m(), 3)
        self.assertEqual(self.pd[6].get_l(), 3)

    def test_the_aos_follow_l_changed_through_the_contracted_gaussian(self):
        self.d.get_contracted_gaussian().set_l(1)
        self.assertEqual(len(self.d), 3)
        self.assertEqual(self.d[2].get_k(), 1)

    def test_get_cartesian_shell(self):
        # For a pure shell it is the Cartesian shell underneath ...
        cart = self.pd.get_cartesian_shell()
        self.assertIs(type(cart), ImmutableCartesianCCAShellView)
        self.assertTrue(cart.is_cartesian())
        self.assertEqual(len(cart), 6)
        self.assertEqual(cart, self.d)

        # ... which aliases the pure shell's contracted Gaussian.
        self.pd.get_contracted_gaussian().set_center(Point(7.0, 7.0, 7.0))
        self.assertEqual(cart.get_center(), Point(7.0, 7.0, 7.0))

        # For a Cartesian shell it is the shell itself.
        self.assertEqual(self.d.get_cartesian_shell(), self.d)

    def test_normalization_constant(self):
        self.assertEqual(
            self.d.normalization_constant(),
            self.d.get_contracted_gaussian().normalization_constant(),
        )
        self.assertEqual(
            self.pd.normalization_constant(), self.d.normalization_constant()
        )

    def test_set_l(self):
        self.d.set_l(3)
        self.assertEqual(self.d.get_l(), 3)
        self.assertEqual(len(self.d), 10)

        self.pd.set_l(3)
        self.assertEqual(self.pd.get_l(), 3)
        self.assertEqual(len(self.pd), 7)
        self.assertEqual(self.pd.get_contracted_gaussian().get_l(), 3)

    def test_comparisons(self):
        self.assertEqual(self.d, make_cart(2))
        self.assertFalse(self.d != make_cart(2))
        self.assertNotEqual(self.d, make_cart(1))
        self.assertEqual(self.pd, make_pure(2))
        self.assertNotEqual(self.pd, make_pure(1))

        # Same contracted Gaussian, different purity: not the same shell
        self.assertNotEqual(self.d, self.pd)
        self.assertNotEqual(self.pd, self.d)

    def test_clone_is_a_deep_copy_of_the_same_kind(self):
        for shell in (self.d, self.pd):
            with self.subTest(shell=type(shell)):
                copy = shell.clone()
                self.assertIs(type(copy), type(shell))
                self.assertEqual(copy, shell)
                copy.set_l(4)
                self.assertEqual(shell.get_l(), 2)

    def test_as_view_is_a_shallow_view_of_the_same_kind(self):
        for shell, view_type in (
            (self.d, ImmutableCartesianCCAShellView),
            (self.pd, ImmutableSphericalCCAShellView),
        ):
            with self.subTest(shell=type(shell)):
                view = shell.as_view()
                self.assertIs(type(view), view_type)
                shell.set_l(3)
                self.assertEqual(view.get_l(), 3)

    def test_are_equal(self):
        self.assertTrue(self.d.are_equal(make_cart(2)))
        self.assertFalse(self.d.are_different(make_cart(2)))
        self.assertFalse(self.d.are_equal(self.pd))
        self.assertTrue(self.d.are_different(self.pd))

    def test_mutable_views_convert_implicitly(self):
        self.assertEqual(CartesianCCAShellView(self.d), self.d)
        self.assertEqual(SphericalCCAShellView(self.pd), self.pd)


if __name__ == "__main__":
    unittest.main(verbosity=2)
