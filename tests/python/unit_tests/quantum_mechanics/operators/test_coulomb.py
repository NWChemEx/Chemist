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
    CoulombElectronElectron,
    CoulombManyElectronsManyElectrons,
)

from chemist import Electron, ManyElectrons


class TestCoulomb(unittest.TestCase):
    def setUp(self):
        self.many_elec = ManyElectrons(4)
        self.coulomb_elec = CoulombElectronElectron()
        self.coulomb_many1 = CoulombManyElectronsManyElectrons()
        self.coulomb_many2 = CoulombManyElectronsManyElectrons(
            self.many_elec, self.many_elec
        )

    def test_particles(self):
        self.assertEqual(self.coulomb_elec.lhs_particle, Electron())
        self.assertEqual(self.coulomb_elec.rhs_particle, Electron())
        self.assertEqual(self.coulomb_many1.lhs_particle, ManyElectrons())
        self.assertEqual(self.coulomb_many1.rhs_particle, ManyElectrons())
        self.assertEqual(self.coulomb_many2.lhs_particle, self.many_elec)
        self.assertEqual(self.coulomb_many2.rhs_particle, self.many_elec)

        # Check for writing
        self.coulomb_many1.lhs_particle = self.many_elec
        self.assertEqual(self.coulomb_many1.lhs_particle, self.many_elec)
        self.coulomb_many1.rhs_particle = self.many_elec
        self.assertEqual(self.coulomb_many1.rhs_particle, self.many_elec)

        # Should not be able to set incompatible particle types
        with self.assertRaises(TypeError):
            self.coulomb_elec.lhs_particle = self.many_elec
        with self.assertRaises(TypeError):
            self.coulomb_elec.rhs_particle = self.many_elec

    def test_comparison(self):
        self.assertTrue(self.coulomb_elec == CoulombElectronElectron())
        self.assertTrue(
            self.coulomb_many1 == CoulombManyElectronsManyElectrons()
        )
        self.assertTrue(
            self.coulomb_many2
            == CoulombManyElectronsManyElectrons(
                self.many_elec, self.many_elec
            )
        )

        self.assertFalse(self.coulomb_elec == self.coulomb_many1)
        self.assertFalse(self.coulomb_many1 == self.coulomb_many2)
