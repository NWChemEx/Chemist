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

from chemist.experimental import ContractedGaussian, Point


class TestContractedGaussian(unittest.TestCase):
    def setUp(self):
        self.defaulted = ContractedGaussian()
        self.origin = Point(0.0, 0.0, 0.0)
        self.r0 = Point(1.0, 0.0, 0.0)
        # Two-primitive s-shell: d1 = d2 = 1.0, zeta1 = 1.0, zeta2 = 2.0,
        # centered at the origin.
        self.s = ContractedGaussian([1.0, 1.0], [1.0, 2.0], 0, 0.0, 0.0, 0.0)

    def test_default_ctor(self):
        self.assertEqual(len(self.defaulted), 0)
        self.assertEqual(self.defaulted.get_l(), 0)
        self.assertEqual(self.defaulted.get_center(), self.origin)

    def test_value_ctor(self):
        self.assertEqual(len(self.s), 2)
        self.assertEqual(self.s.get_l(), 0)
        self.assertEqual(self.s.get_center(), self.origin)
        self.assertEqual(self.s[0].get_coefficient(), 1.0)
        self.assertEqual(self.s[0].get_exponent(), 1.0)
        self.assertEqual(self.s[1].get_exponent(), 2.0)

    def test_setters(self):
        self.s.set_l(2)
        self.assertEqual(self.s.get_l(), 2)
        self.s.set_center(self.r0)
        self.assertEqual(self.s.get_center(), self.r0)

    def test_iteration(self):
        coefficients = [p.get_coefficient() for p in self.s]
        self.assertEqual(coefficients, [1.0, 1.0])

    def test_evaluate(self):
        corr = 1.0 * math.exp(-1.0) + 1.0 * math.exp(-2.0)
        self.assertAlmostEqual(self.s.evaluate(self.r0), corr)

    def test_normalization_constant(self):
        s12 = (2.0 * math.sqrt(2.0) / 3.0) ** 1.5
        total = 1.0 + 1.0 + 2.0 * s12
        corr = total**-0.5
        self.assertAlmostEqual(self.s.normalization_constant(), corr)

    def test_normalized_evaluate(self):
        n_g = self.s.normalization_constant()
        corr = n_g * sum(p.normalized_evaluate(self.r0) for p in self.s)
        self.assertAlmostEqual(self.s.normalized_evaluate(self.r0), corr)

    def test_comparisons(self):
        same = ContractedGaussian([1.0, 1.0], [1.0, 2.0], 0, 0.0, 0.0, 0.0)
        self.assertEqual(self.s, same)
        self.assertNotEqual(self.s, self.defaulted)


if __name__ == "__main__":
    unittest.main(verbosity=2)
