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

from chemist.qm_operator import (
    CoreHamiltonian,
    CoulombManyElectronsNuclei,
    KineticManyElectrons,
)


class TestCoreHamiltonian(unittest.TestCase):
    def setUp(self):
        self.kinetic = KineticManyElectrons()
        self.coulomb = CoulombManyElectronsNuclei()
        self.defaulted = CoreHamiltonian()
        self.with_operators = CoreHamiltonian()
        self.with_operators.emplace_back(1.0, self.kinetic.clone())
        self.with_operators.emplace_back(2.0, self.coulomb.clone())

    def test_size(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.with_operators.size(), 2)

    def test_coefficient(self):
        self.assertEqual(self.with_operators.coefficient(0), 1.0)
        self.assertEqual(self.with_operators.coefficient(1), 2.0)

    def test_get_operator(self):
        got_kinetic = self.with_operators.get_operator(0)
        got_coulomb = self.with_operators.get_operator(1)
        self.assertTrue(got_kinetic.are_equal(self.kinetic))
        self.assertTrue(got_coulomb.are_equal(self.coulomb))

    def test_emplace_back(self):
        self.defaulted.emplace_back(3.0, self.kinetic.clone())
        self.assertEqual(self.defaulted.size(), 1)
        self.assertEqual(self.defaulted.coefficient(0), 3.0)
        self.assertTrue(
            self.with_operators.get_operator(0).are_equal(self.kinetic)
        )

    def test_equality(self):
        other = CoreHamiltonian()
        self.assertEqual(self.defaulted, other)

        other.emplace_back(1.0, self.kinetic.clone())
        other.emplace_back(2.0, self.coulomb.clone())
        self.assertEqual(self.with_operators, other)

        self.assertNotEqual(self.defaulted, self.with_operators)
