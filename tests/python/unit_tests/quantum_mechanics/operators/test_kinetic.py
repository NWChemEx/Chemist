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

from chemist.qm_operator import KineticElectron, KineticManyElectrons

from chemist import Electron, ManyElectrons


class TestKinetic(unittest.TestCase):
    def setUp(self):
        self.many_elec = ManyElectrons(4)
        self.kinetic_elec = KineticElectron()
        self.kinetic_many1 = KineticManyElectrons()
        self.kinetic_many2 = KineticManyElectrons(self.many_elec)

    def test_particle(self):
        self.assertEqual(self.kinetic_elec.particle, Electron())
        self.assertEqual(self.kinetic_many1.particle, ManyElectrons())
        self.assertEqual(self.kinetic_many2.particle, self.many_elec)

        # Check for writing
        self.kinetic_many1.particle = self.many_elec
        self.assertEqual(self.kinetic_many1.particle, self.many_elec)

        # Should not be able to set incompatible particle types
        with self.assertRaises(TypeError):
            self.kinetic_elec.particle = self.many_elec

    def test_comparison(self):
        self.assertTrue(self.kinetic_elec == KineticElectron())
        self.assertTrue(self.kinetic_many1 == KineticManyElectrons())
        self.assertTrue(
            self.kinetic_many2 == KineticManyElectrons(self.many_elec)
        )

        self.assertFalse(self.kinetic_elec == self.kinetic_many1)
        self.assertFalse(self.kinetic_many1 == self.kinetic_many2)
