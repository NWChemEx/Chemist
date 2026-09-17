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

from chemist.experimental import Point, Primitive


class TestPrimitive(unittest.TestCase):
    def setUp(self):
        self.defaulted = Primitive()
        self.origin = Point(0.0, 0.0, 0.0)
        self.r0 = Point(1.0, 0.0, 0.0)
        # c = 2.0, zeta = 1.0, l = 0, centered at the origin
        self.s = Primitive(2.0, 1.0, 0, 0.0, 0.0, 0.0)

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.get_coefficient(), 0.0)
        self.assertEqual(self.defaulted.get_exponent(), 0.0)
        self.assertEqual(self.defaulted.get_l(), 0)
        self.assertEqual(self.defaulted.get_center(), self.origin)

    def test_value_ctor(self):
        self.assertEqual(self.s.get_coefficient(), 2.0)
        self.assertEqual(self.s.get_exponent(), 1.0)
        self.assertEqual(self.s.get_l(), 0)
        self.assertEqual(self.s.get_center(), self.origin)

    def test_setters(self):
        self.defaulted.set_coefficient(4.0)
        self.defaulted.set_exponent(5.0)
        self.defaulted.set_l(2)
        self.assertEqual(self.defaulted.get_coefficient(), 4.0)
        self.assertEqual(self.defaulted.get_exponent(), 5.0)
        self.assertEqual(self.defaulted.get_l(), 2)

    def test_evaluate(self):
        self.assertEqual(self.s.evaluate(self.origin), 2.0)
        corr = 2.0 * math.exp(-1.0)
        self.assertAlmostEqual(self.s.evaluate(self.r0), corr)

    def test_normalization_constant(self):
        corr = (2.0 / math.pi) ** 0.75
        self.assertAlmostEqual(self.s.normalization_constant(), corr)

    def test_normalized_evaluate(self):
        n_chi = self.s.normalization_constant()
        value = self.s.evaluate(self.r0)
        self.assertAlmostEqual(
            self.s.normalized_evaluate(self.r0), n_chi * value
        )

    def test_comparisons(self):
        same = Primitive(2.0, 1.0, 0, 0.0, 0.0, 0.0)
        self.assertEqual(self.s, same)
        self.assertNotEqual(self.s, self.defaulted)


if __name__ == "__main__":
    unittest.main(verbosity=2)
