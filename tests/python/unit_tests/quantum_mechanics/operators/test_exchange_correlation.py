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
    ExchangeCorrelationElectronDensityElectron,
    ExchangeCorrelationManyElectronsDecomposableDensityElectron,
    xc_functional,
)
from chemist.wavefunction import AOs
from tensorwrapper import Tensor

from chemist import DecomposableDensity, Density, Electron, ManyElectrons
from chemist.basis_set import AOBasisSetD


class TestXCFunctional(unittest.TestCase):
    def setUp(self):
        self.none = xc_functional.NONE
        self.custom = xc_functional.CUSTOM

    def test_functional_name(self):
        self.assertEqual(self.none.name, "NONE")
        self.assertEqual(self.custom.name, "CUSTOM")

    def test_functional_value(self):
        self.assertEqual(self.none.value, 0)
        self.assertEqual(self.custom.value, 1)


class TestExchangeCorrelation(unittest.TestCase):
    def setUp(self):
        # Input Values
        self.many_elec = ManyElectrons(4)
        self.tensor = Tensor(np.array([[1.0, 2.0], [3.0, 4.0]]))
        self.basis = AOBasisSetD()
        self.aos = AOs(self.basis)
        self.density = Density(self.tensor, self.aos)
        self.decomp_density = DecomposableDensity(
            self.tensor, self.aos, self.tensor
        )
        self.xc = xc_functional.PBE

        # Test objects
        self.default1 = ExchangeCorrelationElectronDensityElectron()
        self.default2 = (
            ExchangeCorrelationManyElectronsDecomposableDensityElectron()
        )
        self.has_value1 = ExchangeCorrelationElectronDensityElectron(
            self.xc, Electron(), self.density
        )
        self.has_value2 = (
            ExchangeCorrelationManyElectronsDecomposableDensityElectron(
                self.xc, self.many_elec, self.decomp_density
            )
        )

    def test_functional_name(self):
        self.assertEqual(self.default1.functional_name, xc_functional.NONE)
        self.assertEqual(self.default2.functional_name, xc_functional.NONE)
        self.assertEqual(self.has_value1.functional_name, self.xc)
        self.assertEqual(self.has_value2.functional_name, self.xc)

        # Check for writing
        self.default1.functional_name = self.xc
        self.assertEqual(self.default1.functional_name, self.xc)

    def test_particles(self):
        self.assertEqual(self.default1.lhs_particle, Electron())
        self.assertEqual(self.default1.rhs_particle, Density())
        self.assertEqual(self.default2.lhs_particle, ManyElectrons())
        self.assertEqual(self.default2.rhs_particle, DecomposableDensity())

        # Check each particle type for writing
        self.default1.rhs_particle = self.density
        self.assertEqual(self.default1.rhs_particle, self.density)
        self.default2.lhs_particle = self.many_elec
        self.assertEqual(self.default2.lhs_particle, self.many_elec)
        self.default2.rhs_particle = self.decomp_density
        self.assertEqual(self.default2.rhs_particle, self.decomp_density)

        # # Should not be able to set incompatible particle types
        with self.assertRaises(TypeError):
            self.default1.lhs_particle = self.many_elec
        with self.assertRaises(TypeError):
            self.default1.rhs_particle = self.many_elec

    def test_comparison(self):
        self.assertTrue(
            self.default1 == ExchangeCorrelationElectronDensityElectron()
        )
        self.assertTrue(
            self.default2
            == ExchangeCorrelationManyElectronsDecomposableDensityElectron()
        )

        self.assertTrue(
            self.has_value1
            == ExchangeCorrelationElectronDensityElectron(
                self.xc, Electron(), self.density
            )
        )
        self.assertTrue(
            self.has_value2
            == ExchangeCorrelationManyElectronsDecomposableDensityElectron(
                self.xc, self.many_elec, self.decomp_density
            )
        )

        self.assertFalse(self.default1 == self.default2)
        self.assertFalse(self.default1 == self.has_value1)
        self.assertFalse(self.default2 == self.has_value2)
