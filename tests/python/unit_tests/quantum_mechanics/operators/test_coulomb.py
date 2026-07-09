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

import numpy as np
from chemist import (
    DecomposableDensity,
    Density,
    Electron,
    ManyElectrons,
    Nuclei,
    Nucleus,
)
from chemist.basis_set import AOBasisSetD
from chemist.qm_operator import (
    CoulombElectronDensityElectron,
    CoulombManyElectronsDecomposableDensityElectron,
    CoulombNucleiNuclei,
)
from chemist.wavefunction import AOs
from tensorwrapper import Tensor


class TestCoulomb(unittest.TestCase):
    def setUp(self):
        # Input Values
        self.nucleus = Nucleus("H", 1, 1.0, 2.0, 3.0, 4.0, 5.0)
        self.nuclei = Nuclei()
        self.nuclei.push_back(self.nucleus)
        self.many_elec = ManyElectrons(4)
        self.tensor = Tensor(np.array([[1.0, 2.0], [3.0, 4.0]]))
        self.basis = AOBasisSetD()
        self.aos = AOs(self.basis)
        self.density = Density(self.tensor, self.aos)
        self.decomp_density = DecomposableDensity(
            self.tensor, self.aos, self.tensor
        )

        # Test objects
        self.default1 = CoulombElectronDensityElectron()
        self.default2 = CoulombManyElectronsDecomposableDensityElectron()
        self.default3 = CoulombNucleiNuclei()
        self.has_value1 = CoulombElectronDensityElectron(
            Electron(), self.density
        )
        self.has_value2 = CoulombManyElectronsDecomposableDensityElectron(
            self.many_elec, self.decomp_density
        )
        self.has_value3 = CoulombNucleiNuclei(self.nuclei, self.nuclei)

    def test_particles(self):
        self.assertEqual(self.default1.lhs_particle, Electron())
        self.assertEqual(self.default1.rhs_particle, Density())
        self.assertEqual(self.default2.lhs_particle, ManyElectrons())
        self.assertEqual(self.default2.rhs_particle, DecomposableDensity())
        self.assertEqual(self.default3.lhs_particle, Nuclei())
        self.assertEqual(self.default3.rhs_particle, Nuclei())

        # Check each particle type for writing
        self.default1.rhs_particle = self.density
        self.assertEqual(self.default1.rhs_particle, self.density)
        self.default2.lhs_particle = self.many_elec
        self.assertEqual(self.default2.lhs_particle, self.many_elec)
        self.default2.rhs_particle = self.decomp_density
        self.assertEqual(self.default2.rhs_particle, self.decomp_density)
        self.default3.lhs_particle = self.nuclei
        self.assertEqual(self.default3.lhs_particle, self.nuclei)

        # # Should not be able to set incompatible particle types
        with self.assertRaises(TypeError):
            self.default1.lhs_particle = self.many_elec
        with self.assertRaises(TypeError):
            self.default1.rhs_particle = self.many_elec

    def test_comparison(self):
        self.assertTrue(self.default1 == CoulombElectronDensityElectron())
        self.assertTrue(
            self.default2 == CoulombManyElectronsDecomposableDensityElectron()
        )
        self.assertTrue(self.default3 == CoulombNucleiNuclei())

        self.assertTrue(
            self.has_value1
            == CoulombElectronDensityElectron(Electron(), self.density)
        )
        self.assertTrue(
            self.has_value2
            == CoulombManyElectronsDecomposableDensityElectron(
                self.many_elec, self.decomp_density
            )
        )
        self.assertTrue(
            self.has_value3 == CoulombNucleiNuclei(self.nuclei, self.nuclei)
        )

        self.assertFalse(self.default1 == self.default2)
        self.assertFalse(self.default1 == self.has_value1)
        self.assertFalse(self.default2 == self.has_value2)
        self.assertFalse(self.default3 == self.has_value3)
