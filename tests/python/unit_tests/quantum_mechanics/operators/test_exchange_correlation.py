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
from chemist.qm_operator import (
    ExchangeCorrelation,
    Kinetic,
    OperatorBase,
    xc_functional,
)
from chemist.wavefunction import AOs
from tensorwrapper import Tensor


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


class TestExchangeCorrelationDispatch(unittest.TestCase):
    """Tests the single ExchangeCorrelation class Python sees.

    See TestKineticDispatch. Unlike the other operators the first argument is
    the functional, so dispatch is on the second and third arguments.
    """

    def setUp(self):
        self.tensor = Tensor(np.array([[1.0, 2.0], [3.0, 4.0]]))
        self.aos = AOs(AOBasisSetD())
        self.density = Density(self.tensor, self.aos)
        self.decomp_density = DecomposableDensity(
            self.tensor, self.aos, self.tensor
        )
        self.xc = xc_functional.PBE

    def test_dispatches_on_particles(self):
        cases = [
            (
                Electron(),
                self.density,
                "_ExchangeCorrelationElectronDensityElectron",
            ),
            (
                ManyElectrons(4),
                self.density,
                "_ExchangeCorrelationManyElectronsDensityElectron",
            ),
        ]
        for lhs, rhs, name in cases:
            with self.subTest(name=name):
                op = ExchangeCorrelation(self.xc, lhs, rhs)
                self.assertEqual(type(op).__name__, name)
                self.assertEqual(op.functional_name, self.xc)

    def test_decomposable_density_beats_density(self):
        # N.B. DecomposableDensity derives from Density, so a dispatcher that
        #      checked Density first would silently build the plain-density
        #      instantiation here.
        self.assertEqual(
            type(
                ExchangeCorrelation(self.xc, Electron(), self.decomp_density)
            ).__name__,
            "_ExchangeCorrelationElectronDecomposableDensityElectron",
        )
        self.assertEqual(
            type(
                ExchangeCorrelation(
                    self.xc, ManyElectrons(4), self.decomp_density
                )
            ).__name__,
            "_ExchangeCorrelationManyElectronsDecomposableDensityElectron",
        )

    def test_default_is_electron_density(self):
        self.assertEqual(
            type(ExchangeCorrelation()).__name__,
            "_ExchangeCorrelationElectronDensityElectron",
        )
        self.assertEqual(
            ExchangeCorrelation(),
            ExchangeCorrelation(xc_functional.NONE, Electron(), Density()),
        )

    def test_isinstance(self):
        xc_e = ExchangeCorrelation(self.xc, Electron(), self.density)
        self.assertIsInstance(xc_e, ExchangeCorrelation)
        self.assertIsInstance(xc_e, OperatorBase)

        self.assertNotIsInstance(Kinetic(), ExchangeCorrelation)
        self.assertNotIsInstance(Electron(), ExchangeCorrelation)

    def test_unsupported_combination_throws(self):
        with self.assertRaisesRegex(
            TypeError, "a Electron with a ManyElectrons"
        ):
            ExchangeCorrelation(self.xc, Electron(), ManyElectrons(4))

    def test_wrong_number_of_arguments_throws(self):
        with self.assertRaisesRegex(
            TypeError, "a functional and two particles"
        ):
            ExchangeCorrelation(self.xc, Electron())


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
        self.none = xc_functional.NONE

        # Test objects
        self.default1 = ExchangeCorrelation(self.none, Electron(), Density())
        self.default2 = ExchangeCorrelation(
            self.none, ManyElectrons(), DecomposableDensity()
        )
        self.has_value1 = ExchangeCorrelation(
            self.xc, Electron(), self.density
        )
        self.has_value2 = ExchangeCorrelation(
            self.xc, self.many_elec, self.decomp_density
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
            self.default1
            == ExchangeCorrelation(self.none, Electron(), Density())
        )
        self.assertTrue(
            self.default2
            == ExchangeCorrelation(
                self.none, ManyElectrons(), DecomposableDensity()
            )
        )

        self.assertTrue(
            self.has_value1
            == ExchangeCorrelation(self.xc, Electron(), self.density)
        )
        self.assertTrue(
            self.has_value2
            == ExchangeCorrelation(
                self.xc, self.many_elec, self.decomp_density
            )
        )

        self.assertFalse(self.default1 == self.default2)
        self.assertFalse(self.default1 == self.has_value1)
        self.assertFalse(self.default2 == self.has_value2)
