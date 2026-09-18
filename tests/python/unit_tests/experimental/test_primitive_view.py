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
    ImmutablePrimitiveView,
    Primitive,
    PrimitiveView,
)


class TestPrimitiveView(unittest.TestCase):
    def setUp(self):
        self.s = Primitive(2.0, 1.0, 1, 1.0, 2.0, 3.0)
        self.other = Primitive(9.0, 9.0, 2, 9.0, 9.0, 9.0)
        self.sv = PrimitiveView(self.s)
        self.csv = ImmutablePrimitiveView(self.other)

    def test_ctor_aliases(self):
        self.assertEqual(self.sv.get_coefficient(), 2.0)

        # The view aliases self.s; it did not copy it
        self.s.set_coefficient(42.0)
        self.assertEqual(self.sv.get_coefficient(), 42.0)

    def test_set_writes_through(self):
        self.sv.set_coefficient(4.0)
        self.sv.set_exponent(5.0)
        self.sv.set_l(6)
        self.assertEqual(self.s.get_coefficient(), 4.0)
        self.assertEqual(self.s.get_exponent(), 5.0)
        self.assertEqual(self.s.get_l(), 6)

    def test_read_only_view_has_no_setters(self):
        self.assertFalse(hasattr(self.csv, "set_coefficient"))
        self.assertFalse(hasattr(self.csv, "set_l"))
        self.assertTrue(hasattr(self.sv, "set_coefficient"))

    def test_as_primitive_is_a_deep_copy(self):
        copy = self.sv.as_primitive()
        self.assertEqual(copy, self.s)

        copy.set_l(4)
        self.assertEqual(self.s.get_l(), 1)

    def test_comparisons(self):
        self.assertEqual(self.sv, self.s)
        self.assertNotEqual(self.sv, self.other)


if __name__ == "__main__":
    unittest.main(verbosity=2)
