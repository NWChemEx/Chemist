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
from chemist.qm_operator import (
    ExchangeElectronDensityElectron,
    ExchangeManyElectronsDensityElectron,
)
from chemist.wavefunction import AOs
from tensorwrapper import Tensor

from chemist import Density, Electron, ManyElectrons
from chemist.basis_set import AOBasisSetD


class TestExchange(unittest.TestCase):
    def setUp(self):
        self.np_value = np.array([[1.0, 2.0], [3.0, 4.0]])
        self.value = Tensor(self.np_value)
        self.basis = AOBasisSetD()
        self.aos = AOs(self.basis)
        self.density = Density(self.value, self.aos)
        self.many_elec = ManyElectrons(2)

        self.exchange1 = ExchangeElectronDensityElectron()
        self.exchange2 = ExchangeManyElectronsDensityElectron()
        self.exchange3 = ExchangeElectronDensityElectron(
            Electron(), self.density
        )
        self.exchange4 = ExchangeManyElectronsDensityElectron(
            self.many_elec, self.density
        )

    def test_particles(self):
        self.assertEqual(self.exchange1.lhs_particle, Electron())
        self.assertEqual(self.exchange2.lhs_particle, ManyElectrons())
        self.assertEqual(self.exchange3.lhs_particle, Electron())
        self.assertEqual(self.exchange4.lhs_particle, self.many_elec)

        self.assertEqual(self.exchange1.rhs_particle, Density())
        self.assertEqual(self.exchange2.rhs_particle, Density())
        self.assertEqual(self.exchange3.rhs_particle, self.density)
        self.assertEqual(self.exchange4.rhs_particle, self.density)

        # Check for writing
        self.exchange2.lhs_particle = self.many_elec
        self.assertEqual(self.exchange2.lhs_particle, self.many_elec)
        self.exchange1.rhs_particle = self.density
        self.assertEqual(self.exchange1.rhs_particle, self.density)

        # Should not be able to set incompatible particle types
        with self.assertRaises(TypeError):
            self.exchange1.lhs_particle = ManyElectrons()
        with self.assertRaises(TypeError):
            self.exchange2.lhs_particle = Electron()

    def test_comparison(self):
        self.assertTrue(self.exchange1 == ExchangeElectronDensityElectron())
        self.assertTrue(
            self.exchange2 == ExchangeManyElectronsDensityElectron()
        )
        self.assertTrue(
            self.exchange3
            == ExchangeElectronDensityElectron(Electron(), self.density)
        )
        self.assertTrue(
            self.exchange4
            == ExchangeManyElectronsDensityElectron(
                self.many_elec, self.density
            )
        )
