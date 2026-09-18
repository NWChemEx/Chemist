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
    ContractedGaussian,
    ContractedGaussianView,
    Point,
)


class TestContractedGaussianView(unittest.TestCase):
    def setUp(self):
        self.s = ContractedGaussian([2.0, 3.0], [1.0, 2.0], 1, 1.0, 2.0, 3.0)
        self.other = ContractedGaussian(
            [9.0, 9.0], [9.0, 9.0], 2, 9.0, 9.0, 9.0
        )
        self.sv = ContractedGaussianView(self.s)

    def test_aliases_the_original(self):
        self.assertEqual(self.sv.get_l(), 1)
        self.s.set_l(4)
        self.assertEqual(self.sv.get_l(), 4)

    def test_as_contracted_gaussian_is_a_deep_copy(self):
        copy = self.sv.as_contracted_gaussian()
        self.assertEqual(copy, self.s)
        copy.set_l(7)
        self.assertEqual(self.s.get_l(), 1)

    def test_writing_through_the_view_mutates_the_original(self):
        self.sv.set_l(6)
        self.sv.set_center(Point(4.0, 5.0, 6.0))
        self.assertEqual(self.s.get_l(), 6)
        self.assertEqual(self.s.get_center(), Point(4.0, 5.0, 6.0))

    def test_comparisons(self):
        self.assertEqual(self.sv, self.s)
        self.assertNotEqual(self.sv, self.other)


if __name__ == "__main__":
    unittest.main(verbosity=2)
