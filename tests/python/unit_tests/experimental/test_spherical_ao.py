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
    CartesianCCAShell,
    ContractedGaussian,
    ImmutableCartesianCCAShellView,
    Point,
    PointSet,
    SphericalAO,
    SphericalCCAShell,
)

CS = [2.0, 3.0]
ES = [1.0, 2.0]


def spherical(l, m):
    return SphericalAO(CS, ES, l, m, 1.0, 2.0, 3.0)


def cartesian(i, j, k):
    return CartesianAO(CS, ES, i, j, k, 1.0, 2.0, 3.0)


class TestSphericalAO(unittest.TestCase):
    def setUp(self):
        self.r = Point(0.3, -0.4, 0.5)
        self.shell = CartesianCCAShell(CS, ES, 3, 1.0, 2.0, 3.0)
        self.f = SphericalAO(self.shell, -2)
        self.d0 = spherical(2, 0)

    def test_default_ctor(self):
        defaulted = SphericalAO()
        self.assertEqual(defaulted.get_l(), 0)
        self.assertEqual(defaulted.get_m(), 0)
        self.assertEqual(len(defaulted.get_contracted_gaussian()), 0)
        self.assertEqual(defaulted.get_center(), Point(0.0, 0.0, 0.0))
        self.assertIs(
            type(defaulted.get_cartesian_shell()),
            ImmutableCartesianCCAShellView,
        )

    def test_range_ctors(self):
        self.assertEqual(spherical(3, -2), self.f)
        self.assertEqual(
            SphericalAO(CS, ES, 3, -2, Point(1.0, 2.0, 3.0)), self.f
        )

    def test_range_ctor_throws_if_m_is_too_big(self):
        with self.assertRaises(ValueError):
            spherical(1, 2)
        with self.assertRaises(ValueError):
            spherical(1, -2)

    def test_from_a_shell(self):
        self.assertEqual(self.f.get_l(), 3)
        self.assertEqual(self.f.get_m(), -2)
        self.assertEqual(self.f.get_cartesian_shell(), self.shell)

        # The shell is copied, not aliased
        self.shell.set_l(1)
        self.assertEqual(self.f.get_l(), 3)

    def test_from_a_contracted_gaussian(self):
        cg = ContractedGaussian(CS, ES, 3, 1.0, 2.0, 3.0)
        self.assertEqual(SphericalAO(cg, -2), self.f)

    def test_from_a_shell_rejects_a_pure_shell(self):
        # The C++ ctor only accepts a Cartesian shell, so a pure one matches
        # no overload.
        pure = SphericalCCAShell(CS, ES, 3, 1.0, 2.0, 3.0)
        with self.assertRaises(TypeError):
            SphericalAO(pure, 1)

    def test_from_a_shell_throws_if_m_is_too_big(self):
        with self.assertRaises(ValueError):
            SphericalAO(self.shell, 4)

    def test_is_an_ao(self):
        self.assertIsInstance(self.f, AO)

    def test_accessors(self):
        self.assertEqual(self.d0.get_m(), 0)
        self.assertEqual(self.d0.get_l(), 2)
        self.assertEqual(self.d0.get_center(), Point(1.0, 2.0, 3.0))
        self.assertEqual(
            self.d0.get_contracted_gaussian(),
            self.d0.get_cartesian_shell().get_contracted_gaussian(),
        )

    def test_set_m(self):
        self.d0.set_m(-2)
        self.assertEqual(self.d0.get_m(), -2)
        with self.assertRaises(ValueError):
            self.d0.set_m(3)
        with self.assertRaises(ValueError):
            self.d0.set_m(-3)
        self.assertEqual(self.d0.get_m(), -2)  # Strong guarantee

    def test_get_cartesian_shell_is_read_only(self):
        shell = self.f.get_cartesian_shell()
        self.assertIs(type(shell), ImmutableCartesianCCAShellView)
        self.assertTrue(shell.is_cartesian())
        self.assertEqual(len(shell), 10)
        self.assertFalse(hasattr(shell, "set_l"))

    def test_normalization_constant_is_that_of_the_contraction(self):
        self.assertEqual(
            self.d0.normalization_constant(),
            self.d0.get_contracted_gaussian().normalization_constant(),
        )

    def test_p_functions_are_the_cartesian_p_functions(self):
        # m = -1, 0, +1 are y, z, x
        for m, (i, j, k) in ((-1, (0, 1, 0)), (0, (0, 0, 1)), (1, (1, 0, 0))):
            with self.subTest(m=m):
                self.assertAlmostEqual(
                    spherical(1, m).normalized_evaluate(self.r),
                    cartesian(i, j, k).normalized_evaluate(self.r),
                )

    def test_d_minus_2_is_the_normalized_cartesian_d_xy(self):
        dxy = cartesian(1, 1, 0)
        d = spherical(2, -2)
        self.assertAlmostEqual(
            d.normalized_evaluate(self.r), dxy.normalized_evaluate(self.r)
        )
        self.assertAlmostEqual(
            d.evaluate(self.r), math.sqrt(3.0) * dxy.evaluate(self.r)
        )

    def test_d_0_is_zz_minus_half_xx_plus_yy(self):
        xx = cartesian(2, 0, 0).normalized_evaluate(self.r)
        yy = cartesian(0, 2, 0).normalized_evaluate(self.r)
        zz = cartesian(0, 0, 2).normalized_evaluate(self.r)
        self.assertAlmostEqual(
            self.d0.normalized_evaluate(self.r), zz - 0.5 * (xx + yy)
        )

    def test_evaluate_over_a_set_of_points(self):
        points = PointSet([Point(0.0, 0.0, 0.0), self.r])
        raw = self.d0.evaluate(points)
        norm = self.d0.normalized_evaluate(points)
        self.assertEqual(len(raw), 2)
        self.assertEqual(raw[1], self.d0.evaluate(self.r))
        self.assertEqual(norm[1], self.d0.normalized_evaluate(self.r))

    def test_comparisons(self):
        self.assertEqual(self.d0, spherical(2, 0))
        self.assertFalse(self.d0 != spherical(2, 0))
        self.assertNotEqual(self.d0, spherical(2, 1))
        self.assertNotEqual(self.d0, spherical(3, 0))

    def test_clone_is_a_deep_copy_of_the_same_kind(self):
        copy = self.f.clone()
        self.assertIs(type(copy), SphericalAO)
        self.assertEqual(copy, self.f)
        copy.set_m(1)
        self.assertEqual(self.f.get_m(), -2)


if __name__ == "__main__":
    unittest.main(verbosity=2)
