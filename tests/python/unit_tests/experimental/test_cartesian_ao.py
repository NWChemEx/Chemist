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

import math
import unittest

from chemist.experimental import (
    AO,
    CartesianAO,
    ImmutablePointView,
    Point,
    PointSet,
    PointView,
)


class TestCartesianAO(unittest.TestCase):
    def setUp(self):
        self.origin = Point(0.0, 0.0, 0.0)
        self.r = Point(0.3, -0.4, 0.5)

        # One-primitive d_xy function: i = 1, j = 1, k = 0, so l = 2.
        self.dxy = CartesianAO([1.0], [1.2], 1, 1, 0, 0.0, 0.0, 0.0)

        # Two-primitive d_zz function, for when a real contraction is needed.
        self.dzz = CartesianAO([1.0, 1.0], [1.0, 2.0], 0, 0, 2, 0.0, 0.0, 0.0)

    def test_default_ctor(self):
        defaulted = CartesianAO()
        self.assertEqual(defaulted.get_i(), 0)
        self.assertEqual(defaulted.get_j(), 0)
        self.assertEqual(defaulted.get_k(), 0)
        self.assertEqual(defaulted.get_l(), 0)
        self.assertEqual(len(defaulted.get_contracted_gaussian()), 0)
        self.assertEqual(defaulted.get_center(), self.origin)

        # The zero function: no primitives to sum.
        self.assertEqual(defaulted.evaluate(Point(1.0, 1.0, 1.0)), 0.0)

    def test_point_ctor(self):
        same = CartesianAO([1.0], [1.2], 1, 1, 0, self.origin)
        self.assertEqual(same, self.dxy)

    def test_ctor_throws_if_ranges_differ(self):
        with self.assertRaises(ValueError):
            CartesianAO([1.0, 2.0], [1.0], 1, 1, 1, 0.0, 0.0, 0.0)

    def test_is_an_ao(self):
        self.assertIsInstance(self.dxy, AO)

    def test_powers(self):
        self.assertEqual(self.dxy.get_i(), 1)
        self.assertEqual(self.dxy.get_j(), 1)
        self.assertEqual(self.dxy.get_k(), 0)
        self.assertEqual(self.dzz.get_i(), 0)
        self.assertEqual(self.dzz.get_j(), 0)
        self.assertEqual(self.dzz.get_k(), 2)

    def test_get_l_is_the_sum_of_the_powers(self):
        self.assertEqual(self.dxy.get_l(), 2)
        self.assertEqual(
            self.dxy.get_l(), self.dxy.get_contracted_gaussian().get_l()
        )

    def test_get_contracted_gaussian_aliases(self):
        self.dxy.get_contracted_gaussian().set_center(Point(1.0, 2.0, 3.0))
        self.assertEqual(self.dxy.get_center(), Point(1.0, 2.0, 3.0))

    def test_setters(self):
        self.dxy.set_i(2)
        self.dxy.set_j(0)
        self.dxy.set_k(1)
        self.assertEqual(self.dxy.get_i(), 2)
        self.assertEqual(self.dxy.get_j(), 0)
        self.assertEqual(self.dxy.get_k(), 1)

    def test_setters_keep_the_contractions_l_in_sync(self):
        self.dxy.set_k(2)
        self.assertEqual(self.dxy.get_l(), 4)
        self.assertEqual(self.dxy.get_contracted_gaussian().get_l(), 4)

    def test_normalization_constant(self):
        # N^AO_xy = sqrt(3); N^AO_zz = 1
        cg = self.dxy.get_contracted_gaussian()
        self.assertAlmostEqual(
            self.dxy.normalization_constant(),
            math.sqrt(3.0) * cg.normalization_constant(),
            places=14,
        )
        cg_zz = self.dzz.get_contracted_gaussian()
        self.assertAlmostEqual(
            self.dzz.normalization_constant(),
            cg_zz.normalization_constant(),
            places=14,
        )

    def test_normalization_constant_of_an_s_function_is_one(self):
        s = CartesianAO([1.0], [1.2], 0, 0, 0, 0.0, 0.0, 0.0)
        self.assertAlmostEqual(s.normalization_constant(), 1.0, places=14)

    def test_evaluate(self):
        # At the center the monomial vanishes for any l > 0.
        self.assertEqual(self.dxy.evaluate(self.origin), 0.0)

        x, y, z = 0.3, -0.4, 0.5
        r2 = x * x + y * y + z * z
        self.assertAlmostEqual(
            self.dxy.evaluate(self.r), x * y * math.exp(-1.2 * r2), places=12
        )
        corr_zz = z * z * (math.exp(-1.0 * r2) + math.exp(-2.0 * r2))
        self.assertAlmostEqual(self.dzz.evaluate(self.r), corr_zz, places=12)

    def test_evaluate_measures_the_monomial_from_the_center(self):
        shifted = CartesianAO([1.0], [1.2], 1, 0, 0, 1.0, 0.0, 0.0)
        self.assertEqual(shifted.evaluate(Point(1.0, 5.0, 5.0)), 0.0)
        self.assertNotEqual(shifted.evaluate(self.origin), 0.0)

    def test_evaluate_accepts_any_kind_of_point(self):
        corr = self.dxy.evaluate(self.r)
        self.assertEqual(self.dxy.evaluate(PointView(self.r)), corr)
        self.assertEqual(self.dxy.evaluate(ImmutablePointView(self.r)), corr)

    def test_evaluate_point_set(self):
        rv = self.dxy.evaluate(PointSet([self.origin, self.r]))
        self.assertEqual(
            rv, [self.dxy.evaluate(self.origin), self.dxy.evaluate(self.r)]
        )

    def test_normalized_evaluate(self):
        cg = self.dxy.get_contracted_gaussian()
        corr = math.sqrt(3.0) * 0.3 * -0.4 * cg.normalized_evaluate(self.r)
        self.assertAlmostEqual(
            self.dxy.normalized_evaluate(self.r), corr, places=12
        )

    def test_normalized_evaluate_is_not_n_times_evaluate(self):
        naive = self.dzz.normalization_constant() * self.dzz.evaluate(self.r)
        self.assertNotAlmostEqual(
            self.dzz.normalized_evaluate(self.r), naive, places=12
        )

    def test_normalized_evaluate_point_set(self):
        rv = self.dxy.normalized_evaluate(PointSet([self.origin, self.r]))
        corr = [
            self.dxy.normalized_evaluate(self.origin),
            self.dxy.normalized_evaluate(self.r),
        ]
        self.assertEqual(rv, corr)

    def test_comparisons(self):
        same = CartesianAO([1.0], [1.2], 1, 1, 0, 0.0, 0.0, 0.0)
        self.assertEqual(self.dxy, same)
        self.assertFalse(self.dxy != same)

        # Different monomial, contraction, and center, respectively.
        dxz = CartesianAO([1.0], [1.2], 1, 0, 1, 0.0, 0.0, 0.0)
        diff_exp = CartesianAO([1.0], [9.9], 1, 1, 0, 0.0, 0.0, 0.0)
        moved = CartesianAO([1.0], [1.2], 1, 1, 0, 1.0, 0.0, 0.0)
        self.assertNotEqual(self.dxy, dxz)
        self.assertNotEqual(self.dxy, diff_exp)
        self.assertNotEqual(self.dxy, moved)
        self.assertNotEqual(CartesianAO(), self.dxy)

    def test_clone_is_a_deep_copy_of_the_same_kind(self):
        copy = self.dxy.clone()
        self.assertIs(type(copy), CartesianAO)
        self.assertEqual(copy, self.dxy)

        copy.set_i(2)
        self.assertEqual(self.dxy.get_i(), 1)

    def test_are_equal(self):
        same = CartesianAO([1.0], [1.2], 1, 1, 0, 0.0, 0.0, 0.0)
        self.assertTrue(self.dxy.are_equal(same))
        self.assertFalse(self.dxy.are_different(same))
        self.assertFalse(self.dxy.are_equal(self.dzz))
        self.assertTrue(self.dxy.are_different(self.dzz))


if __name__ == "__main__":
    unittest.main(verbosity=2)
