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
    ImmutableAtomicBasisSetView,
    ImmutableMolecularBasisSetView,
    ImmutablePrimitiveView,
    MolecularBasisSet,
    MolecularBasisSetView,
    Point,
    ShellPurity,
)

S_CS, S_ES = [0.5, 0.25], [3.0, 0.5]
P_CS, P_ES = [1.0], [0.8]


def make_set(o_purity=ShellPurity.cartesian):
    o = AtomicBasisSet("cc-pVDZ", 8, Point(0.0, 0.0, 0.0), o_purity)
    o.add_shell(0, S_CS, S_ES)
    o.add_shell(1, P_CS, P_ES)
    h = AtomicBasisSet("STO-3G", 1, Point(0.0, 0.0, 1.8), ShellPurity.pure)
    h.add_shell(0, S_CS, S_ES)
    return MolecularBasisSet([o, h])


class TestMolecularBasisSetView(unittest.TestCase):
    def setUp(self):
        self.basis = make_set()
        self.v = MolecularBasisSetView(self.basis)
        self.cv = ImmutableMolecularBasisSetView(self.basis)

    def test_ctors(self):
        self.assertEqual(self.v, self.basis)
        self.assertEqual(self.cv, self.basis)
        self.assertEqual(ImmutableMolecularBasisSetView(self.v), self.basis)

    def test_the_views_alias_the_set(self):
        self.basis[0].set_name("6-31G")
        self.assertEqual(self.cv[0].get_name(), "6-31G")

        self.v.set_l(1, 2)
        self.v[1].set_atomic_number(2)
        self.v.primitive(0).set_coefficient(42.0)
        self.assertEqual(self.basis.get_l(1), 2)
        self.assertEqual(self.basis[1].get_atomic_number(), 2)
        self.assertEqual(self.basis.primitive(0).get_coefficient(), 42.0)

    def test_atoms(self):
        self.assertEqual(len(self.cv), 2)
        self.assertIs(type(self.cv[0]), ImmutableAtomicBasisSetView)
        self.assertEqual(self.cv[1], self.basis[1])
        self.assertEqual(self.cv.n_shells(), 3)
        self.assertEqual(self.cv.n_aos(), 1 + 3 + 1)
        self.assertEqual(self.cv.n_primitives(), 5)

    def test_read_only_view_has_no_setters(self):
        self.assertFalse(hasattr(self.cv, "set_l"))
        self.assertFalse(hasattr(self.cv[0], "set_name"))
        self.assertIs(type(self.cv.primitive(0)), ImmutablePrimitiveView)

    def test_as_molecular_basis_set(self):
        copy = self.cv.as_molecular_basis_set()
        self.assertIs(type(copy), MolecularBasisSet)
        self.assertEqual(copy, self.basis)
        copy[0].set_name("STO-3G")
        self.assertEqual(self.basis[0].get_name(), "cc-pVDZ")

    def test_swap(self):
        other = make_set(ShellPurity.pure)
        ov = MolecularBasisSetView(other)
        self.v.swap(ov)
        self.assertEqual(self.v, other)
        self.assertEqual(ov, self.basis)

    def test_comparisons(self):
        self.assertEqual(self.v, self.cv)
        self.assertNotEqual(self.cv, make_set(ShellPurity.pure))


if __name__ == "__main__":
    unittest.main()
