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
    AO,
    AOView,
    CartesianAO,
    CartesianAOView,
    ImmutableCartesianAOView,
    ImmutableSphericalAOView,
    Point,
    PointSet,
    SphericalAO,
)

CS = [2.0, 3.0]
ES = [1.0, 2.0]


class TestAO(unittest.TestCase):
    """Drives AO and AOView through their concrete derived classes.

    AO and AOView are abstract, so the point of exposing them is that code can
    work with "some AO" without knowing which kind. These tests check that the
    base-class methods dispatch to, and hand back, the right concrete kind.
    """

    def setUp(self):
        self.r = Point(0.3, -0.4, 0.5)
        self.cart = CartesianAO(CS, ES, 1, 1, 0, 1.0, 2.0, 3.0)
        self.pure = SphericalAO(CS, ES, 2, -2, 1.0, 2.0, 3.0)

    def test_aos_can_not_be_made_directly(self):
        with self.assertRaises(TypeError):
            AO()
        with self.assertRaises(TypeError):
            AOView()

    def test_the_shared_api_agrees_with_the_derived_class(self):
        for ao in (self.cart, self.pure):
            with self.subTest(ao=type(ao)):
                self.assertIsInstance(ao, AO)
                self.assertEqual(AO.get_l(ao), 2)
                self.assertEqual(AO.get_center(ao), Point(1.0, 2.0, 3.0))
                self.assertEqual(
                    AO.get_contracted_gaussian(ao),
                    ao.get_contracted_gaussian(),
                )
                self.assertEqual(
                    AO.normalization_constant(ao), ao.normalization_constant()
                )
                self.assertEqual(AO.evaluate(ao, self.r), ao.evaluate(self.r))
                self.assertEqual(
                    AO.normalized_evaluate(ao, self.r),
                    ao.normalized_evaluate(self.r),
                )
                points = PointSet([self.r])
                self.assertEqual(
                    AO.evaluate(ao, points), [ao.evaluate(self.r)]
                )

    def test_clone_returns_the_same_kind_of_ao(self):
        self.assertIs(type(self.cart.clone()), CartesianAO)
        self.assertIs(type(self.pure.clone()), SphericalAO)

    def test_are_equal_dispatches_on_the_kind_of_ao(self):
        self.assertTrue(self.cart.are_equal(self.cart.clone()))
        self.assertTrue(self.pure.are_equal(self.pure.clone()))

        # A Cartesian and a spherical AO are never equal
        self.assertFalse(self.cart.are_equal(self.pure))
        self.assertTrue(self.pure.are_different(self.cart))
        self.assertNotEqual(self.cart, self.pure)
        self.assertNotEqual(self.pure, self.cart)

    def test_views_dispatch_too(self):
        cv = ImmutableCartesianAOView(self.cart)
        pv = ImmutableSphericalAOView(self.pure)
        mv = CartesianAOView(self.cart)
        for view, ao in ((cv, self.cart), (pv, self.pure), (mv, self.cart)):
            with self.subTest(view=type(view)):
                self.assertIsInstance(view, AOView)
                self.assertEqual(AOView.get_l(view), ao.get_l())
                self.assertEqual(
                    AOView.evaluate(view, self.r), ao.evaluate(self.r)
                )
                self.assertIs(type(view.clone()), type(view))
                self.assertIs(type(view.as_ao()), type(ao))
                self.assertEqual(view.as_ao(), ao)

        # Views of different kinds of AO are never equal
        self.assertFalse(cv.are_equal(pv))
        self.assertTrue(cv.are_different(pv))


if __name__ == "__main__":
    unittest.main(verbosity=2)
