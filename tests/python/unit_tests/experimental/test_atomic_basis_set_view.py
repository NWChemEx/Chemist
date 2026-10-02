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
    AtomicBasisSet,
    AtomicBasisSetView,
    ImmutableAtomicBasisSetView,
    ImmutablePrimitiveView,
    Point,
    ShellPurity,
)

S_CS, S_ES = [0.5, 0.25], [3.0, 0.5]
P_CS, P_ES = [1.0], [0.8]


def make_set(purity=ShellPurity.cartesian):
    basis = AtomicBasisSet("cc-pVDZ", 8, Point(1.0, 2.0, 3.0), purity)
    basis.add_shell(0, S_CS, S_ES)
    basis.add_shell(1, P_CS, P_ES)
    return basis


class TestAtomicBasisSetView(unittest.TestCase):
    def setUp(self):
        self.basis = make_set()
        self.v = AtomicBasisSetView(self.basis)
        self.cv = ImmutableAtomicBasisSetView(self.basis)

    def test_ctors(self):
        self.assertEqual(self.v, self.basis)
        self.assertEqual(self.cv, self.basis)
        self.assertEqual(ImmutableAtomicBasisSetView(self.v), self.basis)

    def test_the_views_alias_the_set(self):
        self.basis.set_name("STO-3G")
        self.assertEqual(self.cv.get_name(), "STO-3G")

        self.v.set_atomic_number(1)
        self.v.set_center(Point(4.0, 5.0, 6.0))
        self.v.primitive(0).set_coefficient(42.0)
        self.assertEqual(self.basis.get_atomic_number(), 1)
        self.assertEqual(self.basis.get_center(), Point(4.0, 5.0, 6.0))
        self.assertEqual(self.basis.primitive(0).get_coefficient(), 42.0)

    def test_shells(self):
        self.assertEqual(len(self.cv), 2)
        self.assertEqual(self.cv[1], self.basis[1])
        self.assertEqual(self.cv.get_l(1), 1)
        self.assertEqual(self.cv.n_aos(), 4)
        self.assertEqual(self.cv.n_primitives(), 3)

    def test_read_only_view_has_no_setters(self):
        self.assertFalse(hasattr(self.cv, "set_name"))
        self.assertIs(type(self.cv.primitive(0)), ImmutablePrimitiveView)

    def test_as_atomic_basis_set(self):
        copy = self.cv.as_atomic_basis_set()
        self.assertIs(type(copy), AtomicBasisSet)
        self.assertEqual(copy, self.basis)
        copy.set_name("STO-3G")
        self.assertEqual(self.basis.get_name(), "cc-pVDZ")

    def test_swap(self):
        other = make_set(ShellPurity.pure)
        ov = AtomicBasisSetView(other)
        self.v.swap(ov)
        self.assertEqual(self.v, other)
        self.assertEqual(ov, self.basis)

    def test_comparisons(self):
        self.assertEqual(self.v, self.cv)
        self.assertNotEqual(self.cv, make_set(ShellPurity.pure))


if __name__ == "__main__":
    unittest.main()
