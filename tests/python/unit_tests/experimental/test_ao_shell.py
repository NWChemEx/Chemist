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
    AOShellView,
    CartesianCCAShell,
    ImmutableCartesianAOView,
    ImmutableCartesianCCAShellView,
    ImmutableSphericalAOView,
    ImmutableSphericalCCAShellView,
    Point,
    SphericalCCAShell,
)

CS = [2.0, 3.0]
ES = [1.0, 2.0]


class TestAOShell(unittest.TestCase):
    """Drives AOShell and AOShellView through their concrete derived classes.

    See test_ao.py for why the abstract bases are tested this way.
    """

    def setUp(self):
        self.d = CartesianCCAShell(CS, ES, 2, 1.0, 2.0, 3.0)
        self.pd = SphericalCCAShell(CS, ES, 2, 1.0, 2.0, 3.0)

    def test_shells_can_not_be_made_directly(self):
        with self.assertRaises(TypeError):
            AOShell()
        with self.assertRaises(TypeError):
            AOShellView()

    def test_the_shared_api_agrees_with_the_derived_class(self):
        for shell in (self.d, self.pd, self.d.as_view(), self.pd.as_view()):
            with self.subTest(shell=type(shell)):
                base = (
                    AOShellView if isinstance(shell, AOShellView) else AOShell
                )
                self.assertEqual(base.get_l(shell), 2)
                self.assertEqual(base.is_pure(shell), shell.is_pure())
                self.assertEqual(
                    base.is_cartesian(shell), shell.is_cartesian()
                )
                self.assertEqual(base.__len__(shell), len(shell))
                self.assertEqual(base.get_center(shell), Point(1.0, 2.0, 3.0))
                self.assertEqual(
                    base.get_contracted_gaussian(shell),
                    shell.get_contracted_gaussian(),
                )
                self.assertEqual(
                    base.normalization_constant(shell),
                    shell.normalization_constant(),
                )

    def test_indexing_through_the_base_hands_out_the_right_ao_views(self):
        for shell, ao_type in (
            (self.d, ImmutableCartesianAOView),
            (self.pd, ImmutableSphericalAOView),
        ):
            for s in (shell, shell.as_view()):
                base = AOShellView if isinstance(s, AOShellView) else AOShell
                with self.subTest(shell=type(s)):
                    for i in range(len(s)):
                        ao = base.__getitem__(s, i)
                        self.assertIs(type(ao), ao_type)
                        self.assertEqual(ao, shell[i])
                        self.assertEqual(base.at(s, i), shell[i])
                    with self.assertRaises(IndexError):
                        base.at(s, len(s))

    def test_the_aos_from_the_base_alias_the_shell(self):
        ao = AOShell.at(self.d, 1)
        self.d.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(ao.get_center(), Point(4.0, 5.0, 6.0))

    def test_clone_is_a_deep_copy_of_the_same_kind(self):
        for shell in (self.d, self.pd):
            copy = AOShell.clone(shell)
            self.assertIs(type(copy), type(shell))
            copy.set_l(3)
            self.assertEqual(shell.get_l(), 2)

    def test_as_view_is_a_shallow_view_of_the_same_kind(self):
        for shell, view_type in (
            (self.d, ImmutableCartesianCCAShellView),
            (self.pd, ImmutableSphericalCCAShellView),
        ):
            view = shell.as_view()
            self.assertIs(type(view), view_type)
            self.assertIs(type(view.clone()), view_type)
            self.assertIs(type(view.as_shell()), type(shell))
            shell.set_l(3)
            self.assertEqual(view.get_l(), 3)

    def test_are_equal(self):
        self.assertTrue(self.d.are_equal(self.d.clone()))
        self.assertFalse(self.d.are_equal(self.pd))
        self.assertTrue(self.pd.are_different(self.d))

        self.assertTrue(self.d.as_view().are_equal(self.d.as_view()))
        self.assertFalse(self.d.as_view().are_equal(self.pd.as_view()))


if __name__ == "__main__":
    unittest.main(verbosity=2)
