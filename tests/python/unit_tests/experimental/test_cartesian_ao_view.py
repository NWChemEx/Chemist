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
    AOView,
    CartesianAO,
    CartesianAOView,
    ContractedGaussian,
    ImmutableCartesianAOView,
    Point,
)


class TestCartesianAOView(unittest.TestCase):
    def setUp(self):
        self.r = Point(0.3, -0.4, 0.5)
        self.dxy = CartesianAO([2.0, 3.0], [1.0, 2.0], 1, 1, 0, 1.0, 2.0, 3.0)
        self.v = CartesianAOView(self.dxy)
        self.cv = ImmutableCartesianAOView(self.dxy)

    def test_there_is_no_default_ctor(self):
        with self.assertRaises(TypeError):
            CartesianAOView()
        with self.assertRaises(TypeError):
            ImmutableCartesianAOView()

    def test_is_an_ao_view(self):
        self.assertIsInstance(self.v, AOView)
        self.assertIsInstance(self.cv, AOView)

    def test_from_a_cartesian_ao_aliases_it(self):
        self.assertEqual(self.v, self.dxy)
        self.assertEqual(self.cv, self.dxy)

        self.dxy.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(self.v.get_center(), Point(4.0, 5.0, 6.0))
        self.assertEqual(self.cv.get_center(), Point(4.0, 5.0, 6.0))

    def test_from_an_aliased_contracted_gaussian(self):
        cg = ContractedGaussian([2.0, 3.0], [1.0, 2.0], 2, 1.0, 2.0, 3.0)
        v = CartesianAOView(cg, 1, 1, 0)
        cv = ImmutableCartesianAOView(cg, 1, 1, 0)
        self.assertEqual(v, self.dxy)
        self.assertEqual(cv, self.dxy)

        cg.set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(v.get_center(), Point(4.0, 5.0, 6.0))
        self.assertEqual(cv.get_center(), Point(4.0, 5.0, 6.0))

    def test_powers_must_match_l(self):
        cg = ContractedGaussian([2.0, 3.0], [1.0, 2.0], 2, 1.0, 2.0, 3.0)
        with self.assertRaises(ValueError):
            CartesianAOView(cg, 1, 0, 0)
        with self.assertRaises(ValueError):
            ImmutableCartesianAOView(cg, 1, 0, 0)

    def test_mutable_to_read_only_conversion(self):
        self.assertEqual(ImmutableCartesianAOView(self.v), self.dxy)

    def test_writing_through_the_view(self):
        self.v.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(self.dxy.get_center(), Point(4.0, 5.0, 6.0))

        # The powers are the view's own, but l lives in the contraction
        self.v.set_k(1)
        self.assertEqual(self.v.get_l(), 3)
        self.assertEqual(self.dxy.get_contracted_gaussian().get_l(), 3)

    def test_read_only_view_can_not_be_written_through(self):
        for setter in ("set_i", "set_j", "set_k"):
            self.assertFalse(hasattr(self.cv, setter))
            self.assertTrue(hasattr(self.v, setter))
        self.assertFalse(hasattr(self.cv.get_contracted_gaussian(), "set_l"))
        self.assertTrue(hasattr(self.v.get_contracted_gaussian(), "set_l"))

    def test_evaluation_agrees_with_the_aliased_ao(self):
        for view in (self.v, self.cv):
            self.assertEqual(view.evaluate(self.r), self.dxy.evaluate(self.r))
            self.assertEqual(
                view.normalized_evaluate(self.r),
                self.dxy.normalized_evaluate(self.r),
            )
            self.assertEqual(
                view.normalization_constant(),
                self.dxy.normalization_constant(),
            )

    def test_as_cartesian_ao_is_a_deep_copy(self):
        for view in (self.v, self.cv):
            copy = view.as_cartesian_ao()
            self.assertIs(type(copy), CartesianAO)
            self.assertEqual(copy, self.dxy)

            copy.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0))
            self.assertEqual(self.dxy.get_center(), Point(1.0, 2.0, 3.0))

    def test_as_ao_materializes_a_cartesian_ao(self):
        copy = self.cv.as_ao()
        self.assertIs(type(copy), CartesianAO)
        self.assertEqual(copy, self.dxy)

    def test_clone_is_a_shallow_copy_of_the_same_kind(self):
        copy = self.cv.clone()
        self.assertIs(type(copy), ImmutableCartesianAOView)
        self.dxy.get_contracted_gaussian().set_center(Point(8.0, 8.0, 8.0))
        self.assertEqual(copy.get_center(), Point(8.0, 8.0, 8.0))

    def test_comparisons(self):
        dxz = CartesianAO([2.0, 3.0], [1.0, 2.0], 1, 0, 1, 1.0, 2.0, 3.0)
        self.assertEqual(self.v, self.dxy)
        self.assertEqual(self.dxy, self.v)
        self.assertEqual(self.dxy, self.cv)
        self.assertEqual(self.cv, self.v)
        self.assertEqual(self.v, self.cv)
        self.assertNotEqual(self.v, dxz)
        self.assertNotEqual(dxz, self.cv)
        self.assertTrue(self.cv.are_equal(self.cv.clone()))

    def test_views_keep_what_they_alias_alive(self):
        # keep_alive holds a reference to the aliased object. Counting the
        # references is how this is observed without relying on a crash.
        before = sys.getrefcount(self.dxy)
        view = CartesianAOView(self.dxy)
        self.assertEqual(sys.getrefcount(self.dxy), before + 1)
        before_view = sys.getrefcount(view)
        cg = view.get_contracted_gaussian()
        self.assertEqual(sys.getrefcount(view), before_view + 1)
        del view, cg
        self.assertEqual(sys.getrefcount(self.dxy), before)

    def test_view_outlives_the_python_name_of_the_ao(self):
        ao = CartesianAO([2.0, 3.0], [1.0, 2.0], 1, 1, 0, 1.0, 2.0, 3.0)
        view = ImmutableCartesianAOView(ao)
        del ao
        self.assertEqual(view, self.dxy)


if __name__ == "__main__":
    unittest.main(verbosity=2)
