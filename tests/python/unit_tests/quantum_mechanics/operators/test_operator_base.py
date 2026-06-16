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

from chemist.qm_operator import CoreHamiltonian, KineticElectron


class TestOperatorBase(unittest.TestCase):
    def setUp(self):
        self.kinetic = KineticElectron()
        self.defaulted = CoreHamiltonian()
        self.with_operators = CoreHamiltonian()
        self.with_operators.emplace_back(1.0, self.kinetic.clone())

    def test_clone(self):
        clone = self.with_operators.clone()
        self.assertTrue(clone.are_equal(self.with_operators))

    def test_equality(self):
        other = CoreHamiltonian()
        self.assertFalse(self.with_operators.are_equal(other))
        self.assertTrue(self.with_operators.are_different(other))

        other.emplace_back(1.0, self.kinetic.clone())
        self.assertTrue(self.with_operators.are_equal(other))
        self.assertFalse(self.with_operators.are_different(other))
