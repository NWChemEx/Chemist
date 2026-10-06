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
    AOView,
    CartesianCCAShell,
    ImmutableCartesianCCAShellView,
    ImmutableSphericalAOView,
    ImmutableSphericalCCAShellView,
    Point,
    SphericalAO,
    SphericalCCAShell,
)

CS = [2.0, 3.0]
ES = [1.0, 2.0]


class TestImmutableSphericalAOView(unittest.TestCase):
    def setUp(self):
        self.r = Point(0.3, -0.4, 0.5)
        self.shell = CartesianCCAShell(CS, ES, 2, 1.0, 2.0, 3.0)
        self.d1 = SphericalAO(self.shell, 1)

    def test_there_is_no_default_ctor(self):
        with self.assertRaises(TypeError):
            ImmutableSphericalAOView()

    def test_is_an_ao_view(self):
        self.assertIsInstance(ImmutableSphericalAOView(self.d1), AOView)

    def test_from_a_spherical_ao(self):
        v = ImmutableSphericalAOView(self.d1)
        self.assertEqual(v, self.d1)
        self.assertEqual(v.get_m(), 1)
        self.assertEqual(v.get_l(), 2)

    def test_from_a_shell_aliases_the_shell(self):
        v = ImmutableSphericalAOView(self.shell, 1)
        self.assertEqual(v, self.d1)

        self.shell.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(v.get_center(), Point(4.0, 5.0, 6.0))

        # ... and so the value tracks edits to the shell.
        edited = SphericalAO(self.shell, 1)
        self.assertEqual(
            v.normalized_evaluate(self.r), edited.normalized_evaluate(self.r)
        )

    def test_from_a_shell_view_aliases_what_it_aliases(self):
        shell_view = ImmutableCartesianCCAShellView(self.shell)
        v = ImmutableSphericalAOView(shell_view, 1)
        self.assertEqual(v, self.d1)

        self.shell.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(v.get_center(), Point(4.0, 5.0, 6.0))

    def test_from_a_shell_rejects_a_pure_shell(self):
        # The C++ ctors only accept a Cartesian shell, so a pure one matches
        # no overload.
        pure = SphericalCCAShell(CS, ES, 2, 1.0, 2.0, 3.0)
        with self.assertRaises(TypeError):
            ImmutableSphericalAOView(pure, 1)
        with self.assertRaises(TypeError):
            ImmutableSphericalAOView(ImmutableSphericalCCAShellView(pure), 1)

    def test_from_a_shell_throws_if_m_is_too_big(self):
        with self.assertRaises(ValueError):
            ImmutableSphericalAOView(self.shell, 3)
        shell_view = ImmutableCartesianCCAShellView(self.shell)
        with self.assertRaises(ValueError):
            ImmutableSphericalAOView(shell_view, -3)

    def test_is_read_only(self):
        v = ImmutableSphericalAOView(self.shell, 1)
        self.assertFalse(hasattr(v, "set_m"))
        self.assertFalse(hasattr(v.get_cartesian_shell(), "set_l"))
        self.assertFalse(hasattr(v.get_contracted_gaussian(), "set_l"))

    def test_get_cartesian_shell_aliases_the_shell(self):
        v = ImmutableSphericalAOView(self.shell, 1)
        shell = v.get_cartesian_shell()
        self.assertIs(type(shell), ImmutableCartesianCCAShellView)
        self.assertEqual(shell, self.shell)
        self.shell.set_l(3)
        self.assertEqual(shell.get_l(), 3)

    def test_clone_is_a_shallow_copy(self):
        v = ImmutableSphericalAOView(self.shell, 1)
        copy = v.clone()
        self.assertIs(type(copy), ImmutableSphericalAOView)
        self.assertEqual(copy, v)
        self.shell.get_contracted_gaussian().set_center(Point(8.0, 8.0, 8.0))
        self.assertEqual(copy.get_center(), Point(8.0, 8.0, 8.0))

    def test_as_spherical_ao(self):
        v = ImmutableSphericalAOView(self.shell, 1)
        for materialized in (v.as_spherical_ao(), v.as_ao()):
            self.assertIs(type(materialized), SphericalAO)
            self.assertEqual(materialized, self.d1)

        # The result owns its state, so it does not observe later writes.
        materialized = v.as_spherical_ao()
        self.shell.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0))
        self.assertEqual(materialized.get_center(), Point(1.0, 2.0, 3.0))

    def test_comparisons(self):
        v = ImmutableSphericalAOView(self.d1)
        self.assertEqual(v, self.d1)
        self.assertEqual(self.d1, v)
        self.assertNotEqual(v, SphericalAO(self.shell, 0))
        self.assertNotEqual(SphericalAO(self.shell, 0), v)


if __name__ == "__main__":
    unittest.main(verbosity=2)
