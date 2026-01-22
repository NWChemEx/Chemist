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

from chemist.qm_operator import xc_functional


class TestExchangeCorrelation(unittest.TestCase):
    def setUp(self):
        self.none = xc_functional.NONE
        self.custom = xc_functional.CUSTOM

    def test_functional_name(self):
        self.assertEqual(self.none.name, "NONE")
        self.assertEqual(self.custom.name, "CUSTOM")

    def test_functional_value(self):
        self.assertEqual(self.none.value, 0)
        self.assertEqual(self.custom.value, 1)
