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

import sys
import unittest

from chemist.experimental import (
    AOShellView,
    CartesianCCAShell,
    CartesianCCAShellView,
    ContractedGaussian,
    ImmutableCartesianAOView,
    ImmutableCartesianCCAShellView,
    ImmutableSphericalAOView,
    ImmutableSphericalCCAShellView,
    Point,
    SphericalCCAShell,
    SphericalCCAShellView,
)

CS = [2.0, 3.0]
ES = [1.0, 2.0]

# (shell type, mutable view type, read-only view type, AO view type)
KINDS = (
    (
        CartesianCCAShell,
        CartesianCCAShellView,
        ImmutableCartesianCCAShellView,
        ImmutableCartesianAOView,
    ),
    (
        SphericalCCAShell,
        SphericalCCAShellView,
        ImmutableSphericalCCAShellView,
        ImmutableSphericalAOView,
    ),
)


class TestCCAShellView(unittest.TestCase):
    """Tests both purities' views; each test loops over KINDS."""

    def kinds(self):
        """Yields a fresh d shell of each kind, plus its associated types."""
        for shell_type, view_type, const_view_type, ao_type in KINDS:
            shell = shell_type(CS, ES, 2, 1.0, 2.0, 3.0)
            yield shell, view_type, const_view_type, ao_type

    def test_there_is_no_default_ctor(self):
        for _, view_type, const_view_type, _ in self.kinds():
            with self.assertRaises(TypeError):
                view_type()
            with self.assertRaises(TypeError):
                const_view_type()

    def test_is_an_ao_shell_view(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            self.assertIsInstance(view_type(shell), AOShellView)
            self.assertIsInstance(const_view_type(shell), AOShellView)

    def test_from_a_shell_aliases_it(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            v = view_type(shell)
            cv = const_view_type(shell)
            self.assertEqual(v, shell)
            self.assertEqual(cv, shell)
            shell.set_l(3)
            self.assertEqual(v.get_l(), 3)
            self.assertEqual(cv.get_l(), 3)

    def test_from_an_aliased_contracted_gaussian(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            cg = ContractedGaussian(CS, ES, 2, 1.0, 2.0, 3.0)
            v = view_type(cg)
            cv = const_view_type(cg)
            self.assertEqual(v, shell)
            self.assertEqual(cv, shell)
            cg.set_l(3)
            self.assertEqual(v.get_l(), 3)
            self.assertEqual(cv.get_l(), 3)

    def test_mutable_to_read_only_conversion(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            self.assertEqual(const_view_type(view_type(shell)), shell)

    def test_writing_through_the_view_mutates_the_shell(self):
        for shell, view_type, _, _ in self.kinds():
            v = view_type(shell)
            v.set_l(3)
            v.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
            self.assertEqual(shell.get_l(), 3)
            self.assertEqual(shell.get_center(), Point(4.0, 5.0, 6.0))

    def test_read_only_view_can_not_be_written_through(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            cv = const_view_type(shell)
            self.assertFalse(hasattr(cv, "set_l"))
            self.assertFalse(hasattr(cv.get_contracted_gaussian(), "set_l"))
            self.assertTrue(hasattr(view_type(shell), "set_l"))

    def test_indexing_hands_out_read_only_ao_views(self):
        for shell, view_type, const_view_type, ao_type in self.kinds():
            for v in (view_type(shell), const_view_type(shell)):
                self.assertEqual(len(v), len(shell))
                self.assertIs(type(v[1]), ao_type)
                self.assertEqual(v[1], shell[1])
                with self.assertRaises(IndexError):
                    v[len(shell)]

    def test_as_cca_shell_is_a_deep_copy(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            for v in (view_type(shell), const_view_type(shell)):
                copy = v.as_cca_shell()
                self.assertIs(type(copy), type(shell))
                self.assertEqual(copy, shell)
                copy.set_l(4)
                self.assertEqual(shell.get_l(), 2)

    def test_as_shell_materializes_the_same_kind_of_shell(self):
        for shell, _, const_view_type, _ in self.kinds():
            copy = const_view_type(shell).as_shell()
            self.assertIs(type(copy), type(shell))
            self.assertEqual(copy, shell)

    def test_clone_is_a_shallow_copy_of_the_same_kind(self):
        for shell, _, const_view_type, _ in self.kinds():
            copy = const_view_type(shell).clone()
            self.assertIs(type(copy), const_view_type)
            shell.set_l(3)
            self.assertEqual(copy.get_l(), 3)

    def test_comparisons(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            v = view_type(shell)
            cv = const_view_type(shell)
            other = type(shell)(CS, ES, 1, 1.0, 2.0, 3.0)
            self.assertEqual(v, shell)
            self.assertEqual(shell, v)
            self.assertEqual(cv, shell)
            self.assertEqual(shell, cv)
            self.assertEqual(cv, v)
            self.assertNotEqual(v, other)
            self.assertNotEqual(other, cv)
            self.assertTrue(cv.are_equal(cv.clone()))

    def test_views_keep_what_they_alias_alive(self):
        for shell, view_type, const_view_type, _ in self.kinds():
            for make_view in (view_type, const_view_type, type(shell).as_view):
                before = sys.getrefcount(shell)
                v = make_view(shell)
                self.assertEqual(sys.getrefcount(shell), before + 1)

                before_view = sys.getrefcount(v)
                ao = v[0]
                self.assertEqual(sys.getrefcount(v), before_view + 1)
                del v, ao
                self.assertEqual(sys.getrefcount(shell), before)

    def test_shell_aos_keep_the_shell_alive(self):
        for shell, _, _, _ in self.kinds():
            for get in (
                lambda s: s[0],
                lambda s: s.get_center(),
                lambda s: s.get_contracted_gaussian(),
                lambda s: s.get_cartesian_shell(),
            ):
                before = sys.getrefcount(shell)
                rv = get(shell)
                self.assertEqual(sys.getrefcount(shell), before + 1)
                del rv


if __name__ == "__main__":
    unittest.main(verbosity=2)
