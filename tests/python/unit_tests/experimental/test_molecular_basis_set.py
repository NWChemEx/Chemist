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
    AtomicBasisSetView,
    ImmutableSphericalCCAShellView,
    MolecularBasisSet,
    Point,
    PointSetView,
    PrimitiveView,
    ShellPurity,
)

S_CS, S_ES = [0.5, 0.25], [3.0, 0.5]
P_CS, P_ES = [1.0], [0.8]
R_O, R_H = Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 1.8)


def make_o():
    o = AtomicBasisSet("cc-pVDZ", 8, R_O, ShellPurity.cartesian)
    o.add_shell(0, S_CS, S_ES)
    o.add_shell(1, P_CS, P_ES)
    return o


def make_h():
    h = AtomicBasisSet("STO-3G", 1, R_H, ShellPurity.pure)
    h.add_shell(0, S_CS, S_ES)
    return h


def make_set():
    return MolecularBasisSet([make_o(), make_h()])


class TestMolecularBasisSet(unittest.TestCase):
    def test_default_ctor(self):
        defaulted = MolecularBasisSet()
        self.assertTrue(defaulted.empty())
        self.assertEqual(len(defaulted), 0)
        self.assertEqual(defaulted.n_shells(), 0)
        self.assertEqual(defaulted.n_aos(), 0)

    def test_list_ctor_and_push_back(self):
        built = MolecularBasisSet()
        built.push_back(make_o())
        built.push_back(AtomicBasisSetView(make_h()))
        self.assertEqual(built, make_set())

    def test_push_back_copies(self):
        o = make_o()
        basis = MolecularBasisSet([o])
        o.set_name("changed")
        self.assertEqual(basis[0].get_name(), "cc-pVDZ")

        # Including an atom of the set itself
        basis.push_back(basis[0])
        self.assertEqual(len(basis), 2)
        self.assertEqual(basis[1], make_o())

    def test_atoms(self):
        basis = make_set()
        self.assertEqual(len(basis), 2)
        self.assertIs(type(basis[0]), AtomicBasisSetView)
        self.assertEqual(basis.at(0), make_o())
        self.assertEqual(basis[1], make_h())
        self.assertEqual(list(basis), [make_o(), make_h()])
        with self.assertRaises(IndexError):
            basis.at(2)

    def test_the_atoms_alias_the_set(self):
        basis = make_set()
        h = basis[1]
        basis.primitive(4).set_exponent(9.0)
        self.assertEqual(h.primitive(1).get_exponent(), 9.0)

        h.set_name("6-31G")
        h.set_center(Point(1.0, 2.0, 3.0))
        self.assertEqual(basis[1].get_name(), "6-31G")
        self.assertEqual(basis.get_centers()[1], Point(1.0, 2.0, 3.0))
        self.assertEqual(basis[0], make_o())

    def test_centers(self):
        basis = make_set()
        centers = basis.get_centers()
        self.assertIs(type(centers), PointSetView)
        self.assertEqual(len(centers), 2)
        self.assertEqual(centers[0], R_O)
        self.assertEqual(centers[1], R_H)

    def test_shell_types(self):
        basis = make_set()
        self.assertEqual(basis.purity(0), ShellPurity.cartesian)
        self.assertEqual(basis.purity(1), ShellPurity.pure)
        self.assertEqual(basis.ordering(1), AOOrdering.cca)
        self.assertFalse(basis.has_uniform_purity())
        self.assertTrue(basis.has_uniform_ordering())
        self.assertFalse(basis.is_pure())
        self.assertFalse(basis.is_cartesian())
        self.assertTrue(MolecularBasisSet([make_h()]).is_pure())

    def test_shells(self):
        basis = make_set()
        self.assertEqual(basis.n_shells(), 3)
        self.assertEqual(basis.shell_range(0), (0, 2))
        self.assertEqual(basis.shell_range(1), (2, 3))
        self.assertEqual([basis.shell_to_atom(s) for s in range(3)], [0, 0, 1])
        self.assertEqual([basis.get_l(s) for s in range(3)], [0, 1, 0])
        self.assertIs(type(basis.shell(2)), ImmutableSphericalCCAShellView)
        self.assertEqual(basis.shell(2), make_h()[0])
        self.assertEqual(basis.n_aos(), 1 + 3 + 1)

        basis.set_l(2, 2)
        self.assertEqual(basis[1].get_l(0), 2)
        self.assertEqual(basis.n_aos(), 1 + 3 + 5)

    def test_primitives(self):
        basis = make_set()
        self.assertEqual(basis.n_primitives(), 5)
        self.assertEqual(basis.primitive_range(2), (3, 5))
        self.assertEqual(
            [basis.primitive_to_shell(p) for p in range(5)], [0, 0, 1, 2, 2]
        )
        self.assertIs(type(basis.primitive(2)), PrimitiveView)
        self.assertEqual(basis.primitive(2).get_coefficient(), 1.0)
        self.assertEqual(basis.get_coefficients(), S_CS + P_CS + S_CS)
        self.assertEqual(basis.get_exponents(), S_ES + P_ES + S_ES)

    def test_comparisons(self):
        basis = make_set()
        self.assertEqual(basis, make_set())
        self.assertNotEqual(basis, MolecularBasisSet([make_h(), make_o()]))
        other = make_set()
        other[0].set_name("STO-3G")
        self.assertNotEqual(basis, other)

    def test_swap(self):
        basis = make_set()
        other = MolecularBasisSet()
        basis.swap(other)
        self.assertEqual(other, make_set())
        self.assertEqual(basis, MolecularBasisSet())


if __name__ == "__main__":
    unittest.main()
