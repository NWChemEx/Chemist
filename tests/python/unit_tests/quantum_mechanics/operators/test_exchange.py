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
from chemist import DecomposableDensity, Density, Electron, ManyElectrons
from chemist.basis_set import AOBasisSetD
from chemist.qm_operator import Exchange, Kinetic, OperatorBase
from chemist.wavefunction import AOs
from tensorwrapper import Tensor


class TestExchangeDispatch(unittest.TestCase):
    """Tests the single Exchange class Python sees. See TestKineticDispatch."""

    def setUp(self):
        self.tensor = Tensor(np.array([[1.0, 2.0], [3.0, 4.0]]))
        self.aos = AOs(AOBasisSetD())
        self.density = Density(self.tensor, self.aos)
        self.decomp_density = DecomposableDensity(
            self.tensor, self.aos, self.tensor
        )

    def test_dispatches_on_particles(self):
        cases = [
            (Electron(), self.density, "_ExchangeElectronDensityElectron"),
            (
                ManyElectrons(4),
                self.density,
                "_ExchangeManyElectronsDensityElectron",
            ),
        ]
        for lhs, rhs, name in cases:
            with self.subTest(name=name):
                self.assertEqual(type(Exchange(lhs, rhs)).__name__, name)

    def test_decomposable_density_beats_density(self):
        # N.B. DecomposableDensity derives from Density, so a dispatcher that
        #      checked Density first would silently build the plain-density
        #      instantiation here.
        self.assertEqual(
            type(Exchange(Electron(), self.decomp_density)).__name__,
            "_ExchangeElectronDecomposableDensityElectron",
        )
        self.assertEqual(
            type(Exchange(ManyElectrons(4), self.decomp_density)).__name__,
            "_ExchangeManyElectronsDecomposableDensityElectron",
        )

    def test_default_is_electron_density(self):
        self.assertEqual(
            type(Exchange()).__name__, "_ExchangeElectronDensityElectron"
        )
        self.assertEqual(Exchange(), Exchange(Electron(), Density()))

    def test_isinstance(self):
        k_e = Exchange(Electron(), self.density)
        self.assertIsInstance(k_e, Exchange)
        self.assertIsInstance(k_e, OperatorBase)

        self.assertNotIsInstance(Kinetic(), Exchange)
        self.assertNotIsInstance(Electron(), Exchange)

    def test_unsupported_combination_throws(self):
        # Exchange is always with a density, never with another particle.
        with self.assertRaisesRegex(
            TypeError, "a Electron with a ManyElectrons"
        ):
            Exchange(Electron(), ManyElectrons(4))

    def test_wrong_number_of_particles_throws(self):
        with self.assertRaisesRegex(TypeError, "takes two particles"):
            Exchange(Electron())


class TestExchange(unittest.TestCase):
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

        # Test objects
        self.default1 = Exchange(Electron(), Density())
        self.default2 = Exchange(ManyElectrons(), DecomposableDensity())
        self.has_value1 = Exchange(Electron(), self.density)
        self.has_value2 = Exchange(self.many_elec, self.decomp_density)

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
        self.assertTrue(self.default1 == Exchange(Electron(), Density()))
        self.assertTrue(
            self.default2 == Exchange(ManyElectrons(), DecomposableDensity())
        )

        self.assertTrue(self.has_value1 == Exchange(Electron(), self.density))
        self.assertTrue(
            self.has_value2 == Exchange(self.many_elec, self.decomp_density)
        )

        self.assertFalse(self.default1 == self.default2)
        self.assertFalse(self.default1 == self.has_value1)
        self.assertFalse(self.default2 == self.has_value2)
