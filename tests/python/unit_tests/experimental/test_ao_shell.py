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
    AOShellBase,
    AOShellBaseView,
    CartesianAOShell,
    CartesianAOShellView,
    CartesianCCAShell,
    ImmutableCartesianAOView,
    ImmutableCartesianCCAShellView,
    ImmutableSphericalAOView,
    SphericalAOShell,
    SphericalAOShellView,
    SphericalCCAShell,
    as_cartesian_shell,
    as_spherical_shell,
)

CS = [2.0, 3.0]
ES = [1.0, 2.0]


class TestAOShell(unittest.TestCase):
    """Drives AOShell<T> and AOShellView<T> through their concrete classes.

    The bases are called unbound, as in test_ao_shell_base.py, so that what
    runs is the binding of the base rather than of the concrete class.
    """

    def setUp(self):
        self.d = CartesianCCAShell(CS, ES, 2, 1.0, 2.0, 3.0)
        self.pd = SphericalCCAShell(CS, ES, 2, 1.0, 2.0, 3.0)

    def kinds(self):
        """Each shell, its view, and the typed bases they should satisfy."""
        return (
            (self.d, CartesianAOShell, CartesianAOShellView),
            (self.pd, SphericalAOShell, SphericalAOShellView),
        )

    def test_shells_can_not_be_made_directly(self):
        for shell_type in (
            CartesianAOShell,
            SphericalAOShell,
            CartesianAOShellView,
            SphericalAOShellView,
        ):
            with self.assertRaises(TypeError):
                shell_type()

    def test_typed_bases_derive_from_the_untyped_ones(self):
        for shell, shell_base, view_base in self.kinds():
            self.assertTrue(issubclass(shell_base, AOShellBase))
            self.assertTrue(issubclass(view_base, AOShellBaseView))
            self.assertIsInstance(shell, shell_base)
            self.assertIsInstance(shell.as_view(), view_base)

    def test_angular_index_exists_only_for_the_purity(self):
        self.assertTrue(hasattr(CartesianAOShell, "cartesian_powers"))
        self.assertFalse(hasattr(CartesianAOShell, "magnetic_index"))
        self.assertTrue(hasattr(SphericalAOShell, "magnetic_index"))
        self.assertFalse(hasattr(SphericalAOShell, "cartesian_powers"))
        self.assertTrue(hasattr(CartesianAOShellView, "cartesian_powers"))
        self.assertFalse(hasattr(SphericalAOShellView, "cartesian_powers"))

    def test_angular_index_follows_the_shell_ordering(self):
        for s, base in (
            (self.d, CartesianAOShell),
            (self.d.as_view(), CartesianAOShellView),
        ):
            with self.subTest(shell=type(s)):
                for i in range(len(s)):
                    self.assertEqual(
                        base.cartesian_powers(s, i), self.d.cartesian_powers(i)
                    )
                with self.assertRaises(IndexError):
                    base.cartesian_powers(s, len(s))
        for s, base in (
            (self.pd, SphericalAOShell),
            (self.pd.as_view(), SphericalAOShellView),
        ):
            with self.subTest(shell=type(s)):
                for i in range(len(s)):
                    self.assertEqual(
                        base.magnetic_index(s, i), self.pd.magnetic_index(i)
                    )
                with self.assertRaises(IndexError):
                    base.magnetic_index(s, len(s))

    def test_indexing_hands_out_the_right_ao_views(self):
        for shell, ao_type, base in (
            (self.d, ImmutableCartesianAOView, CartesianAOShell),
            (self.pd, ImmutableSphericalAOView, SphericalAOShell),
        ):
            with self.subTest(shell=type(shell)):
                for i in range(len(shell)):
                    ao = base.at(shell, i)
                    self.assertIs(type(ao), ao_type)
                    self.assertEqual(ao, shell[i])
                    self.assertEqual(base.__getitem__(shell, i), shell[i])

    def test_get_cartesian_shell(self):
        for shell, base in (
            (self.d, CartesianAOShell),
            (self.pd, SphericalAOShell),
        ):
            with self.subTest(shell=type(shell)):
                cart = base.get_cartesian_shell(shell)
                self.assertIs(type(cart), ImmutableCartesianCCAShellView)
                self.assertEqual(
                    cart.get_contracted_gaussian(),
                    shell.get_contracted_gaussian(),
                )

    def test_clone_as_view_as_shell_keep_the_type(self):
        for shell, shell_base, view_base in self.kinds():
            with self.subTest(shell=type(shell)):
                copy = shell_base.clone(shell)
                self.assertIs(type(copy), type(shell))
                view = shell_base.as_view(shell)
                self.assertIsInstance(view, view_base)
                self.assertIsInstance(view_base.clone(view), view_base)
                self.assertIs(type(view_base.as_shell(view)), type(shell))

    def test_as_cartesian_shell(self):
        for s in (self.d, self.d.as_view()):
            with self.subTest(shell=type(s)):
                self.assertIs(as_cartesian_shell(s), s)
        for s in (self.pd, self.pd.as_view()):
            with self.subTest(shell=type(s)):
                with self.assertRaises(ValueError):
                    as_cartesian_shell(s)

    def test_as_spherical_shell(self):
        for s in (self.pd, self.pd.as_view()):
            with self.subTest(shell=type(s)):
                self.assertIs(as_spherical_shell(s), s)
        for s in (self.d, self.d.as_view()):
            with self.subTest(shell=type(s)):
                with self.assertRaises(ValueError):
                    as_spherical_shell(s)


if __name__ == "__main__":
    unittest.main(verbosity=2)
